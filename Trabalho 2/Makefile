CC = gcc
CFLAGS = -Wall -g -pthread -mavx -march=native

equation: main.cpp equation.cpp comum.cpp timer.cpp comum.h gpu.h timer.h thread.h
	$(CC) $(CFLAGS) -o equation.exe main.cpp equation.cpp comum.cpp timer.cpp
	
run_exe:
	./equation.exe -m matriz_A.bin -n 1024 -o tempos.txt -v vetor_B.bin