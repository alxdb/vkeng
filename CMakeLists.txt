cmake_minimum_required(VERSION 3.24)
project(vkeng)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

add_subdirectory(src)
add_subdirectory(examples)
