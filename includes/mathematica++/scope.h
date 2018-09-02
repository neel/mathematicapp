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

#ifndef MATHEMATICAPP_SCOPE_H
#define MATHEMATICAPP_SCOPE_H

#include <list>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace mathematica{
class connector;
    
namespace detail{
    
struct scope: public boost::enable_shared_from_this<scope>{
  friend class mathematica::connector;
  public:
    typedef boost::shared_ptr<scope> scope_ptr;
    typedef std::vector<std::string> cache_collection_type;
    typedef std::list<scope_ptr> scope_collection_type;
    typedef scope_collection_type::iterator iterator;
    typedef scope_collection_type::const_iterator const_iterator;
  private:
    connector&             _connector;
    scope*                 _parent;
    std::string            _name;
    cache_collection_type  _cache;
    scope_collection_type  _children;
    
    scope(connector& conn, scope* parent=0x0, const std::string& key="");
  public:
    ~scope();
  public:
    std::string name() const;
    bool is_root() const;
    const_iterator begin() const;
    const_iterator end() const;
    size_t locals_count() const;
    boost::shared_ptr<scope> parent();
    boost::shared_ptr<const scope> parent() const;
    
    /**
     * ceates a child scope under its parent
     */
    scope_ptr create(const std::string& key="");
    /**
     * adds a key in the current scope (saved in mathematica runtime cache)
     */
    void add(const std::string& key);
    /**
     * unset this scope and all its children recursively and unsets them from mathematica Runtime
     */
    void unset(const std::string& name="");
    /**
     * delete's all child scopes recursively and also unsets them from Mathematica Runtime but keeps the locals
     * \return the number of elements removed (including children of children recursively)
     */
    void unset_child_scopes();
    void remove_child_scope(scope_ptr child);
    /**
     * finds the scopeby scope name or by a saved key name in the tree (recursively)
     */
    boost::shared_ptr<const scope> find(const std::string& key) const;
    boost::shared_ptr<scope> find(const std::string& key);
    /**
     * checks whether a key (scope name or saved variable name) exists in the tree or not
     */
    bool exists(const std::string& key) const;
};

}
}

#endif // MATHEMATICAPP_SCOPE_H
