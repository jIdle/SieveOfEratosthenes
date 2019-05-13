prime-pthd%: prime-pthd%.c
	gcc -Wall -g -pthread $< -lm
prime-omp: prime-omp.c
	gcc -Wall -g -fopenmp $< -lm
