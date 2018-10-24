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

#include "mathematica++/compatibility.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/declares.h"
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/connection.h"
#include "mathematica++/io.h"
#ifdef USING_LL
#include "mathematica++/transport.h"
#endif

const char* mathematica::exceptions::error::what() const noexcept {
    std::string err_msg = (boost::format("<%1%> error: %2% with return code (%3%)") % _context % _message % _code).str();
    return err_msg.c_str();
}

mathematica::basic_message::basic_message(const mathematica::basic_message& other): _tag(other._tag), _args(other._args){}

mathematica::basic_message::basic_message(const std::string& tag): _tag(tag), _args("Sequence"){}

mathematica::basic_message::~basic_message(){}

mathematica::message::message(const mathematica::message& other): basic_message(other){}

mathematica::message::message(const std::string& tag): basic_message(tag){}

mathematica::message::~message(){}

void mathematica::basic_message::pass(mathematica::wrapper& shell, std::string library_name){
    _args();
    value val;
    shell << Message(MessageName(symbol(library_name), _tag), _args);
//     shell.ignore();
    shell >> val;
}
#ifdef USING_LL
void mathematica::basic_message::pass(mathematica::transport& shell, std::string library_name){
    _args();
    value val;
    shell << Message(MessageName(symbol(library_name), _tag), _args);
//     shell.ignore();
    shell >> val;
}
void mathematica::library::exceptions::internal_error::pass(mathematica::transport& shell, std::string library_name){
    value val;
    shell << mathematica::m("Echo")(what(), library_name+"Exception");
    shell >> val;
}

void mathematica::library::exceptions::library_error::pass(mathematica::transport& shell, std::string library_name){
    basic_message::pass(shell, library_name);
    if(!_errmsg.empty()){
        internal_error::pass(shell, library_name);
    }
}
#endif

mathematica::exceptions::error::error(int ec, const std::string& context, const std::string& message): runtime_error((boost::format("WSTP Error (%1%) in <%2%> \"%3%\"") % ec % context % message).str()), _code(ec), _context(context), _message(message){

}

mathematica::exceptions::error mathematica::exceptions::dispatch(WMK_LINK link, const std::string& context){
	int ec = WMK_Error(link);
	return error(ec, context, std::string(WMK_ErrorMessage(link)));
}

mathematica::exceptions::error mathematica::exceptions::dispatch(mathematica::driver::io::connection& conn, const std::string& context){
    int ec = 0;
    std::string message = conn.error(ec);
    return error(ec, context, message);
}
