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
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Shell)"
#include <boost/test/unit_test.hpp>
#include <string>
#include <cmath>
#include <complex>
#include <iostream>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "mathematica++/connector.h"
#include "mathematica++/m.h"
#include "mathematica++/io.h"
#include "mathematica++/declares.h"
#include "mathematica++/operators.h"
#include "mathematica++/rules.h"

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(shell)

BOOST_AUTO_TEST_CASE(token_io){
  int argc = 4;
  char* argv[] = {(char*)"-linkname", (char*)"math -mathlink", (char*)0x0};
  mathematica::connector shell(argc, argv);
  BOOST_CHECK(shell.connected());
  
  {
    value v;
    shell << m("Plus")(3.45, 4, m("Plus")(3.45, 4));
    shell >> v;  
    BOOST_CHECK(v == 14.9f);
  }
  
  {
    value v;
    shell << m("Total")(m("List")(3, 4, 3, 4));
    shell >> v;  
    BOOST_CHECK(v == 14);
  }
}

BOOST_AUTO_TEST_CASE(token_conversion){
  int argc = 4;
  char* argv[] = {(char*)"-linkname", (char*)"math -mathlink", (char*)0x0};
  mathematica::connector shell(argc, argv);
  BOOST_CHECK(shell.connected());
  
  {
    value v;
    
    shell << List(5, 2.5, std::complex<int>(1, 2));
    shell >> v;
    
    typedef boost::tuples::tuple<int, double, std::complex<int>> type;
    type v1 = tuplify<type>(v);
    type v2 = cast<type>(v);
    
    BOOST_CHECK(v1 == v2);
    BOOST_CHECK(boost::get<0>(v1) == 5);
    BOOST_CHECK(boost::get<1>(v1) == 2.5f);
    BOOST_CHECK(boost::get<2>(v1) == std::complex<int>(1, 2));
  }
  
  {
    value v;
    
    shell << List(5, 2.5, std::complex<int>(1, 2));
    shell >> v;
    
    typedef boost::tuples::tuple<int, double, std::complex<double>> type;
    type v1 = tuplify<type>(v);
    type v2 = cast<type>(v);
    
    BOOST_CHECK(v1 == v2);
    BOOST_CHECK(boost::get<0>(v1) == 5);
    BOOST_CHECK(boost::get<1>(v1) == 2.5f);
    BOOST_CHECK(boost::get<2>(v1) == std::complex<double>(1.0f, 2.0f));
  }
  
  {
    value v;
    
    shell << List(1, 2, List(3, 5), std::complex<double>(3.2f, 4.7f));
    shell >> v;
    
    typedef boost::tuples::tuple<int, int, std::vector<int>, std::complex<double>> type;
    type v1 = cast<type>(v);
    
    BOOST_CHECK(boost::get<0>(v1) == 1);
    BOOST_CHECK(boost::get<1>(v1) == 2);
    BOOST_CHECK(boost::get<2>(v1).size() == 2);
    BOOST_CHECK(boost::get<2>(v1)[0] == 3 && boost::get<2>(v1)[1] == 5);
    BOOST_CHECK(boost::get<3>(v1) == std::complex<double>(3.2f, 4.7f));
  }
  
  {
    value v;
    
	shell << List(
					List(50, 20.5, std::complex<int>(10, 20)), 
					List(60, 30.5, std::complex<int>(20, 30))
            );
    shell >> v;
    
    typedef boost::tuples::tuple<int, double, std::complex<int>> record_type;
    typedef std::vector<record_type> type;
    
    type v1 = cast<type>(v);
    
    BOOST_CHECK(v1.size() == 2);
    BOOST_CHECK(boost::get<0>(v1[0]) +10 == boost::get<0>(v1[1]));
    BOOST_CHECK(boost::get<1>(v1[0]) +10 == boost::get<1>(v1[1]));
    BOOST_CHECK(boost::get<2>(v1[0]) +std::complex<int>(10, 10) == boost::get<2>(v1[1]));
  }
  
  {
    symbol x("x");
    symbol y("y");
    
    {
        auto opt = (Rule("WorkingPrecision")= 2, Rule("VerifySolutions")= symbol::TrueS());
        value soln;
        shell << Solve(Power(x, 2) == 2, x, opt);
        shell >> soln;
        
        typedef std::vector<std::vector<rule<double>>> type;
        // BOOST_CHECK(soln == std::string("List[List[Rule[x, -1.4142135623730951]], List[Rule[x, 1.4142135623730951]]]"));
        type sol = cast<type>(soln);        
        BOOST_CHECK(sol[0][0].value() + sol[1][0].value() == 0);
    }
    
    {
        auto opt = (Rule("WorkingPrecision")= 2, Rule("VerifySolutions")= symbol::TrueS());
        value soln;
        shell << Solve(Power(x, 2) == 2, x, opt);
        shell >> soln;

        typedef std::vector<std::vector<std::pair<std::string, double>>> type;
        // BOOST_CHECK(soln == std::string("List[List[Rule[x, -1.4142135623730951]], List[Rule[x, 1.4142135623730951]]]"));
        type sol = cast<type>(soln);
        BOOST_CHECK(sol[0][0].first == "x");
        BOOST_CHECK(sol[1][0].first == "x");
        BOOST_CHECK(sol[0][0].second + sol[1][0].second == 0);
    }
    
    {
        value soln;
        shell << Solve(2*x+y==7 && 3*x-y==1, List(x, y), (Rule("WorkingPrecision")= 2, Rule("VerifySolutions")= true));
        shell >> soln;
        
        typedef std::vector<std::vector<rule<double>>> type;
        type sol = cast<type>(soln);
        BOOST_CHECK(sol[0][0].key() == "x");
        BOOST_CHECK(sol[0][1].key() == "y");
        double a = round((sol[0][1].value() - sol[0][0].value())*10.0f);
        BOOST_CHECK(a == 22.0f);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
