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


#ifndef MATHEMATICAPP_ACCESSOR_H
#define MATHEMATICAPP_ACCESSOR_H

#include "mathematica++/tokens.h"
#include "mathematica++/defs.h"
#include "mathematica++/history.h"
#include <string>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/thread.hpp>

namespace mathematica{
namespace driver{
namespace io{
struct connection;
}
}
}

namespace mathematica{
struct m;

/**
 * retrieve mathematica output in tems of tokens. Afte sending some inputs to mathematica with functions in <tt>wrapper</tt> class, a response must be pulled or ignored. To pull the response use <tt>pull</tt> then <tt>next()</tt> and to ignore use <tt></tt>
 * 
 * \see wrapper::end
 */
class accessor: public mathematica::history, private boost::noncopyable{
  static int _features;
  public:
    static void set_features(int f);
    static int  features();
    static void enable(mathematica::features f);
    static bool enabled(mathematica::features f);
    static void disable(mathematica::features f);
  protected:
    bool _transaction_lock_enabled;
    boost::recursive_mutex _mutex;
  public:
    typedef std::pair<std::string, int> head_type;
  protected:
	boost::shared_ptr<mathematica::driver::io::connection> _connection;
  protected:
	accessor(boost::shared_ptr<mathematica::driver::io::connection> connection);
  public:
    /**
     * Returns the return packets content as token
     */
    boost::shared_ptr<mathematica::token> next();
    /**
     * ignores the output. Must ignore explicitely if you don't want to fetch the output, Otherwise it will mess up the next computation
     */
    void ignore();
    boost::shared_ptr<mathematica::token> fetch();
public:
	accessor& flush();
	accessor& pull();
  public:
    mathematica::driver::io::connection& connection() const;
    bool connected() const;
  public:
    virtual ~accessor();
  protected:
    void save_graphics(boost::shared_ptr<mathematica::tokens::function> graphics_token, const std::string& format);
  public:
    void export_graphics(boost::shared_ptr<mathematica::token> token, const std::string& format);
    void export_graphics(const std::string& format);
  public:
    void lock();
    bool try_lock();
    void unlock();
    void enable_transaction_lock(bool flag=true);
    bool transaction_lock_enabled() const;
};

/**
 * send inputs to mathematica
 * mathematica::value result1 = shell->function("N",  1L).function("Sin",  1L).integer(3.14).end().pull().next();
 * mathematica::value result2 = shell->function("Round",  1L).function("N",  1L).function("Exp",  1L).integer(2).end().pull().next();
 * mathematica::value result3 = shell->function("FactorInteger",  1L).integer(210).end().pull().next();
 * mathematica::value result4 = shell->function("Import", 1L).str("/home/neel/projects/senschedule/build/SenSchedule.m").end().pull().next();
 */
class wrapper: public accessor{
	protected:
		wrapper(boost::shared_ptr<mathematica::driver::io::connection> connection);
	public:
		mathematica::value import(const std::string& package);
		void exit();
	public:
		wrapper& function(const std::string& name, unsigned int nargs);
		wrapper& integer(int n);
        wrapper& real(double n);
		wrapper& str(const std::string& s);
        wrapper& symbol(const std::string& s);
        /**
        * Completes process of sending inputs in the currect context. Either mathematica output has to be pulled with <tt>pull().next()</tt> or be ignored with <tt>ignore()</tt>
        */
		accessor& end();
	public:
		wrapper& operator()(const std::string& name, unsigned int nargs);
		wrapper& operator()(int n);
        wrapper& operator()(double n);
		wrapper& operator()(const std::string& s);
        wrapper& operator()(const bool& v);
		template <typename T>
		wrapper& operator,(const T arg){
			return operator()(arg);
		}
  public:
    wrapper& put(boost::shared_ptr<mathematica::token> token);
};

}

#endif // MATHEMATICAPP_ACCESSOR_H
