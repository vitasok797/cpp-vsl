function(use_fortran_linker target)
    set_target_properties(${target} PROPERTIES LINKER_LANGUAGE Fortran)
endfunction()
