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

#ifndef MATHEMATICAPP_DRIVERS_H
#define MATHEMATICAPP_DRIVERS_H

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

#endif // MATHEMATICAPP_DRIVERS_H
