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

#include "mathematica++/connector.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/connection.h"
#include "mathematica++/m.h"
#include "mathematica++/io.h"
#include "mathematica++/config.h"
#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/scope.h"

mathematica::connector::connector(boost::shared_ptr<mathematica::driver::io::connection> connection): wrapper(connection), _root(new detail::scope(*this)), _current(_root){

}

mathematica::connector::connector(): wrapper(boost::shared_ptr<mathematica::driver::io::connection>(new mathematica::driver::io::connection())), _root(new detail::scope(*this)), _current(_root){
    initialize();
}

mathematica::connector::connector(int argc, char** argv): wrapper(boost::shared_ptr<mathematica::driver::io::connection>(new mathematica::driver::io::connection(argc, argv))), _root(new detail::scope(*this)), _current(_root){
    initialize();
}

mathematica::connector::connector(const std::string& argv): wrapper(boost::shared_ptr<mathematica::driver::io::connection>(new mathematica::driver::io::connection(argv))), _root(new detail::scope(*this)), _current(_root){
    initialize();
}

void mathematica::connector::initialize(){
    import(M_STORAGE_LIB_PATH);
}

std::string mathematica::connector::name() const{
    return _connection->link_name();
}

mathematica::connector::~connector(){
	_connection->disconnect();
}

mathematica::connector::storage::storage(mathematica::connector& conn, const std::string& name): _conn(conn), _name(name){

}

mathematica::connector& mathematica::connector::storage::conn(){
  return _conn;
}

void mathematica::connector::storage::set_key(const std::string& key){
    _conn._last = key;
    _conn._current->add(key);
}

mathematica::value mathematica::connector::load(const std::string& name){
    *this << ref(name);
    mathematica::value ret;
    *this >> ret;
    return ret;
}

mathematica::m mathematica::connector::ref(const std::string& name) const{
    mathematica::m refered("LoadSymbol");
    refered(name);
    return refered;
}

std::string mathematica::connector::last_key() const{
    return _last;
}

mathematica::m mathematica::connector::last() const{
    return ref(last_key());
}

std::string mathematica::connector::begin(const std::string& name){
    if(transaction_lock_enabled()){
        lock();
    }
    boost::shared_ptr<detail::scope> current = _current->create(name);
    _current = current;
    return _current->name();
}

std::string mathematica::connector::end(bool delete_all){
    boost::shared_ptr<detail::scope> current = _current;
    _current = current->parent();
    if(delete_all){
        current->unset();
    }
    if(transaction_lock_enabled()){
        unlock();
    }
    return current->name();
}

mathematica::connector::local_scope mathematica::connector::scoped(const std::string& name){
    begin();
    local_scope local(*this);
    return local;
}

mathematica::connector::local_scope::local_scope(mathematica::connector &conn): _conn(conn){

}

mathematica::connector::local_scope::local_scope(const mathematica::connector::local_scope &other): _conn(other._conn){

}

mathematica::connector::local_scope::~local_scope(){
    _conn.end();
}

void mathematica::connector::unset(const std::string& name){
    mathematica::m rem("RemoveSymbol");
    rem(name);
    *this << rem;
    ignore();
}

