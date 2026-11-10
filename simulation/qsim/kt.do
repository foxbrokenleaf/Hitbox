onerror {quit -f}
vlib work
vlog -work work kt.vo
vlog -work work kt.vt
vsim -novopt -c -t 1ps -L cycloneive_ver -L altera_ver -L altera_mf_ver -L 220model_ver -L sgate work.kt_vlg_vec_tst
vcd file -direction kt.msim.vcd
vcd add -internal kt_vlg_vec_tst/*
vcd add -internal kt_vlg_vec_tst/i1/*
add wave /*
run -all
