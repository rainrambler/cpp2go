/*
 * Seccheck - A tool for security C/C++ code analysis
 * Copyright (C) 2014 Wang Anyu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "goconvertor.h"
#include "tokenize.h"
#include "symboldatabase.h"

#include <iostream>
#include <locale>
#include <vector>

using namespace std;

struct Statement
{
	Token* StartToken;
	Token* EndToken;

	// Statements Defination from CPP Standard 
	enum StatementType
	{
		LABELED_STATEMENT = 1, // label for goto 
		EXPRESSION_STATEMENT, // expression
		COMPOUND_STATEMENT, // block
		SELECTION_STATEMENT, // if, if...else..., switch
		ITERATION_STATEMENT, // while, do, for
		JUMP_STATEMENT, // break, continue, return, goto
		DECLARATION_STATEMENT, // identifier declaration
		TRY_BLOCK
	};
};

static string convertVariableName(const string& varName)
{
	string firstCharUpper = varName;
	firstCharUpper[0] = toupper(firstCharUpper[0]);
	return firstCharUpper;
}

static string findVariableType(const Variable& var)
{
	if (var.type() != nullptr)
	{
		return var.type()->name();
	}

	if (var.typeStartToken() != nullptr)
	{
		return var.typeStartToken()->str();
	}

	// TODO
	return "NA";
}

static string convertVariableDefine(const Variable& var)
{
	string varName = var.name();

	if (var.isPublic())
	{
		varName = convertVariableName(varName);
	}

	if (var.isArray())
	{
		return (varName) + " [] " + findVariableType(var);
	}
	else if (var.isPointer())
	{
		return (varName) + " * " + findVariableType(var);
	}
	else
	{
		return (varName) + " " + findVariableType(var);
	}	
}

static string array2string(const vector<string>& arr)
{
	string statement = "";
	for (auto itr = arr.begin(); itr != arr.end(); itr++)
	{
		statement += *itr + "\r\n";
	}
	return statement;
}

static string findReturnValue(const Function& func)
{
	if (func.retType != nullptr)
	{
		return func.retType->name();
	}

	if (func.retDef != nullptr)
	{
		return func.retDef->str();
	}

	return "";
}

static bool isScopeClass(const Scope* sc)
{
	if (sc == nullptr)
	{
		return false;
	}

	return (sc->type == Scope::eClass) || (sc->type == Scope::eStruct);
}

static string convertClassMember(const Token& variableToken)
{
	if (variableToken.type() != Token::eVariable)
	{
		return variableToken.str();
	}

	auto v = variableToken.variable();
	if (v == nullptr)
	{
		return variableToken.str();
	}

	auto prevToken = variableToken.previous();
	if (prevToken == nullptr)
	{
		return variableToken.str();
	}

	// member of other class
	if (prevToken->str() == ".")
	{
		return variableToken.str();
	}

	// member of other class
	if (prevToken->str() == "->")
	{
		return variableToken.str();
	}

	if (isScopeClass(variableToken.scope()))
	{
		return "parent." + variableToken.str();
	}
	else
	{
		return variableToken.str();
	}
}

static string convertStatement(const Token* startToken, const Token* endToken)
{
	// TODO
	return "";
}


// 4 statements type
// eType + eVariable + ";"
// eVariable + eAssignment
// eType + eVariable + 
// (, eExtendedOp
static bool splitStatement(const Scope& sc, vector<Statement>& stmts)
{
    for (const Token* ftok2 = sc.classStart; ftok2 != sc.classEnd; ftok2 = ftok2->next())
    {
    }

    return true;
}

static bool isVariableDefination(const Token& tk)
{
    if (tk.type() != Token::eType)
    {
        return false;
    }

    Token* nextTk = tk.next();
    if (nextTk == nullptr)
    {
        return false;
    }

    if (nextTk->type() != Token::eVariable)
    {
        return false;
    }

    Token* secondTk = nextTk->next();
    if (secondTk == nullptr)
    {
        return false;
    }

    return (secondTk->str() == ";");
}

static bool isAssignmentOP(const Token& tk)
{
    return (tk.str() == "=") && (tk.type() == Token::eAssignmentOp);
}

static bool isVariableInit(const Token& tk)
{
    if (tk.type() != Token::eType)
    {
        return false;
    }

    Token* nextTk = tk.next();
    if (nextTk == nullptr)
    {
        return false;
    }

    if (nextTk->type() != Token::eVariable)
    {
        return false;
    }

    Token* secondTk = nextTk->next();
    if (secondTk == nullptr)
    {
        return false;
    }

    return isAssignmentOP(*secondTk);
}

// Indicator a new line
static bool isNewLineChar(const string& s)
{
    return (s == ";") || (s == "{") || (s == "}");
}

static bool isStartBracket(const Token& tk)
{
    return (tk.str() == "(") && (tk.type() == Token::eExtendedOp);
}

static bool isEndBracket(const Token& tk)
{
    return (tk.str() == ")") && (tk.type() == Token::eExtendedOp);
}

static string convertFunctionContent(const Scope& sc)
{
	string line = "";
    bool isInBracket = false;

	for (const Token* ftok2 = sc.classStart; ftok2 != sc.classEnd; ftok2 = ftok2->next())
	{		
        if (isStartBracket(*ftok2))
        {
			line += ftok2->str() + " ";
            isInBracket = true;
        }
        else if (isEndBracket(*ftok2))
        {
            line += ftok2->str() + " ";
            isInBracket = false;
        }
		else if (isNewLineChar(ftok2->str()) && (!isInBracket))
		{
			line += ftok2->str() + "\r\n";
		}
        else if (isVariableDefination(*ftok2))
        {
			auto varTk = ftok2->next();
			line += "type " + convertClassMember(*varTk) + " " + ftok2->str() + "\r\n";

            ftok2 = ftok2->next()->next();
        }
        else if (isVariableInit(*ftok2))
        {
            // Skip the variable type token
            auto varTk = ftok2->next();
            line += convertClassMember(*varTk) + " := ";

            ftok2 = ftok2->next()->next();
        }
		else if (ftok2->type() == Token::eVariable)
		{
			// member variable
			line += convertClassMember(*ftok2) + " ";
		}
		else
		{
			line += ftok2->str() + " ";
		}
	}
	return line;
}

static string convertNormalFunctionDefine(const Function& func)
{
	vector<string> statements;

	string funcDef = "";
	if (isScopeClass(func.nestedIn) && (!func.isStatic))
	{
		// Only non-static method
		funcDef = ("func (parent * " + func.nestedIn->className + " ) " + func.name() + "(");
	}
	else
	{
		// Global function or static function
		funcDef = ("func " + func.name() + "(");
	}

	for (auto itr = func.argumentList.begin(); itr != func.argumentList.end(); ++itr)
	{
		funcDef += convertVariableDefine(*itr) + ",";
	}

	if (func.argumentList.size() > 0)
	{
		funcDef = funcDef.substr(0, funcDef.size() - 1); // Remove last "," 
	}
	
	string retType = findReturnValue(func);
	if (retType == "void")
	{
		retType = "";
	}
	funcDef += ") " + retType; // function return value

	statements.push_back(funcDef);

	// Function content
	string content = convertFunctionContent(*func.functionScope);
	if (content != "")
	{
		statements.push_back(content);
	}

	statements.push_back("}");

	return array2string(statements);
}

// Go lang has neither constructor nor destructor
static string convertFunctionDefine(const Function& func)
{
	switch (func.type)
	{
	case Function::eConstructor:
		break;
	case Function::eCopyConstructor:
		break;
	case Function::eMoveConstructor:
		break;
	case Function::eOperatorEqual:
		break;
	case Function::eDestructor:
		break;
	case Function::eFunction:
		return convertNormalFunctionDefine(func);
	}
	return "";
}

static string convertGlobalScope(const Scope& sc)
{
	return "";
}

static string convertStructScope(const Scope& sc)
{
	vector<string> statements;

	// Start struct defination
	string classDef = "type " + sc.className + " struct {";
	statements.push_back(classDef);

	// Added variable members
	for (auto var = sc.varlist.begin(); var != sc.varlist.end(); ++var)
	{
		string line = convertVariableDefine(*var);
		statements.push_back(line);
	}

	// End of struct defination
	statements.push_back("}");

	// Added functions definition
	for (auto func = sc.functionList.begin(); func != sc.functionList.end(); ++func)
	{
		string line = convertFunctionDefine(*func);
		statements.push_back(line);
	}

	return array2string(statements);
}

static string convertClassScope(const Scope& sc)
{
	return convertStructScope(sc);
}

static string convertNamespaceScope(const Scope& sc)
{
	return "";
}

static string convertIfScope(const Scope& sc)
{
	return "";
}

static string convertElseScope(const Scope& sc)
{
	return "";
}

static string convertForScope(const Scope& sc)
{
	return "";
}

static string convertWhileScope(const Scope& sc)
{
	return "";
}

static string convertDoScope(const Scope& sc)
{
	return "";
}

static string convertSwitchScope(const Scope& sc)
{
	return "";
}

static string convertTryScope(const Scope& sc)
{
	return "";
}

static string convertCatchScope(const Scope& sc)
{
	return "";
}

static bool isClassMember(const Function* func)
{
	if (func == nullptr)
	{
		return false;
	}

	if (func->nestedIn == nullptr)
	{
		return false;
	}

	return (func->nestedIn->type == Scope::eClass) 
		|| (func->nestedIn->type == Scope::eStruct);
}

GoConvertor::GoConvertor(const Tokenizer* const ptr) : tokenizer_(ptr)
{
}

// See: SymbolDatabase::printOut
std::string GoConvertor::convert()
{
	vector<string> contents;
	const SymbolDatabase* pDb = tokenizer_->getSymbolDatabase();
	for (auto scope = pDb->scopeList.begin(); scope != pDb->scopeList.end(); ++scope)
	{
		string content = convertScope(*scope);

		if (content.size() > 0)
		{
			contents.push_back(content);
		}
	}

    string allContents = array2string(contents);
	cout << allContents << endl;
    return allContents;
}

string GoConvertor::convertScope(const Scope& scope)
{
	switch (scope.type)
	{
	case Scope::eGlobal:
		break;
	case Scope::eClass:
		return convertClassScope(scope);
	case Scope::eStruct:
		return convertStructScope(scope);
	case Scope::eUnion:
		break;
	case Scope::eNamespace:
		return convertNamespaceScope(scope);
	case Scope::eFunction:
		{
			if (isClassMember(scope.function))
			{
				return "";
			}
			return convertNormalFunctionDefine(*scope.function);
		}
	case Scope::eIf:
		return convertIfScope(scope);
	case Scope::eElse:
		return convertElseScope(scope);
	case Scope::eFor:
		return convertForScope(scope);
	case Scope::eWhile:
		return convertWhileScope(scope);
	case Scope::eDo:
		return convertDoScope(scope);
	case Scope::eSwitch:
		return convertSwitchScope(scope);
	case Scope::eUnconditional:
		break;
	case Scope::eTry:
		return convertTryScope(scope);
	case Scope::eCatch:
		break;
	default:
		break;
	}

	return "";
}
