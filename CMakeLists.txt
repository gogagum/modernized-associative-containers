cmake_minimum_required(VERSION 3.20)

project(modernized-associative-containers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(modernized-associative-containers INTERFACE)

target_include_directories(modernized-associative-containers INTERFACE include)

add_library(reference-comparator-abi-test test/reference_comparator_abi.compile.pass.cpp)
add_executable(non-const-comparator-test test/non_const_comparator.verify.cpp)
add_executable(non-const-comparator-incomplete-test test/non_const_comparator.incomplete.verify.cpp)

include(GoogleTest)
find_package(GTest)

add_executable(tree-test test/tree_test.cpp)
target_include_directories(tree-test PRIVATE test/include)
target_link_libraries(tree-test GTest::gtest_main modernized-associative-containers)

add_executable(multiset-test
    test/set/multiset_compare.cpp
    test/set/multiset_constructor.cpp
    test/set/multiset_operations.cpp)
target_include_directories(multiset-test PRIVATE test/include)
target_link_libraries(multiset-test GTest::gtest_main modernized-associative-containers)

add_executable(set-test
    test/set/set_compare.cpp
    test/set/set_constructor.cpp
    test/set/set_operations.cpp)
target_include_directories(set-test PRIVATE test/include)
target_link_libraries(set-test GTest::gtest_main modernized-associative-containers)

add_executable(map-test
    test/map/map_allocator.cpp
    test/map/map_compare.cpp
    test/map/map_constructor.cpp
    test/map/map_insertion.cpp
    test/map/map_lookup.cpp
    test/map/map_operations.cpp
    test/src/tracker_allocator.cpp)

target_include_directories(map-test PRIVATE test/include)
target_link_libraries(map-test GTest::gtest_main modernized-associative-containers)

add_executable(multimap-test
    test/map/multimap_allocator.cpp
    test/map/multimap_compare.cpp
    test/map/multimap_constructor.cpp
    test/map/multimap_insertion.cpp
    test/map/multimap_operations.cpp
    test/src/tracker_allocator.cpp)

target_include_directories(multimap-test PRIVATE test/include)
target_link_libraries(multimap-test GTest::gtest_main modernized-associative-containers)

target_include_directories(reference-comparator-abi-test PRIVATE test/include)
target_link_libraries(reference-comparator-abi-test modernized-associative-containers)

target_link_libraries(non-const-comparator-test modernized-associative-containers)
target_link_libraries(non-const-comparator-incomplete-test modernized-associative-containers)

