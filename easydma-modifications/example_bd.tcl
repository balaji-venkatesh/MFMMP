proc easydma_create_ex_bd {easydma_config project_dir project_name} {
    set bd_name "easydma_ex"
    create_bd_design ${bd_name}
    current_bd_design [get_bd_designs ${bd_name}]

    set bar_size [dict get ${easydma_config} CONFIG.USR_BAR_SIZE]
    set dev_id [dict get ${easydma_config} CONFIG.DEV_ID]
    set os_pkts_cap [dict get ${easydma_config} CONFIG.OS_PKTS_CAP]
    set ref_clk_freq [dict get ${easydma_config} CONFIG.REF_CLK_FREQ]

    # Create interface ports
    set pci_exp [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:pcie_7x_mgt_rtl:1.0 pci_exp ]
    set pcie_clk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 pcie_clk ]

    # Create ports
    set pcie_rst [ create_bd_port -dir I -type rst pcie_rst ]

    # Create instance: axi_gpio_0, and set properties
    set axi_gpio_0 [ addip xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]

    # Create instance: easydma_0, and set properties
    set easydma_0 [ addip clarkshen.com:user:easydma:1.0 easydma_0 ]
    set_property -dict [ list \
        CONFIG.USR_BAR_SIZE "${bar_size}" \
        CONFIG.DEV_ID ${dev_id} \
        CONFIG.OS_PKTS_CAP "${os_pkts_cap}" \
        CONFIG.REF_CLK_FREQ "${ref_clk_freq}"
    ] $easydma_0    

    # Create instance: system_ila_0, and set properties
    set system_ila_0 [ addip xilinx.com:ip:system_ila:1.1 system_ila_0 ]
    set_property -dict [ list \
        CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    ] $system_ila_0

    # Create instance: vio_0, and set properties
    set vio_0 [ addip xilinx.com:ip:vio:3.0 vio_0 ]
    set_property -dict [ list \
        CONFIG.C_NUM_PROBE_IN {1} \
        CONFIG.C_NUM_PROBE_OUT {1} \
        CONFIG.C_PROBE_OUT0_INIT_VAL {0x00000011} \
        CONFIG.C_PROBE_OUT0_WIDTH {32} \
        CONFIG.C_PROBE_IN0_WIDTH {32} \
    ] $vio_0

    # Create instance: xlconstant_0, and set properties
    set xlconstant_0 [ addip xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

    # Create interface connections
    connect_bd_intf_net -intf_net easydma_0_h2c_chan0 [get_bd_intf_pins easydma_0/c2h_chan0] [get_bd_intf_pins easydma_0/h2c_chan0]
    connect_bd_intf_net -intf_net [get_bd_intf_nets easydma_0_h2c_chan0] [get_bd_intf_pins easydma_0/c2h_chan0] [get_bd_intf_pins system_ila_0/SLOT_0_AXIS]
    connect_bd_intf_net -intf_net easydma_0_m_axil [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins easydma_0/m_axil]
    connect_bd_intf_net -intf_net easydma_0_pci_exp [get_bd_intf_ports pci_exp] [get_bd_intf_pins easydma_0/pci_exp]
    connect_bd_intf_net -intf_net pcie_clk_1 [get_bd_intf_ports pcie_clk] [get_bd_intf_pins easydma_0/pcie_clk]

    # Create port connections
    connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins vio_0/probe_in0]
    connect_bd_net -net easydma_0_user_clk [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins easydma_0/user_clk] [get_bd_pins system_ila_0/clk] [get_bd_pins vio_0/clk]
    connect_bd_net -net pcie_rst_1 [get_bd_ports pcie_rst] [get_bd_pins easydma_0/pcie_rst]
    connect_bd_net -net vio_0_probe_out0 [get_bd_pins axi_gpio_0/gpio_io_i] [get_bd_pins vio_0/probe_out0]
    connect_bd_net -net xlconstant_0_dout [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins system_ila_0/resetn] [get_bd_pins xlconstant_0/dout]

    # Create address segments
    assign_bd_address -offset 0x00000000 -range 0x000100000000 -target_address_space [get_bd_addr_spaces easydma_0/m_axil] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg] -force

    validate_bd_design
    save_bd_design

    make_wrapper -files [get_files ${project_dir}/${project_name}.srcs/sources_1/bd/${bd_name}/${bd_name}.bd] -top
    if { [file exists "${project_dir}/${project_name}.srcs/sources_1/bd/${bd_name}/hdl/${bd_name}_wrapper.v"] } {
        add_files -fileset sources_1 -norecurse ${project_dir}/${project_name}.srcs/sources_1/bd/${bd_name}/hdl/${bd_name}_wrapper.v
    } elseif { [file exists "${project_dir}/${project_name}.gen/sources_1/bd/${bd_name}/hdl/${bd_name}_wrapper.v"] } {
        add_files -fileset sources_1 -norecurse ${project_dir}/${project_name}.gen/sources_1/bd/${bd_name}/hdl/${bd_name}_wrapper.v
    }
    regenerate_bd_layout
    set_property top ${bd_name}_wrapper [get_filesets sources_1]    
}
