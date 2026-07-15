cmake_minimum_required(VERSION 3.30)

project(vsl)

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

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE fmt::fmt)

find_package(libenvpp CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE libenvpp::libenvpp)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE magic_enum::magic_enum)

find_package(nameof CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE nameof::nameof)

find_package(nanobench CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE nanobench::nanobench)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE nlohmann_json::nlohmann_json)

set(UTF8PROC_DEPRECATED_CONFIG_FILE "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/unofficial-utf8proc/unofficial-utf8proc-config.cmake")
if(EXISTS "${UTF8PROC_DEPRECATED_CONFIG_FILE}")
    file(WRITE "${UTF8PROC_DEPRECATED_CONFIG_FILE}" "find_package(utf8proc CONFIG REQUIRED)\n")
endif()
find_package(Poco REQUIRED COMPONENTS Foundation Net)
target_link_libraries(${CUR_TARGET} INTERFACE Poco::Foundation Poco::Net)

find_package(tabulate CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE tabulate::tabulate)

find_package(uni-algo CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE uni-algo::uni-algo)

#--- interrupt demo ------------------------------------------------------------------------------

set(CUR_TARGET interrupt_demo)
add_executable(${CUR_TARGET})

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

    target_include_directories(${CUR_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_include_directories(${CUR_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vsl/external")

    target_link_libraries(${CUR_TARGET} PRIVATE dependencies)
    configure_cxx_options(${CUR_TARGET})

    target_sources(${CUR_TARGET} PRIVATE
        "vsl/concepts.test.cpp"
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
        "vsl/tabulate.test.cpp"
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

target_include_directories(${CUR_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${CUR_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vsl/external")

target_link_libraries(${CUR_TARGET} PRIVATE dependencies)
configure_cxx_options(${CUR_TARGET})

target_sources(${CUR_TARGET} PRIVATE
    "vsl/benchmark/benchmark.cpp"
    )
