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

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE Microsoft.GSL::GSL)

find_package(Poco REQUIRED COMPONENTS Encodings Foundation Net)
target_link_libraries(${CUR_TARGET} INTERFACE Poco::Encodings Poco::Foundation Poco::Net)

find_package(tabulate CONFIG REQUIRED)
target_link_libraries(${CUR_TARGET} INTERFACE tabulate::tabulate)

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
    set(CUR_TARGET run_tests)
    add_executable(${CUR_TARGET})

    target_include_directories(${CUR_TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${CUR_TARGET} PRIVATE dependencies)
    configure_cxx_options(${CUR_TARGET})

    target_sources(${CUR_TARGET} PRIVATE
        "test/run_tests.cpp"
        "vsl/tabulate/tabulate.test.cpp"
        "vsl/tcp/test.cpp"
        )
endif()
