cmake_minimum_required(VERSION 3.17)
project(rhubarb)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

add_executable(rhubarb "src/main.cpp" "src/bench.cpp" "src/board.cpp" "src/types.cpp" "src/bitboard.cpp" "src/movegen.cpp" "src/makemove.cpp" "src/perft.cpp" "src/parser.cpp" "src/uci.cpp" "src/evaluation.cpp" "src/search.cpp" "src/zobrist.cpp" "src/timemanagement.cpp")