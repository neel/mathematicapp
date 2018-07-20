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

#ifndef WSTP_IO_H
#define WSTP_IO_H

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

#endif // WSTP_IO_H
