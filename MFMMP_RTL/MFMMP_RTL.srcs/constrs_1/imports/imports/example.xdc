create_clock -period 10.000 [get_ports pcie_clk_clk_p*]

set_property PACKAGE_PIN AM11 [get_ports pcie_clk_clk_p*]
set_property PACKAGE_PIN AM10 [get_ports pcie_clk_clk_n*] 
set_property PACKAGE_PIN BD21 [get_ports pcie_rst]
set_property IOSTANDARD LVCMOS12 [get_ports pcie_rst]

set_property PULLUP true [get_ports pcie_rst]
set_false_path -from [get_ports pcie_rst]
