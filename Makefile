DESIGN=MFMMP

csim:
	@rm -rf $(DESIGN)
	@unset DO_COSIM
	vitis_hls -f $(DESIGN).tcl

cosim:
	@rm -rf $(DESIGN)
	export DO_COSIM=1 && vitis_hls -f $(DESIGN).tcl
	cat $(DESIGN)/solution1/sim/report/$(DESIGN)_cosim.rpt

clean:
	rm -rf $(DESIGN)
	rm -f *.log
