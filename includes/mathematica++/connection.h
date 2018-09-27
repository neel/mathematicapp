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

#ifndef MATHEMATICAPP_CONNECTION_H
#define MATHEMATICAPP_CONNECTION_H

#ifdef USING_ML
#include "mathlink.h"
#else
#include "wstp.h"
#endif

#include "compatibility.h"
#include <boost/cstdint.hpp>
#include <stack>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace mathematica{
class token;
struct packet;
class accessor;
namespace driver{
namespace io{

/**
 * no header should include this file, this should only be forward declared inside other classes of mathematica++
 * however this file can be included inside any cpp of mathematica++, but not usable for usercode, usercode should
 * have connector, wrapper etc.. as the interface to connection
 */
struct connection{
  typedef WMK_LINK link_type;
  typedef WMK_ENV  env_type;
  typedef WMK_MARK mark_type;
  
  connection();
  connection(link_type link);
  connection(int argc, char** argv);
  connection(const std::string& name);
  
  static boost::shared_ptr<connection> create(link_type link);
  
  std::string link_name() const;
  bool connected() const;
  
  void function(const std::string& name, unsigned int nargs);
  void integer(int n);
  void uinteger(unsigned int n);
  void long_integer(long n);
  void real(double n);
  void str(const std::string& s);
  void symbol(const std::string& s);
  void evaluate(std::string expression);
  void end();

  void put_array_int8  (const std::vector<boost::uint8_t>& data, const std::vector<int>& dims);
  void put_array_int16 (const std::vector<boost::int16_t>& data, const std::vector<int>& dims);
  void put_array_int32 (const std::vector<boost::int32_t>& data, const std::vector<int>& dims);
  void put_array_int64 (const std::vector<boost::int64_t>& data, const std::vector<int>& dims);
  void put_array_real32(const std::vector<float>& data, const std::vector<int>& dims);
  void put_array_real64(const std::vector<double>& data, const std::vector<int>& dims);
  
  std::pair<std::string, int> get_function();
  std::pair<long long, std::string> get_integer();
  double get_real();
  std::string get_str();
  std::string get_symbol();

  void disconnect();
  
  int next(mathematica::accessor* accessor);
  
  void pull();
  
  void flush();
  
  int head(std::string& type, int& nargs);
  int test(std::string head, int& nargs);

  std::string error(int& code);
  public:
    boost::shared_ptr<mathematica::token> fetch_token(mathematica::accessor* accessor);
  public:
    boost::shared_ptr<mathematica::packet> fetch_packet(mathematica::accessor* accessor);
    boost::shared_ptr<mathematica::packet> ignore_packet(mathematica::accessor* accessor);
  public:
    void push();
    void pop();
  private:
    bool _connected;
    link_type _link;
    env_type  _env;
    std::stack<mark_type> _checkpoints;
		
    connection(connection& other);
};

}

boost::shared_ptr<io::connection> link(io::connection::link_type native_link);

}
}

#endif // MATHEMATICAPP_CONNECTION_H
