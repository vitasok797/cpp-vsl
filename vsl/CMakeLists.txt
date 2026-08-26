cmake_minimum_required(VERSION 3.15)

add_library(vsl_lib INTERFACE)

target_compile_features(vsl_lib INTERFACE cxx_std_20)
target_compile_options(vsl_lib INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /Zc:preprocessor /wd4068>
)

# Disable min/max macros from Windows headers
target_compile_definitions(vsl_lib INTERFACE NOMINMAX)

target_include_directories(vsl_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../vsl/..")
target_include_directories(vsl_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external")

#-------------------------------------------------------------------------------------------------

function(link_library lib_target)
    if(TARGET ${lib_target})
        target_link_libraries(vsl_lib INTERFACE ${lib_target})
        message(STATUS "Linked VSL lib dependency: ${lib_target}")
    else()
        message(STATUS "Skipped VSL lib dependency: ${lib_target}")
    endif()
endfunction()

function(link_library_one_of)
    foreach(lib_target IN LISTS ARGN)
        if(TARGET ${lib_target})
            link_library(${lib_target})
            return()
        endif()
    endforeach()
    message(STATUS "Skipped VSL lib dependency: ${ARGN}")
endfunction()

#--- dependencies --------------------------------------------------------------------------------

find_package(fmt CONFIG)
link_library(fmt::fmt)

find_package(libenvpp CONFIG)
link_library(libenvpp::libenvpp)

find_package(libfort CONFIG)
link_library(libfort::fort)

find_package(magic_enum CONFIG)
link_library(magic_enum::magic_enum)

find_package(nameof CONFIG)
link_library(nameof::nameof)

find_package(nlohmann_json CONFIG)
link_library(nlohmann_json::nlohmann_json)

set(UTF8PROC_DEPRECATED_CONFIG_FILE "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/unofficial-utf8proc/unofficial-utf8proc-config.cmake")
if(EXISTS "${UTF8PROC_DEPRECATED_CONFIG_FILE}")
    file(READ "${UTF8PROC_DEPRECATED_CONFIG_FILE}" FILE_CONTENT)
    set(PATTERN "message\\(WARNING \"[^\"]*\"\\)\n")
    string(REGEX REPLACE "${PATTERN}" "" FILE_CONTENT_MOD "${FILE_CONTENT}")
    file(WRITE "${UTF8PROC_DEPRECATED_CONFIG_FILE}" "${FILE_CONTENT_MOD}")
endif()
find_package(Poco COMPONENTS Foundation Net)
link_library(Poco::Foundation)
link_library(Poco::Net)

find_package(uni-algo CONFIG)
link_library(uni-algo::uni-algo)
