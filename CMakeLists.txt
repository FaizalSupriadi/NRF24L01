cmake_minimum_required(VERSION 3.13)
project(NRF24L01)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(NRF24L01
        NRF24.cpp
        NRF24.hpp
        NRF24_register.hpp)
