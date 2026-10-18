cmake_minimum_required(VERSION 3.10)
project(MyServer)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS program_options)
find_library(CRYPTOPP_LIB NAMES crypto++ cryptopp)

add_executable(server main.cpp server.cpp auth.cpp)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES} ${CRYPTOPP_LIB} pthread)
