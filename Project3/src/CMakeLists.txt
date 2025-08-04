cmake_minimum_required(VERSION 3.2)
project(MyMTree VERSION 1.0 LANGUAGES CXX)

add_library(Entry entry.cpp)
add_library(MTree m_tree.cpp)

target_link_libraries(MTree
        PRIVATE Entry
)

add_executable(MTreeApp main.cpp)

target_link_libraries(MTreeApp
        PUBLIC MTree
        PUBLIC Entry
)     
