CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/" "${CMAKE_CURRENT_SOURCE_DIR}/modules/")
PROJECT(mathematica++)

MESSAGE(STATUS "MODULE PATH" ${CMAKE_MODULE_PATH})

MACRO(USE_CXX11)
  IF(CMAKE_VERSION VERSION_LESS "3.1")
    IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      SET(CMAKE_CXX_FLAGS "--std=gnu++14 ${CMAKE_CXX_FLAGS}")
    ENDIF()
  ELSE()
    SET(CMAKE_CXX_STANDARD 14)
  ENDIF()
ENDMACRO(USE_CXX11)

USE_CXX11()

# https://stackoverflow.com/a/19578320/256007
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

SET(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0 ${CMAKE_CXX_FLAGS}")

SET(Mathematica_USE_STATIC_LIBRARIES ON)

FIND_PACKAGE(Boost COMPONENTS filesystem REQUIRED)
FIND_PACKAGE(Mathematica 11.0.0 REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Mathematica_INCLUDE_DIR})

SET(MATHEMATICAPP_INCLUDE_DIRS
	${CMAKE_CURRENT_SOURCE_DIR}/includes
)

INCLUDE_DIRECTORIES(${MATHEMATICAPP_INCLUDE_DIRS})

OPTION(WITH_ML "Use MathLink functions insteadof using WSTP")
OPTION(PRING_TRACE "Print low level the WS/ML functions that are being used")

IF(WITH_ML)
    MESSAGE("${BoldMagenta} Using MathLink instead of WSTP ${ColourReset}")
    ADD_COMPILE_DEFINITIONS(USING_ML)
    INCLUDE_DIRECTORIES(${Mathematica_MathLink_INCLUDE_DIR})
ELSE()
    ADD_COMPILE_DEFINITIONS(USING_WS)
    INCLUDE_DIRECTORIES(${Mathematica_WSTP_INCLUDE_DIR})
ENDIF()

IF(PRING_TRACE)
    ADD_COMPILE_DEFINITIONS(USING_DEBUG_TRACE)
ENDIF()

SET(MATHEMATICAPP_HEADERS
    includes/mathematica++/scope.h
    includes/mathematica++/connector.h
    includes/mathematica++/exceptions.h
    includes/mathematica++/tokens.h
    includes/mathematica++/accessor.h
    includes/mathematica++/variant.h
    includes/mathematica++/drivers.h
    includes/mathematica++/m.h
    includes/mathematica++/io.h
    includes/mathematica++/symbol.h
    includes/mathematica++/rules.h
    includes/mathematica++/operators.h
    includes/mathematica++/declares.h
    includes/mathematica++/connection.h
    includes/mathematica++/packet.h
    includes/mathematica++/history.h
    includes/mathematica++/mhelper.h
)

SET(MATHEMATICAPP_SOURCES
    sources/scope.cpp
    sources/connector.cpp
    sources/exceptions.cpp
    sources/tokens.cpp
    sources/accessor.cpp
    sources/variant.cpp
    sources/drivers.cpp
    sources/m.cpp
    sources/io.cpp
    sources/symbol.cpp
    sources/rules.cpp
    sources/operators.cpp
    sources/declares.cpp
    sources/connection.cpp
    sources/packet.cpp
    sources/history.cpp
    sources/mhelper.cpp
)

IF (DEFINED XCODE_VERSION)
    ADD_LIBRARY(mathematica++ SHARED ${MATHEMATICAPP_SOURCES} ${MATHEMATICAPP_HEADERS})
ELSE()
    ADD_LIBRARY(mathematica++ SHARED ${MATHEMATICAPP_SOURCES})
ENDIF()

TARGET_INCLUDE_DIRECTORIES(mathematica++ PUBLIC ${MATHEMATICAPP_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(mathematica++ ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(mathematica++ ${Mathematica_LIBRARIES})
# TARGET_LINK_LIBRARIES(mathematica++ ${Mathematica_WSTP_LIBRARY})

SET(M_STORAGE_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/storage.m)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/includes/mathematica++/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/includes/mathematica++/config.h)

MESSAGE(STATUS "Expecting storage.m in " ${M_STORAGE_LIB_PATH})

ENABLE_TESTING()

ADD_SUBDIRECTORY(tests)
