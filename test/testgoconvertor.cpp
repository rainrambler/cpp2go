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
        //TEST_CASE(simpleForClause);
		//TEST_CASE(simpleClass);
		//TEST_CASE(simpleClass2);
		TEST_CASE(simplePointer);
    }

    string convert(const char code[]) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("performance");

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
		ASSERT(s!="");
    }

	void simpleClass() {
		string s = convert(
			"class A {\n"
			" public: A(){}; \n"
			"static int f(int para){ int v = para + 100; return v; }\n"
			"void setM(int m){ m_ = m; }\n"
			"private: int m_; \n"
			"};\n"
			"int main() { A a; a.setM(20); }\n"
			  );
		ASSERT(s!="");
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
