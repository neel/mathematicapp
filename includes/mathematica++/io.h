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

#ifndef MATHEMATICAPP_IO_H
#define MATHEMATICAPP_IO_H

#include "accessor.h"
#include "tokens.h"
#include "variant.h"
#include "connector.h"

namespace mathematica{
struct m;
struct symbol;

namespace detail{
template <typename T>
struct stream_helper{
    static void write(mathematica::accessor& stream, const T& expr){}
};

template <>
struct stream_helper<mathematica::value>{
    static void write(mathematica::accessor& stream, mathematica::value& value){
        value = stream.pull().next();
    }
};
}

wrapper& operator<<(wrapper& stream, const mathematica::symbol& symbol);
wrapper& operator<<(wrapper& stream, const mathematica::m& expr);
wrapper& operator,(wrapper& stream, const mathematica::m& expr);
template <typename T>
wrapper& operator>>(wrapper& stream, T& expr){
    detail::stream_helper<T>::write(stream, expr);
    if(stream.transaction_lock_enabled()){
        stream.unlock();
    }
    return stream;
}

wrapper& operator<<(mathematica::connector::storage storage, const mathematica::m& expr);
//wrapper& operator<<(mathematica::connector::storage storage, const mathematica::value& val);
}

#endif // MATHEMATICAPP_IO_H
