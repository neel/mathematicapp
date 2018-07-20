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

#include "mathematica++/exceptions.h"
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/connection.h"

mathematica::exceptions::error::error(int ec, const std::string& context, const std::string& message): runtime_error((boost::format("WSTP Error (%1%) in <%2%> \"%3%\"") % ec % context % message).str()), _code(ec), _context(context), _message(message){

}

mathematica::exceptions::error mathematica::exceptions::dispatch(WSLINK link, const std::string& context){
	int ec = WSError(link);
	return error(ec, context, std::string(WSErrorMessage(link)));
}

mathematica::exceptions::error mathematica::exceptions::dispatch(mathematica::driver::ws::connection& conn, const std::string& context){
    int ec = 0;
    std::string message = conn.error(ec);
    return error(ec, context, message);
}
