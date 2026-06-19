CC = mpic++
CFLAGS = -Wall -O3 --std=c++11

all: Exercise_10.1.exe 

Exercise_10.1.exe: Exercise_10.1.o random.o
	$(CC) Exercise_10.1.o random.o -o Exercise_10.1.exe

Exercise_10.1.o: Exercise_10.1.cpp random.h
	$(CC) -c Exercise_10.1.cpp -o Exercise_10.1.o $(CFLAGS)
	
random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean:
	rm -f *.o *.exe seed.out
	rm -f OUTPUT/losses_parallel_rank_*.dat
	rm -f OUTPUT/best_path_initial_parallel_rank_*.txt
	rm -f OUTPUT/best_path_final_parallel_rank_*.txt
