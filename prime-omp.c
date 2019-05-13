/*
 * Kobe Davis
 * Prof. Li
 * CS 415P
 * 13 February 2019
 *
 * Assignment 2: Strategy 2 using OpenMP
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int printPrime(int *, int);

int main() {
    int nThreads;
    printf("Enter threads: ");
    scanf("%d", &nThreads);

    double primeBound;
    printf("Enter upper bound: ");
    scanf("%lf", &primeBound);
    double searchBound = sqrt(primeBound);

    int primeArray[(int)primeBound];
    for(int i = 0; i < primeBound; ++i)
        primeArray[i] = 1;
    primeArray[0] = primeArray[1] = 0;

    double nComposite = 0;
    double compRange = 0;
    double interval = 0;
    int total = 0;
    for(int i = 2; i < searchBound; ++i) {
        nComposite = ceil(primeBound / i); // Number of composites in range(100)
        compRange = ceil(nComposite / nThreads); // The number of elements each thread will have to iterate
        interval = i*compRange;
       
        #pragma omp parallel num_threads(nThreads) shared(primeArray, nComposite, compRange, interval) reduction(+:total)
        {
            int id = omp_get_thread_num();
            int low = interval*id;
            int high = low + interval;
            if(!id)
                low += 2*i;
            if(id == nThreads-1)
                high = primeBound;

            for(int j = low; j < high; j += i) {
                if(j > primeBound)
                    break;
                if(primeArray[j]) {
                    #pragma omp critical
                    {
                        primeArray[j] = 0;
                        ++total;
                    }
                }
            }
        }
    }

    printf("\nNumber of primes in range(%d): %d\n", (int)primeBound, printPrime(primeArray, primeBound));
    printf("Total number of composites: %d\n", total);

    return 0;
}

int printPrime(int * toPrint, int size) {
    printf("\nPrimes: [");
    int count = 0;
    for(int i = 0, j = 1; i < size; ++i) {
        if(toPrint[i]) {
            ++count;
            j ? (j = 0, printf("%d", i)) : printf(", %d", i); 
        }
    }   
    printf("]\nFinished.\n");

    return count;
}
