/*
 * Kobe Davis
 * Prof. Li
 * CS 415P
 * 13 February 2019
 *
 * Assignment 2: Improved strategy 2 using pthreads
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <poll.h>
#include <stdbool.h>

int nThreads;
double primeBound;
double searchBound;
int * primeArray;
int * sieve;
int * compCount;
int sLen;
bool sieveInProgress;

pthread_mutex_t lock;
pthread_cond_t sieveWait;

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
    sieveInProgress = true;
    searchBound = sqrt(primeBound);
    primeArray = malloc(primeBound * sizeof(int));
    compCount = malloc(nThreads * sizeof(int));
    pthread_t myThreads[nThreads];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&sieveWait, NULL);

    for(int i = 0; i < primeBound; ++i)
        primeArray[i] = 1;
    primeArray[0] = primeArray[1] = 0;
    for(int i = 0; i < nThreads; ++i)
        compCount[i] = 0;

    for(int i = 0; i < nThreads; ++i)
        pthread_create(&myThreads[i], NULL, (void *)worker, (void *)i);
    poll(NULL, NULL, 1000);
    find_sieves();
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
    int id, low, high, total, prime, i;

    i = 0;
    id = (int)vArg;
    while(1) { // Loop does not dictate exit. Sieve length and master thread dictate stopping condition
        pthread_mutex_lock(&lock);
        while(i == sLen && sieveInProgress) // Master thread hasn't found the next sieve prime yet
            pthread_cond_wait(&sieveWait, &lock);
        pthread_mutex_unlock(&lock);

        if(i == sLen && !sieveInProgress) // Thread has worked on all sieve primes and no more are being made
            break;

        prime = sieve[i++];
        nComposite = ceil(primeBound / prime);
        compRange = ceil(nComposite / nThreads);
        interval = prime * compRange;

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
    for(int i = 2; i < searchBound; ++i) {
        if(isPrime(i)) {
            sieve = realloc(sieve, (sLen + 1)*sizeof(int));
            sieve[sLen++] = i;
            printf("Master thread has found sieve prime %d\n", i);
            pthread_cond_broadcast(&sieveWait);
        }
    }
    sieveInProgress = false;
    printf("Master thread has found %d sieve primes\n", sLen);
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
    bool printOn = true; // To turn off annoying prime array

    printf("\nPrimes: [");
    int count = 0;
    for(int i = 0; i < primeBound; ++i) {
        if(primeArray[i]) {
            if(printOn)
                !count ? (++count, printf("%d", i)) : (++count, printf(", %d", i));
            else
                ++count;
        }
    }   
    printf("]\nFinished.\n");
    return count;
}

int min(int l, int r) { return (l < r) ? l : r; }
