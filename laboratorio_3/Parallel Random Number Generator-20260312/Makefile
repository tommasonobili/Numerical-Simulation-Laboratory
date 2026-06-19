CC = g++
CFLAGS = -Wall -O3 --std=c++11

Ex_03_1.exe : Ex_03_1.o random.o
	$(CC) random.o Ex_03_1.o -o Ex_03_1.exe
Ex_03_1.o : Ex_03_1.cpp
	$(CC) -c Ex_03_1.cpp -o Ex_03_1.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o Ex_03_1.exe seed.out
