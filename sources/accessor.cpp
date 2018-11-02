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

#include "mathematica++/accessor.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/connection.h"
#include "mathematica++/tokens.h"
#include "mathematica++/packet.h"
#include "mathematica++/drivers.h"
#include "mathematica++/m.h"
#include <map>
#include <ctime>
#include <boost/format.hpp>
#include <iostream>

mathematica::wrapper::wrapper(boost::shared_ptr<mathematica::driver::io::connection> connection): accessor(connection){
	
}

mathematica::wrapper& mathematica::wrapper::function(const std::string& name, unsigned int nargs){
	driver::io::impl::function(*_connection, name, nargs);
	return *this;
}

mathematica::wrapper& mathematica::wrapper::integer(int n){
	driver::io::impl::integer(*_connection, n);
	return *this;
}

mathematica::wrapper& mathematica::wrapper::real(double n){
	driver::io::impl::real(*_connection, n);
	return *this;
}

mathematica::wrapper& mathematica::wrapper::str(const std::string& s){
	driver::io::impl::str(*_connection, s);
	return *this;
}

mathematica::wrapper& mathematica::wrapper::symbol(const std::string& s){
	driver::io::impl::symbol(*_connection, s);
	return *this;
}

mathematica::accessor& mathematica::wrapper::end(){
	driver::io::impl::end(*_connection);
	return *this;
}

mathematica::value mathematica::wrapper::import(const std::string& package){
    return function("Import", 1L).str(package).end().pull().next();
}

void mathematica::wrapper::exit(){
    function("Exit", 0);
}

mathematica::wrapper& mathematica::wrapper::operator()(const std::string& name, unsigned int nargs){
	return function(name, nargs);
}

mathematica::wrapper& mathematica::wrapper::operator()(int n){
	return integer(n);
}

mathematica::wrapper& mathematica::wrapper::operator()(double n){
	return real(n);
}

mathematica::wrapper& mathematica::wrapper::operator()(const std::string& s){
	return str(s);
}

mathematica::wrapper& mathematica::wrapper::operator()(const bool& v){
	driver::io::impl::symbol(*_connection, v ? std::string("True") : std::string("False"));
    return *this;
}

mathematica::wrapper& mathematica::wrapper::put(mathematica::token::ptr token){
    switch(token->type()){
        case mathematica::token::token_integer:
            return integer(boost::dynamic_pointer_cast<mathematica::tokens::integer>(token)->value());
        case mathematica::token::token_real:
            return real(boost::dynamic_pointer_cast<mathematica::tokens::real>(token)->value());
        case mathematica::token::token_str:
            return str(boost::dynamic_pointer_cast<mathematica::tokens::str>(token)->stringify());
        case mathematica::token::token_symbol:
            return symbol(boost::dynamic_pointer_cast<mathematica::tokens::symbol>(token)->name());
        case mathematica::token::token_function:{
            boost::shared_ptr<mathematica::tokens::function> f =boost::dynamic_pointer_cast<mathematica::tokens::function>(token);
            function(f->name(), f->nargs());
            for(std::vector<mathematica::value>::const_iterator i = f->_args.begin(); i != f->_args.end(); ++i){
                put(*i);
            }
        }
        case mathematica::token::token_null:
            return symbol("Null");
            //FIXME check this
    }
    return *this;
}

mathematica::accessor::accessor(boost::shared_ptr<mathematica::driver::io::connection> connection): _connection(connection), _transaction_lock_enabled(false){
	
}

mathematica::accessor::~accessor(){
    
}

void mathematica::accessor::set_features(int f){
    _features = f;
}

int mathematica::accessor::features(){
    return _features;
}

void mathematica::accessor::enable(mathematica::features f){
    _features |= f;
}

bool mathematica::accessor::enabled(mathematica::features f){
    return (_features & f);
}

void mathematica::accessor::disable(mathematica::features f){
    _features &= ~f;
}

boost::shared_ptr<mathematica::token> mathematica::accessor::next(){
    std::vector<mathematica::packet::ptr> packets;
    
    mathematica::packet::ptr packet;
    do{
        packet = _connection->fetch_packet(this);
        
        if(!packets.empty()){
            mathematica::packet::ptr last = packets.back();
            if(last->type() == mathematica::packet_MESSAGE && packet->type() == mathematica::packet_TEXT){
                mathematica::packets::Message::ptr message_packet = boost::dynamic_pointer_cast<mathematica::packets::Message>(last);
                mathematica::packets::Text::ptr text_packet = boost::dynamic_pointer_cast<mathematica::packets::Text>(packet);
                
                message_packet->set_text(text_packet);
            }else{
                packets.push_back(packet);
            }
        }else{
            packets.push_back(packet);
        }
    }while(packet && packet->type() != mathematica::packet_RETURN);
    
    if(!packet){
        std::cout << "ERROR: received null packet from mathematica" << std::endl;
        return mathematica::token::ptr();
    }
    
    mathematica::packets::Return::ptr return_packet = boost::dynamic_pointer_cast<mathematica::packets::Return>(packet);
    std::for_each(packets.begin(), packets.end(), boost::bind(&mathematica::packets::Return::relate, return_packet, _1));
    packets.clear();
    
    record(return_packet);
    mathematica::token::ptr token = return_packet->_token;

    if(token->type() == mathematica::token::token_function){
        boost::shared_ptr<mathematica::tokens::function> f =boost::dynamic_pointer_cast<mathematica::tokens::function>(token);
        if(f->is_graphics()){
            save_graphics(f, "png");
        }
    }
    
    return token;
}

void mathematica::accessor::ignore(){
    mathematica::packet::ptr packet;
    do{
        packet = _connection->ignore_packet(this);
    }while(packet->type() != mathematica::packet_RETURN);
    
    if(transaction_lock_enabled()){
        unlock();
    }
}


void mathematica::accessor::save_graphics(boost::shared_ptr<mathematica::tokens::function> graphics_token, const std::string& format){
    wrapper* w = dynamic_cast<wrapper*>(this);
    std::string graphics_name = (boost::format("graphics_%1%_%2%%3%.%4%") % count() % time(0x0) % (rand() % 1000) % format).str();
    mathematica::value v = w->function("Export", 2).str(graphics_name).put(graphics_token).end().pull().next();
    std::cout << ">> graphics saved on " << v << std::endl;
}

void mathematica::accessor::export_graphics(boost::shared_ptr<mathematica::token> token, const std::string& format){
    if(token->type() == mathematica::token::token_function){
        boost::shared_ptr<mathematica::tokens::function> f =boost::dynamic_pointer_cast<mathematica::tokens::function>(token);
        if(f->is_graphics()){
            save_graphics(f, "png");
        }else{
            std::for_each(f->_args.begin(), f->_args.end(), boost::bind(&mathematica::accessor::export_graphics, this, _1, format));
        }
    }
}

void mathematica::accessor::export_graphics(const std::string& format){
    export_graphics(last()->token(), format);
}

boost::shared_ptr< mathematica::token > mathematica::accessor::fetch(){
 int token_type = _connection->next(this);
 boost::shared_ptr<mathematica::token> token = tokens::factory(this, token_type);
 token->fetch();
 return token;
}

mathematica::accessor& mathematica::accessor::flush(){
	_connection->flush();
	return *this;
}

mathematica::accessor& mathematica::accessor::pull(){
    _connection->pull();
	return *this;
}

mathematica::driver::io::connection& mathematica::accessor::connection() const{
    return *_connection;
}

bool mathematica::accessor::connected() const{
    return _connection->connected();
}

void mathematica::accessor::lock(){
    _mutex.lock();
}

bool mathematica::accessor::try_lock(){
    return _mutex.try_lock();
}

void mathematica::accessor::unlock(){
    _mutex.unlock();
}

void mathematica::accessor::enable_transaction_lock(bool flag){
    _transaction_lock_enabled = flag;
}
bool mathematica::accessor::transaction_lock_enabled() const{
    return _transaction_lock_enabled;
}

int mathematica::accessor::_features = 0;
