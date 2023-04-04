CC = gcc
CFLAGS = -Wall -g -mavx -march=native 

equation: main.cpp equation.cpp comum.cpp timer.cpp comum.h gpu.h timer.h
	$(CC) $(CFLAGS) -o equation main.cpp equation.cpp comum.cpp timer.cpp
	