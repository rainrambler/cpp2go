/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2014 Daniel Marjamäki and Cppcheck team.
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


#include "cppcheck.h"
#include "cppcheckexecutor.h"
#include "testsuite.h"
#include "path.h"
//#include "check.h"

#include <algorithm>
#include <list>
#include <string>

extern std::ostringstream errout;
extern std::ostringstream output;

class TestCppcheck : public TestFixture {
public:
    TestCppcheck() : TestFixture("TestCppcheck") {
    }

private:

    class ErrorLogger2 : public ErrorLogger {
    public:
        std::list<std::string> id;

        void reportOut(const std::string & /*outmsg*/) {
        }

        void reportErr(const ErrorLogger::ErrorMessage &msg) {
            id.push_back(msg._id);
        }
    };

    void run() {
        TEST_CASE(instancesSorted);
        TEST_CASE(classInfoFormat);
        TEST_CASE(getErrorMessages);
    }

    void instancesSorted() const {
    }

    void classInfoFormat() const {
    }

    void getErrorMessages() const {
        ErrorLogger2 errorLogger;
        CppCheck cppCheck(errorLogger, true);
        cppCheck.getErrorMessages();
        ASSERT(!errorLogger.id.empty());

        // TODO: check if there are duplicate error ids in errorLogger.id
        std::string duplicate;
        for (auto it = errorLogger.id.begin();
             it != errorLogger.id.end();
             ++it) {
            if (std::find(errorLogger.id.begin(), it, *it) != it) {
                duplicate = "Duplicate ID: " + *it;
                break;
            }
        }
        ASSERT_EQUALS("", duplicate);
    }
};

REGISTER_TEST(TestCppcheck)
