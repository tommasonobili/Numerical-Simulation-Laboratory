CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Generate_square_cities.exe Exercise_9.2.exe

Generate_square_cities.exe: Generate_square_cities.o random.o
	$(CC) Generate_square_cities.o random.o -o Generate_square_cities.exe

Generate_square_cities.o: Generate_square_cities.cpp random.h
	$(CC) -c Generate_square_cities.cpp -o Generate_square_cities.o $(CFLAGS)

Exercise_9.2.exe: Exercise_9.2.o random.o
	$(CC) Exercise_9.2.o random.o -o Exercise_9.2.exe

Exercise_9.2.o: Exercise_9.2.cpp random.h
	$(CC) -c Exercise_9.2.cpp -o Exercise_9.2.o $(CFLAGS)

random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean:
	rm -f *.o *.exe seed.out