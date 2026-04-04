option(MSVC_RUNTIME_STATIC_LINKAGE "Link MSVC runtime statically" NO)

if(MSVC AND MSVC_RUNTIME_STATIC_LINKAGE)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
