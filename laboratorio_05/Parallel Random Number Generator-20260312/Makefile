CC = g++
CFLAGS = -Wall -O3 --std=c++11

all : Ex_05_1.exe Ex_05_1_far_from_origin.exe

Ex_05_1.exe : Ex_05_1.o random.o
	$(CC) random.o Ex_05_1.o -o Ex_05_1.exe

Ex_05_1.o : Ex_05_1.cpp
	$(CC) -c Ex_05_1.cpp -o Ex_05_1.o $(CFLAGS)

Ex_05_1_far_from_origin.exe : Ex_05_1_far_from_origin.o random.o
	$(CC) random.o Ex_05_1_far_from_origin.o -o Ex_05_1_far_from_origin.exe

Ex_05_1_far_from_origin.o : Ex_05_1_far_from_origin.cpp
	$(CC) -c Ex_05_1_far_from_origin.cpp -o Ex_05_1_far_from_origin.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm *.o Ex_05_1.exe Ex_05_1_far_from_origin.exe seed.out