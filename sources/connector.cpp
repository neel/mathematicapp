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

// mathematica::connector::connector(boost::shared_ptr<mathematica::driver::ws::connection> connection): wrapper(connection){
// 	
// }

mathematica::connector::connector(): wrapper(boost::shared_ptr<mathematica::driver::ws::connection>(new mathematica::driver::ws::connection())), _root(new detail::scope(*this)), _current(_root){
    initialize();
}

mathematica::connector::connector(int argc, char** argv): wrapper(boost::shared_ptr<mathematica::driver::ws::connection>(new mathematica::driver::ws::connection(argc, argv))), _root(new detail::scope(*this)), _current(_root){
    initialize();
}

mathematica::connector::connector(const std::string& argv): wrapper(boost::shared_ptr<mathematica::driver::ws::connection>(new mathematica::driver::ws::connection(argv))), _root(new detail::scope(*this)), _current(_root){
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

