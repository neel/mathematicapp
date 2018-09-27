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

#ifndef MATHEMATICAPP_CONNECTOR_H
#define MATHEMATICAPP_CONNECTOR_H

#include <vector>
#include <string>
#include <stack>
#include <complex>
#include <boost/variant.hpp>
#include "tokens.h"
#include "accessor.h"
#include <boost/shared_ptr.hpp>

namespace mathematica{
namespace driver{
namespace ws{
struct connection;
}
}
}

namespace mathematica{
    
struct m;
namespace detail{
    struct scope;
}

/**
 * mathematica::connector shell(argc, argv);
 * mathematica::value result1 = shell.function("N",  1L).function("Sin",  1L).integer(3.14).end().pull().next();
 * mathematica::value result2 = shell.function("Round",  1L).function("N",  1L).function("Exp",  1L).integer(2).end().pull().next();
 * mathematica::value result3 = shell.function("FactorInteger",  1L).integer(210).end().pull().next();
 * mathematica::value result4 = shell.function("Import", 1L).str("/home/neel/projects/senschedule/build/SenSchedule.m").end().pull().next();
 */
class connector: public mathematica::wrapper{
//   connector(boost::shared_ptr<mathematica::driver::ws::connection> connection);
  std::string _last;
  boost::shared_ptr<detail::scope> _root;
  boost::shared_ptr<detail::scope> _current;
  public:
    struct storage{
        friend wrapper& operator<<(mathematica::connector::storage storage, const mathematica::m& expr);
//        friend wrapper& operator<<(mathematica::connector::storage storage, const mathematica::value& val);
        
        connector& _conn;
        std::string _name;
        
        storage(connector& conn, const std::string& name);
        connector& conn();
        private:
          void set_key(const std::string& key);
    };
    struct local_scope{
        connector& _conn;
        
        local_scope(connector& conn);
        local_scope(const local_scope& other);
        /**
         * end()'s current scope
         */
        ~local_scope();
    };
  public:
    connector();
    connector(boost::shared_ptr<mathematica::driver::ws::connection> connection);
    connector(int argc, char** argv);
    connector(const std::string& argv);
    std::string name() const;
  public:
	typedef boost::shared_ptr<mathematica::connector> ptr;
	~connector();
  public:
    void initialize();
  public:
    /**
     * moving results of a computation will lead to hiding of the actual mathematica returned object. Rather it will return a reference to a saved object. which can be referred or loaded latter.
     * shell.save() << SomeMathematicaFunctionCall("that", "returns", "large", "output");
     */
    storage save(const std::string& name=""){
        return storage(*this, name);
    }
    /**
     * Returns a reference to the stored object. This reference can be pussed to the next mathematica input
     */
    mathematica::m ref(const std::string& name) const;
    /**
     * fetches a saved object from storage symbol table, which is in mathematica
     */
    mathematica::value load(const std::string& name="");
    std::string last_key() const;
    mathematica::m last() const;
  public:
    std::string begin(const std::string& name="");
    std::string end(bool delete_all = true);
    /**
     * creates a scoped object that end()'s the scope inits descructor
     */
    local_scope scoped(const std::string& name="");
  public:
    void unset(const std::string& name);
};

}

#endif // MATHEMATICAPP_CONNECTOR_H
