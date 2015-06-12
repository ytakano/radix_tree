# We try to find GTest package,
#  if it was not found or we could not be simple test,
#  add gtest as external project

find_package(GTest QUIET)

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)
include(ExternalProject)
include(FindPackageMessage)

function(add_gtest_as_external)
    set (gtest_local "/usr/src/gtest/")
    if (EXISTS "${gtest_local}/CMakeLists.txt")
        set (gtest_url URL ${gtest_local})
        FIND_PACKAGE_MESSAGE(GTest "Trying to build GTest from local ${gtest_local}" "[${gtest_local}]")
    else()
        set (gtest_web_url "https://googletest.googlecode.com/files/gtest-1.7.0.zip")
        FIND_PACKAGE_MESSAGE(GTest "Trying to build GTest from ${gtest_web_url}" "[${gtest_web_url}]")
        set (gtest_url
            URL ${gtest_web_url}
            LOG_DOWNLOAD Yes
        )
    endif()

    ExternalProject_Add(
        gtest-external ${gtest_url}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -Dgtest_disable_pthreads=Off
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            # We don't want to check compiler by subproject again
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_COMPILER_ID_RUN=TRUE
            -DCMAKE_CXX_COMPILER_WORKS=TRUE
            -DCMAKE_CXX_COMPILER_FORCED=TRUE
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_C_COMPILER_ID_RUN=TRUE
            -DCMAKE_C_COMPILER_WORKS=TRUE
            -DCMAKE_C_COMPILER_FORCED=TRUE
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}\ -Wno-missing-field-initializers
    )
    ExternalProject_Get_Property(gtest-external source_dir binary_dir)
    set(GTEST_INCLUDE_DIR "${source_dir}/include" CACHE PATH "Path to directory." FORCE)

    add_library(gtest IMPORTED STATIC GLOBAL)
    add_dependencies(gtest gtest-external)
    set(GTEST_LIBRARY gtest CACHE STRING "Imported library." FORCE)
    set(GTEST_LIBRARY_DEBUG gtest CACHE STRING "Imported library." FORCE)
    set_property(TARGET gtest PROPERTY
        IMPORTED_LOCATION "${binary_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")

    add_library(gtest_main IMPORTED STATIC GLOBAL)
    add_dependencies(gtest_main gtest-external)
    set(GTEST_MAIN_LIBRARY gtest_main CACHE STRING "Imported library." FORCE)
    set(GTEST_MAIN_LIBRARY_DEBUG gtest_main CACHE STRING "Imported library." FORCE)
    set_property(TARGET gtest_main PROPERTY
        IMPORTED_LOCATION "${binary_dir}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}")

    set(GTEST_BOTH_LIBRARIES "${GTEST_MAIN_LIBRARY}" "${GTEST_LIBRARY}" PARENT_SCOPE)
    set(GTEST_FOUND TRUE CACHE INTERNAL "GTEST_FOUND")
endfunction()

function(try_compile_gtest out_var)
    CMAKE_PUSH_CHECK_STATE()
    set(CMAKE_REQUIRED_FLAGS -Wl,-verbose)
    set(CMAKE_REQUIRED_LIBRARIES ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    CHECK_CXX_SOURCE_COMPILES("#include <gtest/gtest.h>\n TEST(x,y){}" ${out_var})
    CMAKE_POP_CHECK_STATE()
endfunction()

if (GTEST_FOUND)
    try_compile_gtest(GTEST_COMPILED_AND_LINKED)
    if (NOT GTEST_COMPILED_AND_LINKED)
        add_gtest_as_external()
    endif()
else()
    add_gtest_as_external()
endif()
