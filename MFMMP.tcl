set script_dir [file dirname [file normalize [info script]]]
open_project MFMMP_proj
open_solution "solution1"
set_top MFMMP

add_files MFMMP.cpp 
add_files -tb ./MFMMP_cnntb.cpp
add_files -tb util/shared.cpp
add_files -tb nn_params

create_clock -period 5 -name default
if { [info exists ::env(DO_COSIM) ] } {
    set_part  {xcvu9p-flga2104-2-i}
    csynth_design
    export_design -rtl verilog
    cosim_design
} else {
    csim_design
}
exit
