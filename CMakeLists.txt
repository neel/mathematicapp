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

OPTION(WITH_ML          "Use MathLink functions insteadof using WSTP"             ON)
OPTION(WITH_LL          "Support LibraryLink"                                     ON)
OPTION(WITH_WS          "Use WSTP functions insteadof using MathLink"             OFF)
OPTION(PRINT_TRACE      "Print low level the WS/ML functions that are being used" OFF)
OPTION(WITHOUT_EIGEN    "Compile without using eigen even if available"           OFF)

IF(WITH_WS)
    set(WITH_ML OFF CACHE INTERNAL "" FORCE)
    set(WITH_LL OFF CACHE INTERNAL "" FORCE)
ENDIF()

IF(WITH_ML)
    set(WITH_WS OFF CACHE INTERNAL "" FORCE)
    MESSAGE(">> ${BoldMagenta} Using MathLink instead of WSTP ${ColourReset}")
    IF(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        ADD_COMPILE_OPTIONS(-D USING_ML)
    ELSE()
        ADD_COMPILE_DEFINITIONS(USING_ML)
    ENDIF()
    INCLUDE_DIRECTORIES(${Mathematica_MathLink_INCLUDE_DIR})
ELSE()
    set(WITH_ML OFF CACHE INTERNAL "" FORCE)
    set(WITH_LL OFF CACHE INTERNAL "" FORCE)
    MESSAGE(">> ${BoldMagenta} Using WSTP instead of MathLink ${ColourReset}")
    IF(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        ADD_COMPILE_OPTIONS(-D USING_WS)
    ELSE()
        ADD_COMPILE_DEFINITIONS(USING_WS)
    ENDIF()
    INCLUDE_DIRECTORIES(${Mathematica_WSTP_INCLUDE_DIR})
ENDIF()

IF(WITH_LL)
    set(WITH_WS OFF CACHE INTERNAL "" FORCE)
    MESSAGE(">> ${BoldMagenta} LibraryLink Support added ${ColourReset}")
    IF(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        ADD_COMPILE_OPTIONS(-D USING_LL)
    ELSE()
        ADD_COMPILE_DEFINITIONS(USING_LL)
    ENDIF()
    INCLUDE_DIRECTORIES(${Mathematica_WolframLibrary_INCLUDE_DIR})
ENDIF()

IF(PRINT_TRACE)
    IF(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        ADD_COMPILE_OPTIONS(-D USING_DEBUG_TRACE)
    ELSE()
        ADD_COMPILE_DEFINITIONS(USING_DEBUG_TRACE)
    ENDIF()
ENDIF()


IF(NOT WITHOUT_EIGEN)
    FIND_PACKAGE(Eigen3 NO_MODULE)
    IF(Eigen3_FOUND)
        INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
        IF(${CMAKE_VERSION} VERSION_LESS "3.12.0")
            ADD_COMPILE_OPTIONS(-D USING_LIB_EIGEN)
        ELSE()
            ADD_COMPILE_DEFINITIONS(USING_LIB_EIGEN)
        ENDIF()
        MESSAGE(">> ${BoldMagenta} Using Eigen3 library ${ColourReset}")
    ENDIF()
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
    includes/mathematica++/association.h
)

IF(WITH_LL)
    SET(MATHEMATICAPP_SOURCES ${MATHEMATICAPP_SOURCES}
        includes/mathematica++/transport.h
        includes/mathematica++/library.h
    )
ENDIF()

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
    sources/association.cpp
)

IF(WITH_LL)
    SET(MATHEMATICAPP_SOURCES ${MATHEMATICAPP_SOURCES}
        sources/transport.cpp
        sources/library.cpp
    )
ENDIF()

IF (DEFINED XCODE_VERSION)
    ADD_LIBRARY(mathematica++ SHARED ${MATHEMATICAPP_SOURCES} ${MATHEMATICAPP_HEADERS})
ELSE()
    ADD_LIBRARY(mathematica++ SHARED ${MATHEMATICAPP_SOURCES})
ENDIF()

TARGET_INCLUDE_DIRECTORIES(mathematica++ PUBLIC ${MATHEMATICAPP_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(mathematica++ ${Boost_LIBRARIES})
IF(WITH_ML)
    TARGET_LINK_LIBRARIES(mathematica++ ${Mathematica_MathLink_LIBRARIES})
    TARGET_LINK_LIBRARIES(mathematica++ ${Mathematica_WolframLibrary_LIBRARIES})
ELSE()
    TARGET_LINK_LIBRARIES(mathematica++ ${Mathematica_WSTP_LIBRARIES})
ENDIF()

SET(M_STORAGE_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/storage.m)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/includes/mathematica++/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/includes/mathematica++/config.h)

MESSAGE(STATUS "Expecting storage.m in " ${M_STORAGE_LIB_PATH})

ENABLE_TESTING()

ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)
