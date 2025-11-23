option(CRT_LINKAGE_STATIC "Link MSVC runtime statically" NO)

if(MSVC AND CRT_LINKAGE_STATIC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
