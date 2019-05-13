/*
 * Kobe Davis
 * Prof. Li
 * CS 415P
 * 13 February 2019
 *
 * Assignment 2: Strategy 1 using pthreads
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>

double primeBound;
double searchBound;
int nThreads;
int * array;
int * sieve;
int * compCount;
int sLen;

pthread_mutex_t lock;

void find_sieves();
void printArray(int *, int);
int printPrime();
void worker(void *);

int main() {
    printf("\nEnter upper bound: ");
    scanf("%lf", &primeBound);

    printf("\nEnter threads: ");
    scanf("%d", &nThreads);

    searchBound = sqrt(primeBound);
    pthread_t myThreads[nThreads-1];
    compCount = malloc(nThreads * sizeof(int));
    array = malloc(primeBound * sizeof(int));
    pthread_mutex_init(&lock, NULL);
    sLen = 0;

    for(int i = 0; i < nThreads; ++i)
        compCount[i] = 0;
    for(int i = 0; i < primeBound; ++i)
        array[i] = 1;
    array[0] = array[1] = 0;

    find_sieves();
    printf("Master thread found %d sieves.\n", sLen);

    int id = 0;
    for(int i = 0; i < nThreads-1; ++i) { 
        id = i + 1;
        pthread_create(&myThreads[i], NULL, (void *)worker, (void *)id);
    }
    worker(0);
    for(int i = 0; i < nThreads-1; ++i)
        pthread_join(myThreads[i], NULL);

    printf("\nNumber of primes in range [2, %d]: %d\n", (int)primeBound, printPrime());
    int total = 0;
    for(int i = 0; i < nThreads; ++i) {
        printf("Thread-%d: %d\n", i, compCount[i]);
        total += compCount[i];
    }
    printf("Total number of composites: %d\n", total);

    return 0;
}

void worker(void * vArg) {
    int id = (int)vArg;
    int prime = 0;
    printf("Worker-%d started.\n", id);

    while(1) {
        pthread_mutex_lock(&lock);
        bool primesLeft = false;
        for(int i = 0; i < sLen; ++i) {
            if(sieve[i]) {
                prime = sieve[i];
                sieve[i] = 0;
                primesLeft = true;
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        
        if(!primesLeft) // No sieve primes left
            break;

        printf("Worker-%d working on prime %d.\n", id, prime);
        for(int i = 2*prime; i < primeBound; i += prime) {
            if(array[i]) {
                array[i] = 0;
                ++compCount[id];
            }
        }
    }

    printf("Worker-%d finished.\n", id);
}

void find_sieves() { // Find initial primes with range [2, sqrt(N)]
    int tempArray[(int)searchBound];
    for(int i = 0; i < searchBound; ++i)
        tempArray[i] = 0;

    int flag = 0;
    for(int i = 2; i < searchBound; ++i) {
        flag = 0;
        for(int j = 2; j < i - 1; ++j)
            if( (flag = !(i % j)) )
                break;
        if(!flag)
            tempArray[sLen++] = i;
    }

    sieve = malloc(sLen * sizeof(int));
    for(int i = 0; i < sLen; ++i)
        sieve[i] = tempArray[i];
}

int printPrime() {
    printf("\nPrimes: [");
    int count = 0;
    for(int i = 0, j = 1; i < primeBound; ++i) {
        if(array[i]) {
            ++count;
            j ? (j = 0, printf("%d", i)) : printf(", %d", i); 
        }
    }   
    printf("]\nFinished.\n");
    return count;
}
