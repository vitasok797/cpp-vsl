function(target_config_compile_options target config scope)
    set(options ${ARGN})
    target_compile_options(${target} ${scope} $<$<CONFIG:${config}>:${options}>)
endfunction()

function(add_files_compile_options)
    set(optionsArgs "")
    set(oneValueArgs CONFIG)
    set(multiValueArgs FILES OPTIONS)
    cmake_parse_arguments(ARG "${optionsArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_FILES)
        message(FATAL_ERROR "add_files_compile_options: FILES argument is not set")
    endif()

    if(NOT ARG_OPTIONS)
        message(FATAL_ERROR "add_files_compile_options: OPTIONS argument is not set")
    endif()

    set(final_options ${ARG_OPTIONS})
    if(ARG_CONFIG)
        set(final_options "$<$<CONFIG:${ARG_CONFIG}>:${ARG_OPTIONS}>")
    endif()

    foreach(file IN LISTS ARG_FILES)
        string(STRIP "${file}" file)

        get_source_file_property(cur_options "${file}" COMPILE_OPTIONS)
        if(NOT cur_options)
            set(cur_options "")
        endif()

        list(APPEND cur_options ${final_options})
        list(REMOVE_DUPLICATES cur_options)

        set_source_files_properties("${file}" PROPERTIES COMPILE_OPTIONS "${cur_options}")
    endforeach()
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
