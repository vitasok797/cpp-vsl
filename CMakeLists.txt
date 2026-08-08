cmake_minimum_required(VERSION 3.30)

project(vsl_dev)

include(cmake/modules.cmake)

#--- common --------------------------------------------------------------------------------------

function(configure_cxx_options target)
    target_cxx_standard(${target} 20)
    target_set_cxx_base_compile_options(${target})
    target_treat_warnings_as_errors(${target})
endfunction()

#--- dependencies --------------------------------------------------------------------------------

set(CUR_TARGET dependencies)
add_library(${CUR_TARGET} INTERFACE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vsl)
target_link_libraries(${CUR_TARGET} INTERFACE vsl_lib)

find_package(nanobench CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE nanobench::nanobench)

#--- interrupt demo ------------------------------------------------------------------------------

set(CUR_TARGET interrupt_demo)
add_executable(${CUR_TARGET})

target_link_libraries(${CUR_TARGET} PRIVATE dependencies)

configure_cxx_options(${CUR_TARGET})

target_sources(${CUR_TARGET} PRIVATE
    "vsl/interrupt/demo.cpp"
    )

#--- tests executable ----------------------------------------------------------------------------

option(VSL_BUILD_TESTS "Build tests" ON)

if(VSL_BUILD_TESTS AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    set(CUR_TARGET tests)
    add_executable(${CUR_TARGET})

    target_link_libraries(${CUR_TARGET} PRIVATE GTest::gtest_main GTest::gmock)
    target_link_libraries(${CUR_TARGET} PRIVATE dependencies)

    configure_cxx_options(${CUR_TARGET})

    target_sources(${CUR_TARGET} PRIVATE
        "vsl/assert.test.cpp"
        "vsl/concepts.test.cpp"
        "vsl/container.test.cpp"
        "vsl/csv.test.cpp"
        "vsl/debug.test.cpp"
        "vsl/encoding.test.cpp"
        "vsl/enum.test.cpp"
        "vsl/env.test.cpp"
        "vsl/flat_map.test.cpp"
        "vsl/json.test.cpp"
        "vsl/math.test.cpp"
        "vsl/pointer.test.cpp"
        "vsl/reflection.test.cpp"
        "vsl/regex.test.cpp"
        "vsl/table.test.cpp"
        "vsl/tcp/test.cpp"
        "vsl/text.test.cpp"
        "vsl/types.test.cpp"
        "vsl/unicode.test.cpp"
        "vsl/util.test.cpp"
        )

    include(GoogleTest)
    gtest_discover_tests(${CUR_TARGET})
endif()

#--- benchmark executable ------------------------------------------------------------------------

set(CUR_TARGET benchmark)
add_executable(${CUR_TARGET})

target_link_libraries(${CUR_TARGET} PRIVATE dependencies)

configure_cxx_options(${CUR_TARGET})

target_sources(${CUR_TARGET} PRIVATE
    "bench/benchmark.cpp"
    )
