cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(modernized-associative-containers INTERFACE)

target_include_directories(modernized-associative-containers INTERFACE include)

add_executable(tree-right-rotate-test test/tree_right_rotate.pass.cpp)
add_executable(tree-remove-test test/tree_remove.pass.cpp)
add_executable(tree-left-rotate-test test/tree_left_rotate.pass.cpp)
add_executable(tree-balance-after-insert-test test/tree_balance_after_insert.pass.cpp)
add_executable(reference-comparator-abi-test test/reference_comparator_abi.compile.pass.cpp)
add_executable(non-const-comparator-test test/non_const_comparator.verify.cpp)
add_executable(non-const-comparator-incomplete-test test/non_const_comparator.incomplete.verify.cpp)

target_include_directories(tree-right-rotate-test PRIVATE test/include)
target_link_libraries(tree-right-rotate-test modernized-associative-containers)

target_include_directories(tree-remove-test PRIVATE test/include)
target_link_libraries(tree-remove-test modernized-associative-containers)

target_include_directories(tree-left-rotate-test PRIVATE test/include)
target_link_libraries(tree-left-rotate-test modernized-associative-containers)

target_include_directories(tree-balance-after-insert-test PRIVATE test/include)
target_link_libraries(tree-balance-after-insert-test modernized-associative-containers)

target_include_directories(reference-comparator-abi-test PRIVATE test/include)
target_link_libraries(reference-comparator-abi-test modernized-associative-containers)

target_link_libraries(non-const-comparator-test modernized-associative-containers)
target_link_libraries(non-const-comparator-incomplete-test modernized-associative-containers)

# add_executable(multiset-nodiscard-verify-test test/set/multiset.nodiscard.verify.cpp)
# target_include_directories(multiset-nodiscard-verify-test PRIVATE test/include)
# target_link_libraries(multiset-nodiscard-verify-test modernized-associative-containers)

# add_executable(set-nodiscard-verify-test test/set/set.nodiscard.verify.cpp)
# target_include_directories(set-nodiscard-verify-test PRIVATE test/include)
# target_link_libraries(set-nodiscard-verify-test modernized-associative-containers)

add_executable(at-abort-pass-test test/map/at.abort.pass.cpp)
target_include_directories(at-abort-pass-test PRIVATE test/include)
target_link_libraries(at-abort-pass-test PRIVATE modernized-associative-containers)

add_executable(at-const-abort-pass-test test/map/at.const.abort.pass.cpp)
target_include_directories(at-const-abort-pass-test PRIVATE test/include)
target_link_libraries(at-const-abort-pass-test PRIVATE modernized-associative-containers)

# add_executable(map-nodiscard-verify-test test/map/map.nodiscard.verify.cpp)
# target_include_directories(map-nodiscard-verify-test PRIVATE test/include)
# target_link_libraries(map-nodiscard-verify-test modernized-associative-containers)

# add_executable(multimap-nodiscard-verify-test test/map/multimap.nodiscard.verify.cpp)
# target_include_directories(multimap-nodiscard-verify-test PRIVATE test/include)
# target_link_libraries(multimap-nodiscard-verify-test modernized-associative-containers)
