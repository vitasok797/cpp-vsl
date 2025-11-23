function(target_config_compile_options target config scope)
    set(options ${ARGN})
    target_compile_options(${target} ${scope} $<$<CONFIG:${config}>:${options}>)
endfunction()

function(add_files_compile_options files)
    set(options ${ARGN})
    foreach(file ${files})
        string(STRIP "${file}" file)

        get_source_file_property(cur_options "${file}" COMPILE_OPTIONS)
        if(NOT cur_options)
            set(cur_options "")
        endif()

        list(APPEND cur_options ${options})
        set_source_files_properties("${file}" PROPERTIES COMPILE_OPTIONS "${cur_options}")
    endforeach()
endfunction()

function(add_files_config_compile_options files config)
    add_files_compile_options("${files}" $<$<CONFIG:${config}>:${ARGN}>)
endfunction()

function(target_treat_warnings_as_errors target)
    set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR YES)
endfunction()

function(set_libraries_debug_postfix postfix)
    set(CMAKE_DEBUG_POSTFIX ${postfix} PARENT_SCOPE)
endfunction()

function(set_executable_debug_postfix target postfix)
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX ${postfix})
endfunction()
