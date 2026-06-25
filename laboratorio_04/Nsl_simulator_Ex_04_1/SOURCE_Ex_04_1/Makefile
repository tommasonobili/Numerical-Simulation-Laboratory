CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo
all: Ex_04_1.exe
Ex_04_1.exe : system.o Ex_04_1.o particle.o random.o
	$(CC) system.o particle.o random.o Ex_04_1.o -o Ex_04_1.exe $(AFLAGS)
Ex_04_1.o : Ex_04_1.cpp system.h particle.h
	$(CC) -c Ex_04_1.cpp -o Ex_04_1.o $(CFLAGS)
system.o : system.cpp system.h random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)
particle.o : particle.cpp particle.h
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o Ex_04_1.exe
remove:
	rm ../OUTPUT_Ex_04_1/*.*
	rm ../OUTPUT_Ex_04_1/CONFIG/*.*
