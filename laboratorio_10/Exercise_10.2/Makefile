CC = mpic++
CFLAGS = -Wall -O3 --std=c++11

all: Exercise_10.2_sequential.exe Exercise_10.2_parallel.exe 

Exercise_10.2_sequential.exe: Exercise_10.2_sequential.o random.o
	$(CC) Exercise_10.2_sequential.o random.o -o Exercise_10.2_sequential.exe

Exercise_10.2_parallel.exe: Exercise_10.2_parallel.o random.o
	$(CC) Exercise_10.2_parallel.o random.o -o Exercise_10.2_parallel.exe

Exercise_10.2_sequential.o: Exercise_10.2_sequential.cpp random.h
	$(CC) -c Exercise_10.2_sequential.cpp -o Exercise_10.2_sequential.o $(CFLAGS)

Exercise_10.2_parallel.o: Exercise_10.2_parallel.cpp random.h
	$(CC) -c Exercise_10.2_parallel.cpp -o Exercise_10.2_parallel.o $(CFLAGS)
	
random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean:
	rm -f *.o *.exe seed.out
	rm -f losses_sequential_rank_*.dat
	rm -f losses_parallel_rank_*.dat
	rm -f best_path_initial_sequential_rank_*.txt
	rm -f best_path_initial_parallel_rank_*.txt
	rm -f best_path_final_sequential_rank_*.txt
	rm -f best_path_final_parallel_rank_*.txt
