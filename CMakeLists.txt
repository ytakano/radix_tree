cmake_minimum_required(VERSION 2.8.4)

project(radix-tree)

install(FILES radix_tree.hpp radix_tree_it.hpp radix_tree_node.hpp DESTINATION include/radix_tree)


option(BUILD_TESTS "Should we build tests?" OFF)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "radix tree")
set (CPACK_DEBIAN_PACKAGE_DESCRIPTION # The format of Description: http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Description
"Implementation of radix tree in C++
")
set (CPACK_PACKAGE_CONTACT "Yuuki Takano <ytakanoster@gmail.com>")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_SOURCE_GENERATOR "")
set (CPACK_STRIP_FILES "")
set (CPACK_GENERATOR "DEB")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER "nobody")
include(CPack)
