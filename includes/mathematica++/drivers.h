/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
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
 * 
 */

#ifndef DRIVERS_H
#define DRIVERS_H

#include <string>

namespace mathematica{
namespace driver{
namespace ws{
struct connection;
}
}
}

namespace mathematica{
struct symbol;
namespace driver{
namespace ws{

struct impl{
	static void function(mathematica::driver::ws::connection& conn, const std::string& name, unsigned int nargs);
	static void integer(mathematica::driver::ws::connection& conn, int n);
    static void uinteger(mathematica::driver::ws::connection& conn, unsigned int n);
  static void long_integer(mathematica::driver::ws::connection& conn, long n);
	static void real(mathematica::driver::ws::connection& conn, double n);
	static void str(mathematica::driver::ws::connection& conn, const std::string& s);
	static void symbol(mathematica::driver::ws::connection& conn, const std::string& s);
	static void end(mathematica::driver::ws::connection& conn);
  static void evaluate(mathematica::driver::ws::connection& conn, const std::string& expression);
  
  static void put(mathematica::driver::ws::connection& conn, int n);
  static void put(mathematica::driver::ws::connection& conn, unsigned int n);
  static void put(mathematica::driver::ws::connection& conn, long n);
  static void put(mathematica::driver::ws::connection& conn, double n);
  static void put(mathematica::driver::ws::connection& conn, const std::string& s);
  static void put(mathematica::driver::ws::connection& conn, char s);
  static void put(mathematica::driver::ws::connection& conn, bool s);
  static void put(mathematica::driver::ws::connection& conn, const mathematica::symbol& s);
};

namespace ml{
}
}
}
}

#endif // DRIVERS_H
