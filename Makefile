FCT = flux_corrected_transport.o

make: $(FCT)
	gcc -o fludyntrans.exe fludyntrans.c flux_corrected_transport.o -lm

makeFCT:
	gcc -c flux_corrected_transport.c
	
run:
	./fludyntrans.exe && gnuplot plot_fludyn.p
