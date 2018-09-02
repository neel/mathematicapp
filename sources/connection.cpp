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

#include "mathematica++/symbol.h"
#include "mathematica++/rules.h"
#include "mathematica++/connection.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/tokens.h"
#include "mathematica++/packet.h"
#include "mathematica++/accessor.h"
#include <boost/lexical_cast.hpp>

// #define M_DEBUG 1

#ifdef M_DEBUG
#include <iostream>
#include <boost/format.hpp>
#endif

mathematica::driver::ws::connection::connection(){
    #ifdef _WIN32
    char* n_argv[] = {(char*)"-linkname", (char*)"math -mathlink"};
    #elif __linux__
    char* n_argv[] = {(char*)"-linkname", (char*)"math -mathlink"};
    #elif __APPLE__
    char* n_argv[] = {(char*)"-linkname", (char*)"/Applications/Mathematica.app/Contents/MacOS/MathKernel -mathlink"};
    #endif
    
    char** argv = n_argv;
    int argc = 2;
    
    int err;
    WSENV env =  WSInitialize((WSParametersPointer)0);
    WSLINK link = WSOpenArgcArgv(env, argc, argv, &err);
    _connected = (err == WSEOK);
    if(_connected){
        err = WSActivate(link);

        _link = link;
        _env  = env;
    }
}

mathematica::driver::ws::connection::connection(int argc, char** argv){
		int err;
		WSENV env =  WSInitialize((WSParametersPointer)0);
		WSLINK link = WSOpenArgcArgv(env, argc, argv, &err);
        _connected = (err == WSEOK);
        if(_connected){
            err = WSActivate(link);

            _link = link;
            _env  = env;
        }
}

mathematica::driver::ws::connection::connection(const std::string& name){
    int err;
    WSENV env =  WSInitialize((WSParametersPointer)0);
    WSLINK link = WSOpenString(env, name.c_str(), &err);
    _connected = (err == WSEOK);
    if(_connected){
        err = WSActivate(link);

        _link = link;
        _env  = env;
    }
}

std::string mathematica::driver::ws::connection::link_name() const{
    const char* name;
    name = WSLinkName(_link);
    if(name){
        std::string name_str(name);
        WSReleaseLinkName(_link, name);
        return name_str;
    }else{
        return std::string();
    }
}

bool mathematica::driver::ws::connection::connected() const{
    return _connected;
}


mathematica::driver::ws::connection::connection(mathematica::driver::ws::connection& other){/*noncopyable*/}

void mathematica::driver::ws::connection::function(const std::string& name, unsigned int nargs){
#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::function " << boost::format("WSPutFunction[%1%, %2%]")% name % nargs << std::endl;
#endif
		if(!WSPutFunction(_link, name.c_str(), nargs)){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::function");
		}
}

void mathematica::driver::ws::connection::integer(int n){
#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::integer " << n << std::endl;
#endif
		if(!WSPutInteger(_link, n)){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::integer");
		}
}

void mathematica::driver::ws::connection::uinteger(unsigned int n){
#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::uinteger " << n << std::endl;
#endif
		long int m = (long int)n;
        long_integer(m);
}


void mathematica::driver::ws::connection::long_integer(long int n){
#ifdef M_DEBUG
    std::cout << "mathematica::driver::ws::connection::integer " << n << std::endl;
#endif
    if(!WSPutLongInteger(_link, n)){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::integer");
    }
}

void mathematica::driver::ws::connection::real(double n){
		#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::real " << n << std::endl;
		#endif
		if(!WSPutReal(_link, n)){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::real");
		}
}

void mathematica::driver::ws::connection::str(const std::string& s){
#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::str \"" << s << "\"" << std::endl;
#endif
		if(!WSPutString(_link, s.c_str())){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::str");
		}
}

void mathematica::driver::ws::connection::symbol(const std::string& s){
#ifdef M_DEBUG
		std::cout << "mathematica::driver::ws::connection::symbol " << s << std::endl;
#endif
		if(!WSPutSymbol(_link, s.c_str())){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::symbol");
		}
}

void mathematica::driver::ws::connection::evaluate(std::string expression){
// #ifdef M_DEBUG
//     std::cout << "mathematica::driver::ws::connection::evaluate " << expression << std::endl;
// #endif
//     char* expr = const_cast<char*>(expression.c_str());
//     if(!WSEvaluate(_link, expr)){
//         throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::evaluate");
//     }
}


void mathematica::driver::ws::connection::end(){
#ifdef M_DEBUG
    std::cout << "mathematica::driver::ws::connection::end" << std::endl;
#endif
	if(!WSEndPacket(_link)){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::end");
	}
}

std::pair<std::string, int> mathematica::driver::ws::connection::get_function(){
	int args;
	const char* symbol = "";
	int success = WSGetFunction(_link, &symbol, &args);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_function");
	}
#ifdef M_DEBUG
    else{
		std::cout << "mathematica::driver::ws::connection::get_function " << std::string(symbol) << " " << args << std::endl;
    }
#endif
	std::string name(symbol);
	WSReleaseSymbol(_link, symbol);
	return std::make_pair(name, args);
}

int mathematica::driver::ws::connection::get_integer(){
	int data;
	int success = WSGetInteger(_link, &data);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
	}
#ifdef M_DEBUG
    else{
		std::cout << "mathematica::driver::ws::connection::get_integer " << data << std::endl;
    }
#endif
	return data;
}

double mathematica::driver::ws::connection::get_real(){
	double data;
	int success = WSGetReal(_link, &data);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_real");
	}
#ifdef M_DEBUG
    else{
		std::cout << "mathematica::driver::ws::connection::get_real " << data << std::endl;
    }
#endif
	return data;
}

std::string mathematica::driver::ws::connection::get_str(){
    const char* message;
    int success = WSGetString(_link, &message);
    if(!success){
        throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_str");
    }
#ifdef M_DEBUG
    else{
		std::cout << "mathematica::driver::ws::connection::get_str " << std::string(message) << std::endl;
    }
#endif
  std::string symbol(message);
  WSReleaseString(_link, message);
  return symbol;
}

std::string mathematica::driver::ws::connection::get_symbol(){
	const char* name;
	int success = WSGetSymbol(_link, &name);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_symbol");
	}
#ifdef M_DEBUG
    else{
		std::cout << "mathematica::driver::ws::connection::get_symbol " << std::string(name) << std::endl;
    }
#endif
	std::string symbol(name);
	WSReleaseSymbol(_link, name);
	return symbol;
}

void mathematica::driver::ws::connection::disconnect(){
	WSClose(_link);
  WSDeinitialize(_env);
}

boost::shared_ptr<mathematica::packet> mathematica::driver::ws::connection::fetch_packet(mathematica::accessor* accessor){
    boost::shared_ptr<mathematica::packet> packet;
    flush();
    int pkt = WSNextPacket(_link);
    push();
    boost::shared_ptr<mathematica::token> token = fetch_token(accessor);
    pop();
    WSNewPacket(_link);
    packet = packets::create(mathematica::packet_type(pkt), token);
    return packet;
}

boost::shared_ptr<mathematica::packet> mathematica::driver::ws::connection::ignore_packet(mathematica::accessor* accessor){
    boost::shared_ptr<mathematica::packet> packet;
    flush();
    int pkt = WSNextPacket(_link);
    boost::shared_ptr<mathematica::token> token;
    WSNewPacket(_link);
    packet = packets::create(mathematica::packet_type(pkt), token);
    return packet;
}


boost::shared_ptr<mathematica::token> mathematica::driver::ws::connection::fetch_token(mathematica::accessor* accessor){
//     int token_type = WSGetType(_link);
    int token_type = WSGetNext(_link);
    boost::shared_ptr<mathematica::token> token = tokens::factory(accessor, token_type);
    if(token){
        token->fetch();
    }
    return token;
}


// bool mathematica::driver::ws::connection::block(int pkt){
//   switch(pkt){
//       case CALLPKT: /* not handled */
//       case BEGINDLGPKT: /* not handled */
//       case ENDDLGPKT: /* not handled */
//       case INPUTPKT: /* not handled */
//       case INPUTSTRPKT: /* not handled */
//       case MENUPKT: /* not handled */
//       case SUSPENDPKT: /* not handled (obsolete) */
//       case RESUMEPKT: /* not handled (obsolete) */
//           break;
//       case DISPLAYPKT:
//           break;
//       case DISPLAYENDPKT:
//           break;
//       case ENTEREXPRPKT:
//           break;
//       case ENTERTEXTPKT:
//           break;
//       case EVALUATEPKT:
//           break;
//       case INPUTNAMEPKT:
//             flush();
//             WSWaitForLinkActivity(_link);
//           break;
//       case OUTPUTNAMEPKT:
//           break;
//       case SYNTAXPKT:
//           break;
//       case TEXTPKT:
//           break;
//       case MESSAGEPKT:
//           break;
//       case RETURNEXPRPKT:
//       case RETURNTEXTPKT:
//       case RETURNPKT:
//           break;
//   }
// }


int mathematica::driver::ws::connection::next(mathematica::accessor* accessor){
//   int token_type = WSGetType(_link);
  int token_type = WSGetNext(_link);
  return token_type;
}

// void mathematica::driver::ws::connection::pull(int packet_type){
// 	int pkt = 0;
// 	while((pkt = WSNextPacket(_link), pkt) && pkt != packet_type){
// 		WSNewPacket(_link);
// 		mathematica::exceptions::error* error = exceptions::dispatch(_link, "mathematica::driver::ws::connection::pull");
// 		if(error) throw error;
// 	}
// }

void mathematica::driver::ws::connection::pull(){
    flush();
    WSWaitForLinkActivity(_link);
}


void mathematica::driver::ws::connection::flush(){
	if(!WSFlush(_link)){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::flush");
	}
}

int mathematica::driver::ws::connection::test(std::string head, int& nargs){
		int n;
		int c = WSTestHead(_link, head.c_str(), &n);
		if(!c){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::test");
		}
		nargs = n;
		return c;
}

int mathematica::driver::ws::connection::head(std::string& type, int& nargs){
		WSMARK mark = WSCreateMark(_link);
		const char* tname;
		int n;
		int c = WSGetFunction(_link, &tname, &n);
		nargs = n;
		if(c){
				type = std::string(tname);
				WSReleaseSymbol(_link, tname);
		}else{
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::head");
		}
		WSSeekToMark(_link, mark, 0);
		WSDestroyMark(_link, mark);
		return c;
}


std::string mathematica::driver::ws::connection::error(int& code){
	int ec = WSError(_link);
	if(ec){
		return std::string(WSErrorMessage(_link));
	}
	return std::string();
}

void mathematica::driver::ws::connection::push(){
    _checkpoints.push(WSCreateMark(_link));
}

void mathematica::driver::ws::connection::pop(){
    WSMARK mark = _checkpoints.top();
    WSSeekToMark(_link, mark, 0);
    WSDestroyMark(_link, mark);
    _checkpoints.pop();
}
