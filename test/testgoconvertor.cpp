/*
* cpp2go - A tool for convert C/C++ code to Golang
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


#include "tokenize.h"
#include "goconvertor.h"
#include "testsuite.h"
#include "settings.h"

#include <sstream>

using namespace std;

extern std::ostringstream errout;

class TestGoConvertor : public TestFixture {
public:
    TestGoConvertor() : TestFixture("TestGoConvertor") {
    }

private:
    void run() {

        TEST_CASE(simplePointerDef);
        TEST_CASE(simpleForClause);
        TEST_CASE(simpleClass);
        TEST_CASE(simpleClassDef);
        TEST_CASE(simpleClass2);
        TEST_CASE(simplePointer);

        TEST_CASE(autoVariable);
    }

    string convert(const char code[]) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("performance");
        settings.debug = true;
        settings.debugwarnings = true;
        settings._verbose = true;

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, "test.cpp");

        GoConvertor goconv(&tokenizer);
        std::string res = goconv.convert();
        return res;
    }

    void simpleForClause() {
        string s = convert("int main() { int sum = 0; for (int i = 0; i < 50; i++) { sum+=2;}; return sum;}\n"
            );
        ASSERT(s == 
            "func main() int\r\n"
            "{\r\n"
            "type sum int\r\n"
            "sum = 0 ;\r\n"
            "for ( i := 0 ; i < 50 ; i ++ ) {\r\n"
            "sum = sum + 2 ;\r\n"
            "}\r\n"
            ";\r\n"
            "return sum ;\r\n"
            "\r\n"
            "}\r\n");
    }

    void simpleClassDef() {
        string s = convert(
            "class CA { int abc_; };\n"
            "int main() { CA a; }\n"
            );
        ASSERT(s == 
            "type CA struct {\n"
            "abc_ int\n"
            "}\n"
            "\n"
            "func main() int\n"
            "{\n"
            "type a CA\n"
            "\n"
            "}\n"
            "\n");
    }

    void autoVariable() {
        string s = convert(            
            "int main() { auto i = 1; auto b = 0.1; }\n"
            );
        ASSERT(s!="");
    }

    void simplePointerDef() {
        string s = convert(
            "class CA { int abc_; };\n"
            "int main() { CA* a = new CA(); CA b;}\n"
            );
        ASSERT(s == "type CA struct {\r\n"
            "abc_ int\r\n}\r\n\r\n"
            "func main() int\r\n{\r\n"
            "type a *CA\r\n"
            "a = new CA ( ) ;\r\n"
            "type b CA\r\n\r\n}\r\n");
    }

    void simpleClass() {
        string s = convert(
            "class A {\n"
            " public: A(){}; \n"			
            "void setM(int m){ m_ = m; }\n"
            "static int f(int para){ int v = para + 100; return v; }\n"
            "private: int m_; \n"
            "};\n"
            "int main() { A a; a.setM(20); }\n"
            );
        ASSERT(s == "type A struct {\r\n"
            "m_ int\r\n"
            "}\r\n"
            "\r\n"
            "func (parent * A ) setM(m int) \r\n"
            "{\r\n"
            "parent.m_ = m ;\r\n"
            "\r\n"
            "}\r\n"
            "\r\n"
            "func f(para int) int\r\n"
            "{\r\n"
            "type v int\r\n"
            "v = para + 100 ;\r\n"
            "return v ;\r\n"
            "\r\n"
            "}\r\n"
            "\r\n"
            "\r\n"
            "func main() int\r\n"
            "{\r\n"
            "type a A\r\n"
            "a . setM ( 20 ) ;\r\n"
            "\r\n"
            "}\r\n"
            "\r\n");
    }

    void simpleClass2() {
        string s = convert(
            "class A {\n"
            " public: A(); \n"
            "static int f(int para);\n"
            "void setM(int m);\n"
            "private: int m_; \n"
            "};\n"
            "A::A(){}\n"
            "void A::setM(int m){ m_ = m; }\n"
            "int A::f(int para){int v = para + 100; return v;}\n"
            "int main() { A a; a.setM(20); }\n"
            );
        ASSERT(s!="");
    }

    void simplePointer() {
        string s = convert(
            "class A {\n"
            " public: A(){}; \n"
            "static int f(int* para){ int v = (*para) + 100; return v; }\n"
            "void setM(int m){ m_ = m; }\n"
            "private: int m_; \n"
            "};\n"
            "int main() { A* a = new A(); a->setM(20);delete a;a=0; }\n"
            );
        ASSERT(s!="");
    }
};

REGISTER_TEST(TestGoConvertor)
