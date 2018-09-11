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

#include "mathematica++/compatibility.h"
#include "mathematica++/symbol.h"
#include "mathematica++/rules.h"
#include "mathematica++/connection.h"
#include "mathematica++/exceptions.h"
#include "mathematica++/tokens.h"
#include "mathematica++/packet.h"
#include "mathematica++/accessor.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

#ifdef USING_DEBUG_TRACE
#include <iterator>
#include <iostream>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
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
    WMK_ENV env =  WMK_Initialize((WMK_ParametersPointer)0);
    WMK_LINK link = WMK_OpenArgcArgv(env, argc, argv, &err);
    _connected = (err == WMK_EOK);
    if(_connected){
        err = WMK_Activate(link);

        _link = link;
        _env  = env;
    }
}

mathematica::driver::ws::connection::connection(int argc, char** argv){
		int err;
		WMK_ENV env =  WMK_Initialize((WMK_ParametersPointer)0);
		WMK_LINK link = WMK_OpenArgcArgv(env, argc, argv, &err);
        _connected = (err == WMK_EOK);
        if(_connected){
            err = WMK_Activate(link);

            _link = link;
            _env  = env;
        }
}

mathematica::driver::ws::connection::connection(const std::string& name){
    int err;
    WMK_ENV env =  WMK_Initialize((WMK_ParametersPointer)0);
    WMK_LINK link = WMK_OpenString(env, name.c_str(), &err);
    _connected = (err == WMK_EOK);
    if(_connected){
        err = WMK_Activate(link);

        _link = link;
        _env  = env;
    }
}

std::string mathematica::driver::ws::connection::link_name() const{
    const char* name;
    name = WMK_LinkName(_link);
    if(name){
        std::string name_str(name);
        WMK_ReleaseLinkName(_link, name);
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
#ifdef USING_DEBUG_TRACE
		std::clog << boost::format("[%1%]: %2%(\"%3%\", %4%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutFunction) % name % nargs << std::endl;
#endif
		if(!WMK_PutFunction(_link, name.c_str(), nargs)){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::function");
		}
}

void mathematica::driver::ws::connection::integer(int n){
#ifdef USING_DEBUG_TRACE
    std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutInteger) % n << std::endl;
#endif
    if(!WMK_PutInteger(_link, n)){
            throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::integer");
    }
}

void mathematica::driver::ws::connection::uinteger(unsigned int n){
    long int m = (long int)n;
    long_integer(m);
}


void mathematica::driver::ws::connection::long_integer(long int n){
#ifdef USING_DEBUG_TRACE
    std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutLongInteger) % n << std::endl;
#endif
    if(!WMK_PutLongInteger(_link, n)){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::integer");
    }
}

void mathematica::driver::ws::connection::real(double n){
#ifdef USING_DEBUG_TRACE
    std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutReal) % n << std::endl;
#endif
    if(!WMK_PutReal(_link, n)){
            throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::real");
    }
}

void mathematica::driver::ws::connection::str(const std::string& s){
    if(boost::starts_with(s, "#")){
        std::string nstr = s;
        nstr.erase(0, 1);
#ifdef USING_DEBUG_TRACE
        std::clog << boost::format("[%1%]: %2%(\"%3%\")") % boost::posix_time::second_clock::local_time() % STR(WMK_PutRealNumberAsString) % nstr << std::endl;
#endif
        if(!WMK_PutRealNumberAsString(_link, nstr.c_str())){
            throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::str");
        }
    }else{
#ifdef USING_DEBUG_TRACE
        std::clog << boost::format("[%1%]: %2%(\"%3%\")") % boost::posix_time::second_clock::local_time() % STR(WMK_PutString) % s << std::endl;
#endif
        if(!WMK_PutString(_link, s.c_str())){
            throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::str");
        }
    }
}

void mathematica::driver::ws::connection::symbol(const std::string& s){
#ifdef USING_DEBUG_TRACE
    std::clog << boost::format("[%1%]: %2%(\"%3%\")") % boost::posix_time::second_clock::local_time() % STR(WMK_PutSymbol) % s << std::endl;
#endif
    if(!WMK_PutSymbol(_link, s.c_str())){
            throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::symbol");
    }
}

void mathematica::driver::ws::connection::evaluate(std::string expression){
// #ifdef USING_DEBUG_TRACE
//     std::cout << "mathematica::driver::ws::connection::evaluate " << expression << std::endl;
// #endif
//     char* expr = const_cast<char*>(expression.c_str());
//     if(!WSEvaluate(_link, expr)){
//         throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::evaluate");
//     }
}

void mathematica::driver::ws::connection::put_array_int8(const std::vector<boost::uint8_t>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<boost::uint8_t>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutInteger8Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const boost::uint8_t* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutInteger8Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_int8");
    }
}

void mathematica::driver::ws::connection::put_array_int16(const std::vector<boost::int16_t>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<boost::int16_t>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutInteger16Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const boost::int16_t* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutInteger16Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_int16");
    }
}

void mathematica::driver::ws::connection::put_array_int32(const std::vector<boost::int32_t>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<boost::int32_t>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutInteger32Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const boost::int32_t* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutInteger32Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_int32");
    }
}

void mathematica::driver::ws::connection::put_array_int64(const std::vector<boost::int64_t>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<boost::int64_t>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutInteger64Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const boost::int64_t* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutInteger64Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_int64");
    }
}

void mathematica::driver::ws::connection::put_array_real32(const std::vector<float>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<float>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutReal32Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const float* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutReal32Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_real32");
    }
}

void mathematica::driver::ws::connection::put_array_real64(const std::vector<double>&  data, const std::vector<int>& dims){
#ifdef USING_DEBUG_TRACE
    std::ostringstream data_stream;
    std::copy(data.begin(), data.end(), std::ostream_iterator<double>(data_stream, ","));
    std::string data_str = data_stream.str();
    data_str.pop_back();
    
    std::ostringstream depth_stream;
    std::copy(dims.begin(), dims.end(), std::ostream_iterator<int>(depth_stream, ","));
    std::string depth_str = depth_stream.str();
    depth_str.pop_back();
    
    std::clog << boost::format("[%1%]: %2%({%3%}, {%4%}, 0, %5%)") % boost::posix_time::second_clock::local_time() % STR(WMK_PutReal64Array) % data_str % depth_str % dims.size() << std::endl;
#endif
    const double* elems = data.data();
    const int* dimsa = dims.data();
    if(!WMK_PutReal64Array(_link, elems, dimsa, reinterpret_cast<const char **>(0), dims.size())){
        throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::put_array_real64");
    }
}

void mathematica::driver::ws::connection::end(){
#ifdef USING_DEBUG_TRACE
    std::clog << boost::format("[%1%]: %2%()") % boost::posix_time::second_clock::local_time() % STR(WMK_EndPacket) << std::endl;
#endif
	if(!WMK_EndPacket(_link)){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::end");
	}
}

std::pair<std::string, int> mathematica::driver::ws::connection::get_function(){
	int args;
	const char* symbol = "";
	int success = WMK_GetFunction(_link, &symbol, &args);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_function");
	}
#ifdef USING_DEBUG_TRACE
    else{
		std::clog << boost::format("[%1%]: %2%(\"%3%\", %4%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetFunction) % symbol % args << std::endl;
    }
#endif
	std::string name(symbol);
	WMK_ReleaseSymbol(_link, symbol);
	return std::make_pair(name, args);
}

std::pair<long long, std::string> mathematica::driver::ws::connection::get_integer(){
    long int result_int = 0;
    std::string result_str;
    
    int raw_type = WMK_GetRawType(_link);
    if(raw_type == WMK_TK_SHORT){
        short data;
        int success = WMK_GetInteger16(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }
#ifdef USING_DEBUG_TRACE
        else{
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetInteger16) % data << std::endl;
        }
#endif
        result_int = data;
    }else if(raw_type == WMK_TK_INT){
        int data;
        int success = WMK_GetInteger32(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }
#ifdef USING_DEBUG_TRACE
        else{
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetInteger32) % data << std::endl;
        }
#endif
        result_int = data;
    }else if(raw_type == WMK_TK_LONG){
        long data;
        int success = WMK_GetInteger64(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }
#ifdef USING_DEBUG_TRACE
        else{
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetInteger64) % data << std::endl;
        }
#endif
        result_int = data;
    }else if(raw_type == WMK_TK_INT64){
        long int data;
        int success = WMK_GetInteger64(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }
#ifdef USING_DEBUG_TRACE
        else{
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetInteger64) % data << std::endl;
        }
#endif
        result_int = data;
    }else if(raw_type == WMK_TK_SIZE_T){
        long int data;
        int success = WMK_GetInteger64(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }
#ifdef USING_DEBUG_TRACE
        else{
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetInteger64) % data << std::endl;
        }
#endif
        result_int = data;
    }else{
        const char* data;
        int success = WMK_GetNumberAsString(_link, &data);
        if(!success){
            throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_integer");
        }else{
#ifdef USING_DEBUG_TRACE
            std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetNumberAsString) % data << std::endl;
#endif            
            std::string number(data);
            number.insert(0, "#");
            WMK_ReleaseString(_link, data);
            result_str = number;
        }
    }
	return std::make_pair(result_int, result_str);
}

double mathematica::driver::ws::connection::get_real(){
	double data;
	int success = WMK_GetReal(_link, &data);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_real");
	}
#ifdef USING_DEBUG_TRACE
    else{
		std::clog << boost::format("[%1%]: %2%(%3%)") % boost::posix_time::second_clock::local_time() % STR(WMK_GetReal) % data << std::endl;
    }
#endif
	return data;
}

std::string mathematica::driver::ws::connection::get_str(){
    const char* message;
    int success = WMK_GetString(_link, &message);
    if(!success){
        throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_str");
    }
#ifdef USING_DEBUG_TRACE
    else{
		std::clog << boost::format("[%1%]: %2%(\"%3%\")") % boost::posix_time::second_clock::local_time() % STR(WMK_GetString) % message << std::endl;
    }
#endif
  std::string symbol(message);
  WMK_ReleaseString(_link, message);
  return symbol;
}

std::string mathematica::driver::ws::connection::get_symbol(){
	const char* name;
	int success = WMK_GetSymbol(_link, &name);
	if(!success){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::get_symbol");
	}
#ifdef USING_DEBUG_TRACE
    else{
		std::clog << boost::format("[%1%]: %2%(\"%3%\")") % boost::posix_time::second_clock::local_time() % STR(WMK_GetSymbol) % name << std::endl;
    }
#endif
	std::string symbol(name);
	WMK_ReleaseSymbol(_link, name);
	return symbol;
}

void mathematica::driver::ws::connection::disconnect(){
    WMK_Close(_link);
    WMK_Deinitialize(_env);
}

boost::shared_ptr<mathematica::packet> mathematica::driver::ws::connection::fetch_packet(mathematica::accessor* accessor){
    boost::shared_ptr<mathematica::packet> packet;
    flush();
    int pkt = WMK_NextPacket(_link);
    push();
    boost::shared_ptr<mathematica::token> token = fetch_token(accessor);
    pop();
    WMK_NewPacket(_link);
    packet = packets::create(mathematica::packet_type(pkt), token);
    return packet;
}

boost::shared_ptr<mathematica::packet> mathematica::driver::ws::connection::ignore_packet(mathematica::accessor* accessor){
    boost::shared_ptr<mathematica::packet> packet;
    flush();
    int pkt = WMK_NextPacket(_link);
    boost::shared_ptr<mathematica::token> token;
    WMK_NewPacket(_link);
    packet = packets::create(mathematica::packet_type(pkt), token);
    return packet;
}


boost::shared_ptr<mathematica::token> mathematica::driver::ws::connection::fetch_token(mathematica::accessor* accessor){
//     int token_type = WSGetType(_link);
    int token_type = WMK_GetNext(_link);
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
  int token_type = WMK_GetNext(_link);
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
    WMK_WaitForLinkActivity(_link);
}


void mathematica::driver::ws::connection::flush(){
	if(!WMK_Flush(_link)){
		throw exceptions::dispatch(_link, "mathematica::driver::ws::connection::flush");
	}
}

int mathematica::driver::ws::connection::test(std::string head, int& nargs){
		int n;
		int c = WMK_TestHead(_link, head.c_str(), &n);
		if(!c){
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::test");
		}
		nargs = n;
		return c;
}

int mathematica::driver::ws::connection::head(std::string& type, int& nargs){
		WMK_MARK mark = WMK_CreateMark(_link);
		const char* tname;
		int n;
		int c = WMK_GetFunction(_link, &tname, &n);
		nargs = n;
		if(c){
				type = std::string(tname);
				WMK_ReleaseSymbol(_link, tname);
		}else{
				throw exceptions::dispatch(*this, "mathematica::driver::ws::connection::head");
		}
		WMK_SeekToMark(_link, mark, 0);
		WMK_DestroyMark(_link, mark);
		return c;
}


std::string mathematica::driver::ws::connection::error(int& code){
	int ec = WMK_Error(_link);
	if(ec){
		return std::string(WMK_ErrorMessage(_link));
	}
	return std::string();
}

void mathematica::driver::ws::connection::push(){
    _checkpoints.push(WMK_CreateMark(_link));
}

void mathematica::driver::ws::connection::pop(){
    WMK_MARK mark = _checkpoints.top();
    WMK_SeekToMark(_link, mark, 0);
    WMK_DestroyMark(_link, mark);
    _checkpoints.pop();
}
