/*
 * Copyright 2017 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#ifndef WSTP_EXCEPTIONS_H
#define WSTP_EXCEPTIONS_H

#include "wstp.h"
#include <string>
#include <stdexcept>

namespace mathematica{

namespace driver{
namespace ws{
struct connection;
}
}

struct message{
    
};

namespace exceptions{
struct error: public std::runtime_error{
	int 		_code;
	std::string _context;
	std::string _message;
	
	error(int ec, const std::string& context, const std::string& message);
	int code() const{return _code;}
	const std::string& context() const{return _context;}
};

error dispatch(WSLINK link, const std::string& context);

error dispatch(mathematica::driver::ws::connection& conn, const std::string& context);
}

}

#endif // WSTP_EXCEPTIONS_H
