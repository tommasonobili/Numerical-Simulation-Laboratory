CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Ex_08_2.exe energia.exe

Ex_08_2.exe : Ex_08_2.o random.o
	$(CC) random.o Ex_08_2.o -o Ex_08_2.exe

energia.exe : energia.o random.o
	$(CC) random.o energia.o -o energia.exe

Ex_08_2.o : Ex_08_2.cpp
	$(CC) -c Ex_08_2.cpp -o Ex_08_2.o $(CFLAGS)

energia.o : energia.cpp random.h
	$(CC) -c energia.cpp -o energia.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm -f *.o Ex_08_2.exe energia.exe seed.out