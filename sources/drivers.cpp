/*
 * Copyright (c) 2018, Sunanda Bose (Neel Basu) (neel.basu.z@gmail.com) 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. 
 */

#include "mathematica++/drivers.h"
#include "mathematica++/exceptions.h"
#include <boost/lexical_cast.hpp>
#include "mathematica++/symbol.h"
#include "mathematica++/rules.h"
#include "mathematica++/connection.h"

void mathematica::driver::io::impl::function(mathematica::driver::io::connection& conn, const std::string& name, unsigned int nargs){
    conn.function(name, nargs);
}

void mathematica::driver::io::impl::integer(mathematica::driver::io::connection& conn, int n){
    conn.integer(n);
}

void mathematica::driver::io::impl::uinteger(mathematica::driver::io::connection& conn, unsigned int n){
    conn.uinteger(n);
}

void mathematica::driver::io::impl::long_integer(mathematica::driver::io::connection& conn, long int n){
    conn.long_integer(n);
}


void mathematica::driver::io::impl::real(mathematica::driver::io::connection& conn, double n){
    conn.real(n);
}

void mathematica::driver::io::impl::str(mathematica::driver::io::connection& conn, const std::string& s){
    conn.str(s);
}

void mathematica::driver::io::impl::symbol(mathematica::driver::io::connection& conn, const std::string& s){
    conn.symbol(s);
}

void mathematica::driver::io::impl::evaluate(mathematica::driver::io::connection& conn, const std::string& expression){
    conn.evaluate(expression);
}


void mathematica::driver::io::impl::end(mathematica::driver::io::connection& conn){
    conn.end();
}

void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, const std::string& s){
    impl::str(conn, s);
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, double n){
    impl::real(conn, n);
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, int n){
    impl::integer(conn, n);
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, unsigned int n){
    impl::uinteger(conn, n);
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, long int n){
    impl::long_integer(conn, n);
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, char s){
    impl::symbol(conn, boost::lexical_cast<std::string>(s));
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, bool s){
    impl::symbol(conn, s ? std::string("True") : std::string("False"));
}
void mathematica::driver::io::impl::put(mathematica::driver::io::connection& conn, const mathematica::symbol& s){
    impl::symbol(conn, s.name());
}

void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::int8_t>  data, std::vector<int> dims){
    std::vector<boost::int16_t> transformed(data.begin(), data.end());
    conn.put_array_int16(transformed, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::uint8_t>  data, std::vector<int> dims){
    conn.put_array_int8(data, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::int16_t>  data, std::vector<int> dims){
    conn.put_array_int16(data, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::uint16_t>  data, std::vector<int> dims){
    std::vector<boost::int32_t> transformed(data.begin(), data.end());
    conn.put_array_int32(transformed, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::int32_t>  data, std::vector<int> dims){
    conn.put_array_int32(data, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::uint32_t>  data, std::vector<int> dims){
    std::vector<boost::int64_t> transformed(data.begin(), data.end());
    conn.put_array_int64(transformed, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<boost::int64_t>  data, std::vector<int> dims){
    conn.put_array_int64(data, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<float>  data, std::vector<int> dims){
    conn.put_array_real32(data, dims);
}
void mathematica::driver::io::impl::put_array(mathematica::driver::io::connection& conn, std::vector<double>  data, std::vector<int> dims){
    conn.put_array_real64(data, dims);
}
