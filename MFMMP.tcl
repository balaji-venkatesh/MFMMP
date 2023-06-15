set project MFMMP
set design MFMMP
set script_dir [file dirname [file normalize [info script]]]
open_project $project
open_solution "solution1"
set_top $design
add_files MFMMP.cpp 
add_files -tb ./MFMMP_tb.cpp
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
