CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Generate_circumference_cities.exe Exercise_9.1.exe

Generate_circumference_cities.exe: Generate_circumference_cities.o random.o
	$(CC) Generate_circumference_cities.o random.o -o Generate_circumference_cities.exe

Generate_circumference_cities.o: Generate_circumference_cities.cpp random.h
	$(CC) -c Generate_circumference_cities.cpp -o Generate_circumference_cities.o $(CFLAGS)

Exercise_9.1.exe: Exercise_9.1.o random.o
	$(CC) Exercise_9.1.o random.o -o Exercise_9.1.exe

Exercise_9.1.o: Exercise_9.1.cpp random.h
	$(CC) -c Exercise_9.1.cpp -o Exercise_9.1.o $(CFLAGS)

random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean:
	rm -f *.o *.exe seed.out