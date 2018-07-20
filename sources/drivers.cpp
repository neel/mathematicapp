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

// #define M_DEBUG 1

#include "mathematica++/drivers.h"
#include "mathematica++/exceptions.h"
#include <boost/lexical_cast.hpp>
#include "mathematica++/symbol.h"
#include "mathematica++/rules.h"
#include "mathematica++/connection.h"

void mathematica::driver::ws::impl::function(mathematica::driver::ws::connection& conn, const std::string& name, unsigned int nargs){
    conn.function(name, nargs);
}

void mathematica::driver::ws::impl::integer(mathematica::driver::ws::connection& conn, int n){
    conn.integer(n);
}

void mathematica::driver::ws::impl::uinteger(mathematica::driver::ws::connection& conn, unsigned int n){
    conn.uinteger(n);
}

void mathematica::driver::ws::impl::long_integer(mathematica::driver::ws::connection& conn, long int n){
    conn.long_integer(n);
}


void mathematica::driver::ws::impl::real(mathematica::driver::ws::connection& conn, double n){
    conn.real(n);
}

void mathematica::driver::ws::impl::str(mathematica::driver::ws::connection& conn, const std::string& s){
    conn.str(s);
}

void mathematica::driver::ws::impl::symbol(mathematica::driver::ws::connection& conn, const std::string& s){
    conn.symbol(s);
}

void mathematica::driver::ws::impl::evaluate(mathematica::driver::ws::connection& conn, const std::string& expression){
    conn.evaluate(expression);
}


void mathematica::driver::ws::impl::end(mathematica::driver::ws::connection& conn){
    conn.end();
}

void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, const std::string& s){
    impl::str(conn, s);
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, double n){
    impl::real(conn, n);
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, int n){
    impl::integer(conn, n);
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, unsigned int n){
    impl::uinteger(conn, n);
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, long int n){
    impl::long_integer(conn, n);
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, char s){
    impl::symbol(conn, boost::lexical_cast<std::string>(s));
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, bool s){
    impl::symbol(conn, s ? std::string("True") : std::string("False"));
}
void mathematica::driver::ws::impl::put(mathematica::driver::ws::connection& conn, const mathematica::symbol& s){
    impl::symbol(conn, s.name());
}

