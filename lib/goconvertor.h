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

#ifndef GOCONVERTOR_H_1dzfes
#define GOCONVERTOR_H_1dzfes

#include "config.h"

#include <string>
#include <vector>

class Tokenizer;
class Scope;

class CPPCHECKLIB GoConvertor
{
public:
	GoConvertor(const Tokenizer* const ptr);

	std::string convert();

private:
	std::string convertScope(const Scope& scope);

private:
	const Tokenizer* tokenizer_;
};

#endif // GOCONVERTOR_H_1dzfes
