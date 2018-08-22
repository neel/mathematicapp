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

#include "mathematica++/tokens.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/connector.h"
#include "mathematica++/connection.h"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

mathematica::token::token(mathematica::accessor* accessor, int t, token_type type): _token(t), _type(type), _accessor(accessor){

}

int mathematica::token::mathematica_type() const{
	return _token;
}

mathematica::token::token_type mathematica::token::type() const{
	return _type;
}

mathematica::token::operator int() const{
    return boost::lexical_cast<int>(stringify());
}

mathematica::token::operator double() const{
    return boost::lexical_cast<double>(stringify());
}

mathematica::token::operator std::string() const{
    return stringify();
}

std::ostream& mathematica::operator<<(std::ostream& stream, const mathematica::token& tkn){
	stream << tkn.stringify();
	return stream;
}

std::ostream& mathematica::operator<<(std::ostream& stream, const mathematica::value& tkn){
    if(tkn)
        return mathematica::operator<<(stream, *tkn);
    else{
        //FIXME throw exceptions
        return stream;
    }
}

mathematica::tokens::integer::integer(mathematica::accessor* accessor): token(accessor, WSTKINT, token_integer){

}

void mathematica::tokens::integer::fetch(){
	_data = _accessor->connection().get_integer();
}

std::string mathematica::tokens::integer::stringify() const{
	return boost::lexical_cast<std::string>(value());
}

mathematica::variant mathematica::tokens::integer::serialize() const{
    return mathematica::variant(value());
}

int mathematica::tokens::integer::value() const{
	return _data;
}


mathematica::tokens::real::real(mathematica::accessor* accessor): token(accessor, WSTKREAL, token_real){

}

void mathematica::tokens::real::fetch(){
	_data = _accessor->connection().get_real();
}

std::string mathematica::tokens::real::stringify() const{
	return boost::lexical_cast<std::string>(value());
}

mathematica::variant mathematica::tokens::real::serialize() const{
    return mathematica::variant(value());
}


double mathematica::tokens::real::value() const{
	return _data;
}


mathematica::tokens::str::str(mathematica::accessor* accessor): token(accessor, WSTKSTR, token_str){

}

void mathematica::tokens::str::fetch(){
  _message = _accessor->connection().get_str();
}

std::string mathematica::tokens::str::stringify() const{
	return _message;
}

mathematica::variant mathematica::tokens::str::serialize() const{
    return mathematica::variant(stringify());
}

mathematica::tokens::symbol::symbol(mathematica::accessor* accessor): token(accessor, WSTKSYM, token_symbol){

}

void mathematica::tokens::symbol::fetch(){
	_name = _accessor->connection().get_symbol();
}

std::string mathematica::tokens::symbol::name() const{
	return _name;
}

std::string mathematica::tokens::symbol::stringify() const{
	return name();
}

mathematica::variant mathematica::tokens::symbol::serialize() const{
    return mathematica::variant(name());
}


mathematica::tokens::function::function(mathematica::accessor* accessor): token(accessor, WSTKFUNC, token_function){

}

void mathematica::tokens::function::fetch(){
    std::pair<std::string, int> pair = _accessor->connection().get_function();
    _name = pair.first;
    _nargs = pair.second;
    populate();
}

int mathematica::tokens::function::nargs() const{
	return _nargs;
}

std::string mathematica::tokens::function::name() const{
	return _name;
}

bool mathematica::tokens::function::is_graphics() const{
    return _name == "Graphics";
}

void mathematica::tokens::function::populate(){
	for(int i = 0; i < _nargs; ++i){
		boost::shared_ptr<mathematica::token> t = _accessor->fetch();
		_args.push_back(t);
	}
}

std::string mathematica::tokens::function::stringify() const{
	std::string arguments = "";
	for(std::vector<boost::shared_ptr<mathematica::token>>::const_iterator i = _args.begin(); i != _args.end(); ++i){
		if(i != _args.begin())
			arguments += ", ";
		arguments += (*i)->stringify();
	}
	return (boost::format("%1%[%2%]") % name() % arguments).str();
}

mathematica::variant mathematica::tokens::function::serialize() const{
    std::vector<mathematica::variant> children;
    BOOST_FOREACH(mathematica::value arg, _args){
        variant v = arg->serialize();
        children.push_back(v);
    };
    return mathematica::variant(mathematica::composite(name(), children));
}

boost::shared_ptr<mathematica::token> mathematica::tokens::factory(mathematica::accessor* accessor, int type){
	mathematica::token* t = 0x0;
	switch(type){
		case WSTKINT:
			t = new tokens::integer(accessor);
			break;
		case WSTKREAL:
			t = new tokens::real(accessor);
			break;
		case WSTKSTR:
			t = new tokens::str(accessor);
			break;
		case WSTKSYM:
			t = new tokens::symbol(accessor);
			break;
		case WSTKFUNC:
			t = new tokens::function(accessor);
			break;
    default:
      std::cout << "unknown packet recieved" << type << std::endl;
	}
	return boost::shared_ptr<mathematica::token>(t);
}

