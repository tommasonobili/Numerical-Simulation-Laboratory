CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo
all: Ex_07_2.exe
Ex_07_2.exe : system.o Ex_07_2.o particle.o random.o
	$(CC) system.o particle.o random.o Ex_07_2.o -o Ex_07_2.exe $(AFLAGS)
Ex_07_2.o : Ex_07_2.cpp system.h particle.h
	$(CC) -c Ex_07_2.cpp -o Ex_07_2.o $(CFLAGS)
system.o : system.cpp system.h random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)
particle.o : particle.cpp particle.h
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o Ex_07_2.exe
remove:
	rm ../OUTPUT_Ex_07_2/*.*
	rm ../OUTPUT_Ex_07_2/CONFIG/*.*
