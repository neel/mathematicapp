#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Connection)"
#include <boost/test/unit_test.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <boost/format.hpp>
#include "mathematica++/connector.h"

BOOST_AUTO_TEST_SUITE(connection)

BOOST_AUTO_TEST_CASE(cstring_argv){
  int argc = 4;
  char* argv[] = {(char*)"-linkname", (char*)"math -mathlink", (char*)0x0};
  mathematica::connector connector(argc, argv);
  BOOST_CHECK(connector.connected());
}

BOOST_AUTO_TEST_CASE(str){
  std::string argv = "-linkname math -mathlink";
  mathematica::connector connector(argv);
  BOOST_CHECK(connector.connected());
}

BOOST_AUTO_TEST_SUITE_END()
