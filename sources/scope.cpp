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
#include "mathematica++/scope.h"
#include "mathematica++/connector.h"
#include <ctime>
#include <boost/format.hpp>

mathematica::detail::scope::scope(mathematica::connector& conn, mathematica::detail::scope* parent, const std::string& key): _connector(conn), _parent(parent), _name(key){

}

mathematica::detail::scope::~scope(){

}

std::string mathematica::detail::scope::name() const{
    return _name;
}

bool mathematica::detail::scope::is_root() const{
    return !_parent;
}

mathematica::detail::scope::const_iterator mathematica::detail::scope::begin() const{
    return _children.begin();
}

mathematica::detail::scope::const_iterator mathematica::detail::scope::end() const{
    return _children.end();
}

size_t mathematica::detail::scope::locals_count() const{
    return _cache.size();
}

boost::shared_ptr<mathematica::detail::scope> mathematica::detail::scope::parent(){
    return shared_from_this();
}

boost::shared_ptr<const mathematica::detail::scope> mathematica::detail::scope::parent() const{
    return shared_from_this();
}

mathematica::detail::scope::scope_ptr mathematica::detail::scope::create(const std::string& key){
    std::string name = key;
    if(name.empty()){
        std::time_t now = std::time(nullptr);
        std::srand(std::time(0));
        name = (boost::format("%1%.%2%") % now % std::rand()).str();
    }
    mathematica::detail::scope::scope_ptr child(new scope(_connector, this, name));
    return child;
}

void mathematica::detail::scope::add(const std::string& key){
    _cache.push_back(key);
}

boost::shared_ptr<const mathematica::detail::scope> mathematica::detail::scope::find(const std::string& key) const{
    if(_name == key){
        return shared_from_this();
    }
    cache_collection_type::const_iterator it = std::find(_cache.begin(), _cache.end(), key);
    if(it != _cache.end()){
        return shared_from_this();
    }
    for(const_iterator i = begin(); i != end(); ++i){
        boost::shared_ptr<const mathematica::detail::scope> child = (*i)->find(key);
        if(child.get()){
            return child;
        }
    }
    return boost::shared_ptr<const mathematica::detail::scope>();
}

boost::shared_ptr<mathematica::detail::scope> mathematica::detail::scope::find(const std::string& key){
    if(_name == key){
        return shared_from_this();
    }
    cache_collection_type::const_iterator it = std::find(_cache.begin(), _cache.end(), key);
    if(it != _cache.end()){
        return shared_from_this();
    }
    for(const_iterator i = begin(); i != end(); ++i){
        boost::shared_ptr<mathematica::detail::scope> child = (*i)->find(key);
        if(child.get()){
            return child;
        }
    }
    return boost::shared_ptr<mathematica::detail::scope>();
}

bool mathematica::detail::scope::exists(const std::string& key) const{
    return !!find(key);
}

void mathematica::detail::scope::remove_child_scope(mathematica::detail::scope::scope_ptr child){
    _children.remove(child);
}

void mathematica::detail::scope::unset(const std::string& name){
    if(!name.empty()){
        cache_collection_type::iterator it = std::find(_cache.begin(), _cache.end(), name);
        if(it != _cache.end()){
            _connector.unset(name);
            _cache.erase(it);
        }
        return;
    }
    unset_child_scopes();
    for(cache_collection_type::const_iterator i = _cache.begin(); i != _cache.end(); ++i){
        _connector.unset(*i);
    }
    _cache.clear();
    for(const_iterator i = begin(); i != end(); ++i){
        scope_ptr child = *i;
        child->unset();
    }
    scope_ptr p = parent();
    p->remove_child_scope(shared_from_this());
}

void mathematica::detail::scope::unset_child_scopes(){
    for(const_iterator i = begin(); i != end(); ++i){
        scope_ptr child = *i;
        child->unset();
    }
    _children.clear();
}
