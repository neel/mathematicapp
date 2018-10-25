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

#include "mathematica++/io.h"
#include "mathematica++/m.h"
#include "mathematica++/exceptions.h"

template <>
mathematica::wrapper& mathematica::operator<<<mathematica::symbol>(mathematica::wrapper& stream, const mathematica::symbol& symbol){
    if(stream.transaction_lock_enabled()){
        stream.lock();
    }
    stream.symbol(symbol.name());
    stream.end();
    return stream;
}

template <>
mathematica::wrapper& mathematica::operator<<<mathematica::m>(mathematica::wrapper& stream, const mathematica::m& expr){
    if(stream.transaction_lock_enabled()){
        stream.lock();
    }
    const_cast<mathematica::m&>(expr).invoke(stream.connection());
    stream.end();
    return stream;
}

template <>
mathematica::wrapper& mathematica::operator<<<mathematica::value>(mathematica::wrapper& stream, const mathematica::value& val){
    mathematica::m argm = detail::M_Helper::convert(val);
    stream << argm;
    return stream;
}

mathematica::wrapper& mathematica::operator,(mathematica::wrapper& stream, const mathematica::m& expr){
    if(stream.transaction_lock_enabled()){
        stream.lock();
    }
    const_cast<mathematica::m&>(expr).invoke(stream.connection());
    stream.end();
    stream.ignore();
    return stream;
}

mathematica::wrapper& mathematica::operator<<(mathematica::connector::storage storage, const mathematica::m& expr){
    mathematica::m expression("StoreSymbol");
    if(storage._name.empty()){
        expression(expr);
    }else{
        expression(expr, storage._name);
    }
    storage.conn() << expression;
    mathematica::value key;
    storage.conn() >> key;
    storage.set_key(key->stringify());
    return storage.conn();
}

mathematica::wrapper& mathematica::operator<<(mathematica::connector::storage storage, const mathematica::value& val){
    mathematica::m expression("StoreSymbol");
    if(storage._name.empty()){
        expression(val);
    }else{
        expression(val, storage._name);
    }
    storage.conn() << expression;
    mathematica::value key;
    storage.conn() >> key;
    storage.set_key(key->stringify());
    return storage.conn();
}

// mathematica::wrapper & mathematica::operator<<(mathematica::wrapper& shell, mathematica::basic_message& msg){
//     msg.pass(shell);
//     return shell;
// }

