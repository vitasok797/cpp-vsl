cmake_minimum_required(VERSION 3.15)

add_library(vsl_lib INTERFACE)

target_compile_features(vsl_lib INTERFACE cxx_std_20)
target_compile_options(vsl_lib INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /Zc:preprocessor /wd4068>
)

target_include_directories(vsl_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../vsl/..")
target_include_directories(vsl_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external")

#--- dependencies --------------------------------------------------------------------------------

find_package(fmt CONFIG)
target_link_libraries(vsl_lib INTERFACE fmt::fmt)

find_package(libenvpp CONFIG)
target_link_libraries(vsl_lib INTERFACE libenvpp::libenvpp)

find_package(libfort CONFIG)
target_link_libraries(vsl_lib INTERFACE libfort::fort)

find_package(magic_enum CONFIG)
target_link_libraries(vsl_lib INTERFACE magic_enum::magic_enum)

find_package(nameof CONFIG)
target_link_libraries(vsl_lib INTERFACE nameof::nameof)

find_package(nlohmann_json CONFIG)
target_link_libraries(vsl_lib INTERFACE nlohmann_json::nlohmann_json)

set(UTF8PROC_DEPRECATED_CONFIG_FILE "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/unofficial-utf8proc/unofficial-utf8proc-config.cmake")
if(EXISTS "${UTF8PROC_DEPRECATED_CONFIG_FILE}")
    file(READ "${UTF8PROC_DEPRECATED_CONFIG_FILE}" FILE_CONTENT)
    set(PATTERN "message\\(WARNING \"[^\"]*\"\\)\n")
    string(REGEX REPLACE "${PATTERN}" "" FILE_CONTENT_MOD "${FILE_CONTENT}")
    file(WRITE "${UTF8PROC_DEPRECATED_CONFIG_FILE}" "${FILE_CONTENT_MOD}")
endif()
find_package(Poco COMPONENTS Foundation Net)
target_link_libraries(vsl_lib INTERFACE Poco::Foundation Poco::Net)

find_package(uni-algo CONFIG)
target_link_libraries(vsl_lib INTERFACE uni-algo::uni-algo)
