CC = gcc
CFLAGS = -Wall -g -mavx -march=native 

equation: main.cpp equation.cpp comum.cpp timer.cpp comum.h gpu.h timer.h
	$(CC) $(CFLAGS) -o equation.exe main.cpp equation.cpp comum.cpp timer.cpp
	
run_exe:
	./equation.exe -m matriz_A.bin -n 32 -o tempos.txt -v vetor_B.bin