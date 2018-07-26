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

#include "mathematica++/io.h"
#include "mathematica++/m.h"

mathematica::wrapper& mathematica::operator<<(mathematica::wrapper& stream, const mathematica::symbol& symbol){
    if(stream.transaction_lock_enabled()){
        stream.lock();
    }
    stream.symbol(symbol.name());
    stream.end();
    return stream;
}

mathematica::wrapper& mathematica::operator<<(mathematica::wrapper& stream, const mathematica::m& expr){
    if(stream.transaction_lock_enabled()){
        stream.lock();
    }
    const_cast<mathematica::m&>(expr).invoke(stream.connection());
    stream.end();
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

//mathematica::wrapper& mathematica::operator<<(mathematica::connector::storage storage, const mathematica::value& val){
//    mathematica::m expression("StoreSymbol");
//    expression(val);
//    storage.conn() << expression;
//    mathematica::value key;
//    storage.conn() >> key;
//    storage.set_key(key->stringify());
//    return storage.conn();
//}

