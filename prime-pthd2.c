/*
 * Kobe Davis
 * Prof. Li
 * CS 415P
 * 13 February 2019
 *
 * Assignment 2: Strategy 2 using pthreads
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

int nThreads;
double primeBound;
double searchBound;
int * primeArray;
int * sieve;
int sLen;
int * compCount;

void worker(void *);
void find_sieves();
int printPrime();
int isPrime(int);
int min(int, int);

int main() {
    printf("Enter number of threads: ");
    scanf("%d", &nThreads);
    printf("Enter upper bound: ");
    scanf("%lf", &primeBound);

    sLen = 0;
    searchBound = sqrt(primeBound);
    primeArray = malloc(primeBound * sizeof(int));
    compCount = malloc(nThreads * sizeof(int));
    pthread_t myThreads[nThreads];

    for(int i = 0; i < primeBound; ++i)
        primeArray[i] = 1;
    primeArray[0] = primeArray[1] = 0;
    for(int i = 0; i < nThreads; ++i)
        compCount[i] = 0;

    find_sieves();
    printf("Master has found %d sieve primes\n", sLen);

    for(int i = 0; i < nThreads; ++i)
        pthread_create(&myThreads[i], NULL, (void *)worker, (void *)i);
    for(int i = 0; i < nThreads; ++i)
        pthread_join(myThreads[i], NULL);

    int total = 0;
    for(int i = 0; i < nThreads; ++i)
        total += compCount[i];
    printf("Number of primes in range [2, %d]: %d\n", (int)primeBound, printPrime());
    printf("Total number of composites: %d\n", total);

    return 0;
}

void worker(void * vArg) {
    double nComposite, compRange, interval;
    int id, low, high, total, prime;

    id = (int)vArg;
    for(int i = 0; i < sLen; ++i) {
        prime = sieve[i];
        nComposite = ceil(primeBound / prime);
        compRange = ceil(nComposite / nThreads);
        interval = prime * compRange;

        // The lower and upper bounds of the range are computed dynamically
        // based on the number of sieve prime composites within the Nth interval
        // for the Nth thread.
        // This is done so that the lower and upper bound will always be a multiple
        // of the current sieve prime, and the following loop will not iterate 
        // redundantly.

        low = (!id) ? interval*id + 2*prime : interval*id;
        high = min(low + interval, primeBound);

        if(!i)
            printf("Worker-%d has started on range [%d - %d]\n", id, low, high);

        printf("Worker-%d working on prime %d (1st composite: %d)\n", id, prime, low);
        for(int j = low; j < high; j += prime) {
            if(primeArray[j]) {
                primeArray[j] = 0;
                ++total;
            }
        }
    }

    compCount[id] = total;
    printf("Worker-%d finished\n", id);
}

void find_sieves() {
    int temp[(int)searchBound];
    for(int i = 2; i < searchBound; ++i)
        if(isPrime(i))
            temp[sLen++] = i;

    sieve = malloc(sLen * sizeof(int));
    for(int i = 0; i < sLen; ++i) {
        sieve[i] = temp[i];
    }
}

int isPrime(int toCheck) {
    if(toCheck < 2 || !(toCheck & 1))
        return toCheck == 2;
    for(int i = 3; i <= toCheck/i; i += 2)
        if(!(toCheck%i))
            return 0;
    return 1;
}

int printPrime() {
    printf("\nPrimes: [");
    int count = 0;
    for(int i = 0; i < primeBound; ++i) {
        if(primeArray[i])
            !count ? (++count, printf("%d", i)) : (++count, printf(", %d", i));
    }   
    printf("]\nFinished.\n");
    return count;
}

int min(int l, int r) { return (l < r) ? l : r; }
