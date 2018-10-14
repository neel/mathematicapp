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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Chain)"
#include <boost/test/unit_test.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/connector.h"
#include "mathematica++/tokens.h"
#include "mathematica++/variant.h"

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(chain)

BOOST_AUTO_TEST_CASE(scalers){
  connector shell;
  BOOST_CHECK(shell.connected());
  
  {
    value result = shell.function("Plus", 2L).integer(30).integer(50).end().pull().next(); // Plus[30, 50]
    BOOST_CHECK(result->type() == token::token_integer);
    BOOST_CHECK(result->stringify() == "80");
    BOOST_CHECK(boost::lexical_cast<int>(result) == 80);
  }
  
  {
    value result = shell.function("Round",  1L).function("N",  1L).function("Exp",  1L).integer(2).end().pull().next(); // Round[N[Exp[2]]]
    BOOST_CHECK(result->type() == token::token_integer);
    BOOST_CHECK(result->stringify() == "7");
    BOOST_CHECK(boost::lexical_cast<int>(result) == 7);
  }
  
  {
    value result = shell.function("Total", 1L).function("List", 2L).integer(3).integer(5).end().pull().next(); // Total[List[3,5]]
    BOOST_CHECK(result->type() == token::token_integer);
    BOOST_CHECK(result->stringify() == "8");
    BOOST_CHECK(boost::lexical_cast<int>(result) == 8);
  }
  
  {
    value result = shell.function("N",  1L).function("Sin",  1L).integer(3).end().pull().next(); // N[Sin[3]]
    BOOST_CHECK(result->type() == token::token_real);
    int v = boost::lexical_cast<double>(result->stringify())*100000.0f;
    BOOST_CHECK(v == 14112);
  }
}

BOOST_AUTO_TEST_CASE(vectors){
    connector shell;
    BOOST_CHECK(shell.connected());
    
    {
        value result = shell.function("Total", 1L).function("Flatten", 1L).function("List", 4L).integer(30).function("List", 2L).integer(40).integer(50).function("List", 1L).integer(60).integer(70).end().pull().next(); // Total[Flatten[List[30, List[40, 50], List[60], 70]]]
        BOOST_CHECK(result->type() == token::token_integer);
        BOOST_CHECK(result->stringify() == "250");
        BOOST_CHECK(boost::lexical_cast<int>(result) == 250);
    }
    
    {
        value result = shell.function("Flatten", 1L).function("List", 4L).integer(30).function("List", 2L).integer(40).integer(50).function("List", 1L).integer(60).integer(70).end().pull().next(); // Flatten[List[30, List[40, 50], List[60], 70]]
        BOOST_CHECK(result->type() == token::token_function);
        tokens::function::ptr f = boost::dynamic_pointer_cast<tokens::function>(result);
        BOOST_CHECK(f);
        BOOST_CHECK(f->name() == "List");
        BOOST_CHECK(f->nargs() == 5);
        BOOST_CHECK(f->_args.size() == 5);
        for(int i = 1; i < 5; ++i){
            value v = f->_args[i];
            BOOST_CHECK(v->type() == token::token_integer);
            int vi = boost::lexical_cast<int>(v);
            BOOST_CHECK(boost::lexical_cast<int>(f->_args[i-1]) +10 == vi);
        }
    }
    
    {
        value result = shell.function("List", 4L).integer(30).function("List", 2L).integer(40).integer(50).function("List", 1L).integer(60).integer(70).end().pull().next(); // List[30, List[40, 50], List[60], 70]
        BOOST_CHECK(result->type() == token::token_function);
        tokens::function::ptr f = boost::dynamic_pointer_cast<tokens::function>(result);
        BOOST_CHECK(f);
        BOOST_CHECK(f->name() == "List");
        BOOST_CHECK(f->nargs() == 4);
        BOOST_CHECK(f->_args.size() == 4);
        BOOST_CHECK(f->_args[0]->type() == token::token_integer);
        BOOST_CHECK(f->_args[1]->type() == token::token_function);
        BOOST_CHECK(f->_args[2]->type() == token::token_function);
        BOOST_CHECK(f->_args[3]->type() == token::token_integer);
        BOOST_CHECK(result->stringify() == "List[30, List[40, 50], List[60], 70]");
        
        {
            tokens::function::ptr f1 = boost::dynamic_pointer_cast<tokens::function>(f->_args[1]);
            BOOST_CHECK(f1->name() == "List");
            BOOST_CHECK(f1->nargs() == 2);
            BOOST_CHECK(boost::lexical_cast<int>(f1->_args[0]) +10 == boost::lexical_cast<int>(f1->_args[1]));
        }
        
        {
            tokens::function::ptr f2 = boost::dynamic_pointer_cast<tokens::function>(f->_args[2]);
            BOOST_CHECK(f2->name() == "List");
            BOOST_CHECK(f2->nargs() == 1);
            BOOST_CHECK(boost::lexical_cast<int>(f2->_args[0]) == 60);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
