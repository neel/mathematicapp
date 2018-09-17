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
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::STL)"
#include <boost/test/unit_test.hpp>
#include <string>
#include <cmath>
#include <complex>
#include <map>
#include <vector>
#include <iostream>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "boost/tuple/tuple.hpp"
#include "mathematica++/connector.h"
#include "mathematica++/m.h"
#include "mathematica++/io.h"
#include "mathematica++/declares.h"
#include "mathematica++/operators.h"
#include "mathematica++/rules.h"

#include "mathematica++/association.h"
using namespace mathematica;

// MATHEMATICA_DECLARE(FactorInteger)
MATHEMATICA_DECLARE(Flatten)

BOOST_AUTO_TEST_SUITE(stl)

BOOST_AUTO_TEST_CASE(stl_vector){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        std::vector<int> list = {1,2,3,4,5};
        shell << Total(list);
        shell >> result;
        // std::cout << result << std::endl; // Prints 15
        BOOST_CHECK(*result == 15);
    }
    {
        value result;
        typedef std::vector<std::vector<int>> ivv_type;
        shell << FactorInteger(2434500);
        shell >> result;
        ivv_type prime_powers = mathematica::cast<ivv_type>(result);
        int sbase = 0 , spow = 0;
        for(auto pp: prime_powers){
            // std::cout << pp[0] << " ^ " << pp[1] << std::endl;
            sbase += pp[0];
            spow  += pp[1];
        }
        BOOST_CHECK(sbase == 551);
        BOOST_CHECK(spow == 8);
    }
    {
        value result;
        typedef std::vector<std::pair<int, int>>    ipv_type;
        typedef std::vector<boost::tuple<int, int>> itv_type;
        shell << FactorInteger(2434500);
        shell >> result;
        
        int sbase = 0 , spow = 0;
        
        ipv_type prime_powers_pair = mathematica::cast<ipv_type>(result);
        for(auto pp: prime_powers_pair){
            // std::cout << pp.first << " ^ " << pp.second << std::endl;
            sbase += pp.first;
            spow  += pp.second;
        }
        BOOST_CHECK(sbase == 551);
        BOOST_CHECK(spow == 8);
        
        sbase = 0, spow = 0;
        itv_type prime_powers_tuple = mathematica::cast<itv_type>(result);
        for(auto pp: prime_powers_tuple){
            // std::cout << pp << std::endl;
            sbase += boost::tuples::get<0>(pp);
            spow  += boost::tuples::get<1>(pp);
        }
        BOOST_CHECK(sbase == 551);
        BOOST_CHECK(spow == 8);
    }
    {
        typedef std::vector<std::vector<std::vector<int>>> ivv_type; // 2 x 2 x 3
        ivv_type matrix;
        std::vector<int> row11 = {111, 112, 113};
        std::vector<int> row12 = {121, 122, 123};
        
        std::vector<int> row21 = {211, 212, 213};
        std::vector<int> row22 = {221, 222, 223};
        
        std::vector<std::vector<int>> row1;
        std::vector<std::vector<int>> row2;
        
        row1.push_back(row11);
        row1.push_back(row12);
        
        row2.push_back(row21);
        row2.push_back(row22);
        
        matrix.push_back(row1);
        matrix.push_back(row2);
        
        value res;
        
        shell.enable(mathematica::bulk_io);
        shell << Total(Flatten(matrix));
        shell >> res;
        
        BOOST_CHECK(*res == 2004);
    }
    {
        typedef std::vector<std::vector<std::vector<int>>> ivv_type; // 2 x 2 x 3
        ivv_type matrix;
        std::vector<int> row11 = {111, 112, 113};
        std::vector<int> row12 = {121, 122, 123};
        
        std::vector<int> row21 = {211, 212, 213};
        std::vector<int> row22 = {221, 222};
        
        std::vector<std::vector<int>> row1;
        std::vector<std::vector<int>> row2;
        
        row1.push_back(row11);
        row1.push_back(row12);
        
        row2.push_back(row21);
        row2.push_back(row22);
        
        matrix.push_back(row1);
        matrix.push_back(row2);
        
        value res;
        
        shell.enable(mathematica::bulk_io);
        shell << Total(Flatten(matrix));
        shell >> res;
        
        BOOST_CHECK(*res == 1781);
    }
    point_meta pm;
    std::cout << pm.build() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
