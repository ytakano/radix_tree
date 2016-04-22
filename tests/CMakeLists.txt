include(GTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

macro(cxx_test pretty_name bin_name sources libs)
    add_executable(${bin_name} ${sources})
    target_link_libraries(${bin_name} ${libs} ${GTEST_BOTH_LIBRARIES})
    add_test(${pretty_name} ${bin_name})
    add_dependencies(check ${bin_name})
endmacro()

include_directories(${CMAKE_SOURCE_DIR} ${GTEST_INCLUDE_DIR})

cxx_test("radix_tree::insert" test_radix_tree_insert "test_radix_tree_insert.cpp" "-pthread")
cxx_test("radix_tree::erase" test_radix_tree_erase "test_radix_tree_erase.cpp" "-pthread")
cxx_test("radix_tree::find" test_radix_tree_find "test_radix_tree_find.cpp" "-pthread")
cxx_test("radix_tree::prefix_match" test_radix_tree_prefix_match "test_radix_tree_prefix_match.cpp" "-pthread")
cxx_test("radix_tree::longest_match" test_radix_tree_longest_match "test_radix_tree_longest_match.cpp" "-pthread")
cxx_test("radix_tree::greedy_match" test_radix_tree_greedy_match "test_radix_tree_greedy_match.cpp" "-pthread")
cxx_test("radix_tree_iterator" test_radix_tree_iterator "test_radix_tree_iterator.cpp" "-pthread")
