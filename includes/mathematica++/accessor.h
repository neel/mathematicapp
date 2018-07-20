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

#ifndef WSTP_ACCESSOR_H
#define WSTP_ACCESSOR_H

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
namespace ws{
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
    protected:
        bool _transaction_lock_enabled;
        boost::recursive_mutex _mutex;
	public:
		typedef std::pair<std::string, int> head_type;
	protected:
		boost::shared_ptr<mathematica::driver::ws::connection> _connection;
    protected:
		accessor(boost::shared_ptr<mathematica::driver::ws::connection> connection);
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
    mathematica::driver::ws::connection& connection() const;
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
		wrapper(boost::shared_ptr<mathematica::driver::ws::connection> connection);
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
		template <typename T>
		wrapper& operator,(const T arg){
			return operator()(arg);
		}
  public:
    wrapper& put(boost::shared_ptr<mathematica::token> token);
};

}

#endif // WSTP_ACCESSOR_H
