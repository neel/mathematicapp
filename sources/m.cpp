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

#include "mathematica++/m.h"
#include <boost/ref.hpp>
#include <boost/assert.hpp>
#include <sstream>

mathematica::m::m(const std::string& name): _name(name), _length(0){}
mathematica::m::m(const mathematica::m& other): _name(other._name), _queue(other._queue), _length(other._length){}

mathematica::m& mathematica::m::operator()(){
    _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::io::impl::function, _1, _name, _length)));
    return *this;
}

mathematica::m& mathematica::m::invoke(mathematica::driver::io::connection& conn){
//     _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::io::impl::function, boost::ref(conn), "EvaluatePacket", 1)));
    while(!_queue.empty()){
        _queue.front()->invoke(conn);
        _queue.pop_front();
    }
    return *this;
}

mathematica::m mathematica::detail::M_Helper::convert(const mathematica::value val){
    if(val->type() == mathematica::token::token_integer){
        auto token_int = boost::dynamic_pointer_cast<mathematica::tokens::integer>(val);
        if(token_int->storage() == mathematica::tokens::integer::multiprecision){
            std::string valstr = token_int->value_str();
            valstr.erase(0, 1);
            return mathematica::m("Evaluate")(mathematica::m("ToExpression")(valstr));
        }else{
            return mathematica::m("Evaluate")((long long)*token_int);
        }
    }else if(val->type() == mathematica::token::token_real){
        return mathematica::m("Evaluate")((double)*val);
    }else if(val->type() == mathematica::token::token_str){
        return mathematica::m("Evaluate")(val->stringify());
    }else if(val->type() == mathematica::token::token_symbol){
        return mathematica::m("Evaluate")(mathematica::symbol(val->stringify()));
    }else if(val->type() == mathematica::token::token_function){
        boost::shared_ptr<mathematica::tokens::function> token_fnc = boost::dynamic_pointer_cast<mathematica::tokens::function>(val);
        mathematica::m fnc(token_fnc->name());
        for(const mathematica::value& arg: token_fnc->_args){
            mathematica::m argm = convert(arg);
            fnc.arg(argm);
        }
        fnc();
        return fnc;
    }else {
        BOOST_ASSERT_MSG(true, static_cast<std::stringstream&>(std::stringstream() << "Unexpected token received of type " << val->type()).str().c_str());
        return mathematica::m("Evaluate")(mathematica::m("ToExpression")(val->stringify()));
    }
}
