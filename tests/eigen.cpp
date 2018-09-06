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
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Eigen3)"
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
#include <Eigen/Dense>
#include <boost/cstdint.hpp>

using namespace mathematica;

MATHEMATICA_DECLARE(Flatten)
MATHEMATICA_DECLARE(Map)
MATHEMATICA_DECLARE(Dimensions)

BOOST_AUTO_TEST_SUITE(eigen)

BOOST_AUTO_TEST_CASE(eigen_matrix){
    typedef std::pair<int, int> matrix_dimension_type;
    
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        Eigen::Matrix<int, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Dimensions(matrix);
        shell >> result;
        
        matrix_dimension_type dims = cast<matrix_dimension_type>(result);
        
        BOOST_CHECK(dims.first == 4 && dims.second == 3);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<int, 1, 3> matrix;
        matrix << 1, 2, 3;
        shell << Dimensions(matrix);
        shell >> result;
        
        matrix_dimension_type dims = cast<matrix_dimension_type>(result);
     
        BOOST_CHECK(dims.first == 1 && dims.second == 3);  
        
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<int, 4, 1> matrix;
        matrix <<  1,
                   4,
                   7,
                  10;
        shell << Dimensions(matrix);
        shell >> result;
        
        matrix_dimension_type dims = cast<matrix_dimension_type>(result);
        
        BOOST_CHECK(dims.first == 4 && dims.second == 1);
//         std::cout << result << std::endl;
    }
    {
        value result;
        typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> dynamic_matrix_type;
//         dynamic_matrix_type matrix;
        int arr[4][3] = {{1 ,2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}}; 
        Eigen::Map<dynamic_matrix_type> map(&arr[0][0], 4, 3);
        dynamic_matrix_type matrix = map;
    
        shell << Dimensions(matrix);
        shell >> result;
        
        matrix_dimension_type dims = cast<matrix_dimension_type>(result);
        
        BOOST_CHECK(dims.first == 4 && dims.second == 3);
//         std::cout << result << std::endl;
    }
}

BOOST_AUTO_TEST_CASE(eigen_matrix_types){
    typedef std::pair<int, int> matrix_dimension_type;
    
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        Eigen::Matrix<boost::uint8_t, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<boost::int16_t, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<boost::uint16_t, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<boost::uint32_t, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<boost::int64_t, 4, 3> matrix;
        matrix <<  1,  2,  3,
                   4,  5,  6,
                   7,  8,  9,
                  10, 11, 12;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<float, 4, 3> matrix;
        matrix <<  1.0f,  2.0f,  3.0f,
                   4.0f,  5.0f,  6.0f,
                   7.0f,  8.0f,  9.0f,
                  10.0f, 11.0f, 12.0f;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78.0f);
//         std::cout << result << std::endl;
    }
    {
        value result;
        Eigen::Matrix<double, 4, 3> matrix;
        matrix <<  1.0f,  2.0f,  3.0f,
                   4.0f,  5.0f,  6.0f,
                   7.0f,  8.0f,  9.0f,
                  10.0f, 11.0f, 12.0f;
        shell << Total(Flatten(matrix));
        shell >> result;
        
        BOOST_CHECK(*result == 78.0f);
//         std::cout << result << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
