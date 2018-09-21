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
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Association)"
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
#include <boost/date_time.hpp>


struct point{
    std::pair<int, int> location;
    std::string name;
    double elevation;

    point(): location(std::make_pair(0, 0)), name(""), elevation (0.0f){}
    point (std::pair<int, int> loc, const std::string& name_, double elevation_): location(loc), name(name_), elevation(elevation_){}
};

struct circle{
    point center;
    int radius;
    
    circle(): center(point()), radius(0){}
    circle(const point& c, int r): center(c), radius(r){}
};

struct sensor_deployment{
    std::vector<circle> sensors;
    int start;
    int duration;
    long job;
};

// struct sensor_deployment{
//     std::vector<circle> sensors;
//     boost::posix_time::ptime start;
//     boost::posix_time::time_duration duration;
//     long job;
// };

MATHEMATICA_ASSOCIATE(point, std::pair<int, int>, std::string, double){
    MATHEMATICA_PROPERTY(0, location)
    MATHEMATICA_PROPERTY(1, name)
    MATHEMATICA_PROPERTY(2, elevation)
};

MATHEMATICA_ASSOCIATE(circle, point, int){
    MATHEMATICA_PROPERTY(0, center)
    MATHEMATICA_PROPERTY(1, radius)
};

MATHEMATICA_ASSOCIATE(sensor_deployment, std::vector<circle>, int, int, long){
    MATHEMATICA_PROPERTY(0, sensors)
    MATHEMATICA_PROPERTY(1, start)
    MATHEMATICA_PROPERTY(2, duration)
    MATHEMATICA_PROPERTY(3, job)
};

MATHEMATICA_DECLARE(Part)

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(assoc)

BOOST_AUTO_TEST_CASE(association_struct){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        point pt_1(std::make_pair(1, 1), "Hallo", 100.0f);
        point pt_2(std::make_pair(2, 2), "World", 200.0f);
        {
            shell << List(pt_1, pt_2)[1];
            shell >> result;
            point pt = cast<point>(result);
            BOOST_CHECK(pt.location == std::make_pair(1, 1));
            BOOST_CHECK(pt.name == "Hallo");
            BOOST_CHECK(pt.elevation == 100.0f);
        }
        {
            shell << List(pt_1, pt_2)[2];
            shell >> result;
            point pt = cast<point>(result);
            BOOST_CHECK(pt.location == std::make_pair(2, 2));
            BOOST_CHECK(pt.name == "World");
            BOOST_CHECK(pt.elevation == 200.0f);
        }
        {
            typedef std::vector<point> collection_type;
            
            shell << List(pt_1, pt_2);
            shell >> result;
            collection_type points = cast<collection_type>(result);
            BOOST_CHECK(points.size() == 2);
            BOOST_CHECK(points[0].location == std::make_pair(1, 1));
            BOOST_CHECK(points[0].name == "Hallo");
            BOOST_CHECK(points[0].elevation == 100.0f);
            BOOST_CHECK(points[1].location == std::make_pair(2, 2));
            BOOST_CHECK(points[1].name == "World");
            BOOST_CHECK(points[1].elevation == 200.0f);
        }
        {
            typedef std::pair<point, point> collection_type;
            
            shell << List(pt_1, pt_2);
            shell >> result;
            collection_type points = cast<collection_type>(result);
            BOOST_CHECK(points.first.location == std::make_pair(1, 1));
            BOOST_CHECK(points.first.name == "Hallo");
            BOOST_CHECK(points.first.elevation == 100.0f);
            BOOST_CHECK(points.second.location == std::make_pair(2, 2));
            BOOST_CHECK(points.second.name == "World");
            BOOST_CHECK(points.second.elevation == 200.0f);
        }
        {
            typedef boost::tuple<point, point> collection_type;
            
            shell << List(pt_1, pt_2);
            shell >> result;
            collection_type points = cast<collection_type>(result);
            BOOST_CHECK(boost::get<0>(points).location == std::make_pair(1, 1));
            BOOST_CHECK(boost::get<0>(points).name == "Hallo");
            BOOST_CHECK(boost::get<0>(points).elevation == 100.0f);
            BOOST_CHECK(boost::get<1>(points).location == std::make_pair(2, 2));
            BOOST_CHECK(boost::get<1>(points).name == "World");
            BOOST_CHECK(boost::get<1>(points).elevation == 200.0f);
        }
    }
    {
        value result;
        circle disc_1(point(std::make_pair(4, 5), "c1", 300.0f), 4);
        circle disc_2(point(std::make_pair(5, 6), "c2", 300.0f), 5);
        {
            typedef std::vector<circle> collection_type;
            
            shell << List(disc_1, disc_2);
            shell >> result;
            collection_type circles = cast<collection_type>(result);
            BOOST_CHECK(circles.size() == 2);
            BOOST_CHECK(circles[0].center.location == std::make_pair(4, 5));
            BOOST_CHECK(circles[0].center.name == "c1");
            BOOST_CHECK(circles[0].center.elevation == 300.0f);
            BOOST_CHECK(circles[0].radius == 4);
            BOOST_CHECK(circles[1].center.location == std::make_pair(5, 6));
            BOOST_CHECK(circles[1].center.name == "c2");
            BOOST_CHECK(circles[1].center.elevation == 300.0f);
            BOOST_CHECK(circles[1].radius == 5);
        }
        {
            typedef std::pair<circle, circle> collection_type;
            
            shell << List(disc_1, disc_2);
            shell >> result;
            collection_type circles = cast<collection_type>(result);
            BOOST_CHECK(circles.first.center.location == std::make_pair(4, 5));
            BOOST_CHECK(circles.first.center.name == "c1");
            BOOST_CHECK(circles.first.center.elevation == 300.0f);
            BOOST_CHECK(circles.first.radius == 4);
            BOOST_CHECK(circles.second.center.location == std::make_pair(5, 6));
            BOOST_CHECK(circles.second.center.name == "c2");
            BOOST_CHECK(circles.second.center.elevation == 300.0f);
            BOOST_CHECK(circles.second.radius == 5);
        }
        {
            typedef std::vector<circle> collection_type;
            
            collection_type circles;
            circles.push_back(disc_1);
            circles.push_back(disc_2);
            
            shell << Part(circles, 1);
            shell >> result;
            
            circle disc = cast<circle>(result);
            BOOST_CHECK(disc.center.location == std::make_pair(4, 5));
            BOOST_CHECK(disc.center.name == "c1");
            BOOST_CHECK(disc.center.elevation == 300.0f);
            BOOST_CHECK(disc.radius == 4);
        }
    }
    {
        value result;
        circle area_1(point(std::make_pair(4, 5), "c1", 300.0f), 4);
        circle area_2(point(std::make_pair(5, 6), "c2", 300.0f), 5);
        sensor_deployment deployment;
        deployment.sensors.push_back(area_1);
        deployment.sensors.push_back(area_2);
        deployment.start = 0;
        deployment.duration = 0;
//         deployment.start = boost::posix_time::second_clock::local_time();
//         deployment.duration = boost::posix_time::hours(1);
        deployment.job = 1;
        
        shell << List(deployment)[1];
        shell >> result;
        
        std::cout << result << std::endl;
        
        {
            sensor_deployment deplr = cast<sensor_deployment>(result);
            BOOST_CHECK(deplr.sensors.size() == 2);
            BOOST_CHECK(deplr.sensors[0].center.location == std::make_pair(4, 5));
            BOOST_CHECK(deplr.sensors[0].center.name == "c1");
            BOOST_CHECK(deplr.sensors[0].center.elevation == 300.0f);
            BOOST_CHECK(deplr.sensors[0].radius == 4);
            BOOST_CHECK(deplr.sensors[1].center.location == std::make_pair(5, 6));
            BOOST_CHECK(deplr.sensors[1].center.name == "c2");
            BOOST_CHECK(deplr.sensors[1].center.elevation == 300.0f);
            BOOST_CHECK(deplr.sensors[1].radius == 5);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
