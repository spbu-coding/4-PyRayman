cmake_minimum_required(VERSION 3.7)
project(tests LANGUAGES C)

set(CMAKE_C_STANDARD 99)

add_executable(converter src/converter/converter.c src/converter/bmp.c )
add_executable(comparer src/converter/comparer.c src/converter/bmp.c  )



