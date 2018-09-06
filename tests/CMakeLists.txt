CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

FIND_PACKAGE(Boost COMPONENTS unit_test_framework filesystem system date_time REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(connection connection.cpp)
TARGET_LINK_LIBRARIES(connection ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(connection mathematica++)

ADD_EXECUTABLE(chain chain.cpp)
TARGET_LINK_LIBRARIES(chain ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(chain mathematica++)

ADD_EXECUTABLE(shell shell.cpp)
TARGET_LINK_LIBRARIES(shell ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(shell mathematica++)

ADD_EXECUTABLE(common common.cpp)
TARGET_LINK_LIBRARIES(common ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(common mathematica++)

ADD_EXECUTABLE(storage storage.cpp)
TARGET_LINK_LIBRARIES(storage ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(storage mathematica++)

ADD_EXECUTABLE(stl stl.cpp)
TARGET_LINK_LIBRARIES(stl ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(stl mathematica++)

IF(NOT WITHOUT_EIGEN)
    FIND_PACKAGE(Eigen3 NO_MODULE)
    IF(Eigen3_FOUND)
        ADD_EXECUTABLE(eigen eigen.cpp)
        TARGET_LINK_LIBRARIES(eigen ${Boost_LIBRARIES})
        TARGET_LINK_LIBRARIES(eigen mathematica++)
        
        ADD_TEST(eigen eigen --report_level=short --log_level=message --show_progress=true)
    ENDIF()
ENDIF()

ENABLE_TESTING()

ADD_TEST(connection connection --report_level=short --log_level=message --show_progress=true)
ADD_TEST(chain chain --report_level=short --log_level=message --show_progress=true)
ADD_TEST(shell shell --report_level=short --log_level=message --show_progress=true)
ADD_TEST(common common --report_level=short --log_level=message --show_progress=true)
ADD_TEST(storage storage --report_level=short --log_level=message --show_progress=true)
ADD_TEST(stl stl --report_level=short --log_level=message --show_progress=true)
