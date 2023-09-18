#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Macros
#define NUMThreads 3 // number of threads we need
#define true 1

pthread_mutex_t mutex; // declare mutex

sem_t sem1; // declare semaphore
sem_t sem2; // declare semaphore 
sem_t sem3; // declare semaphore 

// parameters
struct parameters{
    int first;
    int second;
    int value;
};

void *multiply(void *parameters); // declare runner function multiply
void *subtract(void *parameters); // declare runner function subtract
void *print(); // declare runner function print


// runner multiply function
void *multiply(void *params){
    struct parameters *p = (struct parameters *) params;
    while (true) {
        sem_wait(&sem1); // lock sem1 semaphore
        pthread_mutex_lock(&mutex); // acquire mutex lock
        p->first=p->first*5;
        p->second=p->second*5;
        pthread_mutex_unlock(&mutex); // release mutex lock
        printf("multiply returns: %d, %d \n", p->first,p->second);
        sem_post(&sem1); // unlock sem1 semaphore
        sem_post(&sem2); // unlock sem2 semaphore to be used in thread 2
        pthread_exit(0); // exit thread  
    } 
}

// runner print function
void *print(){
    sem_wait(&sem2); // lock sem2 semaphore
    printf("I am the second thread!\n");
    sem_post(&sem2); // unlock sem2 semaphore
    sem_post(&sem3); // unlock sem2 semaphore to be used in thread 3
    pthread_exit(0); // exit thread   
}

// runner subtract function
void *subtract(void *params){
    struct parameters *p = (struct parameters *) params;
    while (true) {
        sem_wait(&sem3); // lock sem3 semaphore
        pthread_mutex_lock(&mutex); // acquire mutex lock
        p->value=p->second-p->first;
        pthread_mutex_unlock(&mutex); // release mutex locks
        printf("subtract returns: %d \n", p->value);
        sem_post(&sem3); // unlock sem3 semaphore
        pthread_exit(0); // exit thread   
    }
}

int main(int argc, char *argv[]){
    // Error handling: ensure number of arguments is 3
    if(argc!=3){
        printf("Invalid number of arguments\n");
        printf("usage: <integer 1, integer 2>\n");
        return -1; // exit the program with error code
    }
    
    // Error handling: ensure input value is a positive integer < 100 and a multiple of 10
    if(atoi(argv[1]) < 0){
        printf("%d must be >= 0\n", atoi(argv[1]));
        return -1;
    }
    if (atoi(argv[1]) > 100) {
        printf("%d must be < 100\n", atoi(argv[1]));
        return -1;
    }
    if (atoi(argv[1]) % 10 != 0) {
        printf("%d must be a multiple of 10\n", atoi(argv[1]));
        return -1;
    }    

    if(atoi(argv[2]) < 0){
        printf("%d must be >= 0\n", atoi(argv[2]));
        return -1;
    }
    if (atoi(argv[2]) > 100) {
        printf("%d must be < 100\n", atoi(argv[2]));
        return -1;
    }
    if (atoi(argv[2]) % 10 != 0) {
        printf("%d must be a multiple of 10\n", atoi(argv[2]));
        return -1;
    } 

    int i; // initialize variable for the loops

    struct parameters *p = malloc(sizeof(struct parameters));
    p->first=atoi(argv[1]);
    p->second=atoi(argv[2]);
    p->value=0;

    pthread_t thread_id[NUMThreads]; // declare threads
    
    // initialize thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // initialize mutex
    if(pthread_mutex_init(&mutex, NULL) !=0){
        printf("Error in initializing mutex\n");
    }

    // initialize semaphore 1 to value 1
    if(sem_init(&sem1, 0, 1) !=0){
        printf("Error in initializing semaphore 1\n");
    }

    // initialize semaphore 2 to value 0 to be unlocked in thread 1
    if(sem_init(&sem2, 0, 0) !=0){
        printf("Error in initializing semaphore 2\n");
    }

    // initialize semaphore 3 to value 0 to be unlocked in thread 2
    if(sem_init(&sem3, 0, 0) !=0){
        printf("Error in initializing semaphore 3\n");
    }

    // create  threads
    if (pthread_create(&thread_id[0], &attr, multiply, p) != 0) {
            printf("Error in Thread %d \n", 1);
    }

    if (pthread_create(&thread_id[1], &attr, print, NULL) != 0) {
            printf("Error in Thread %d \n", 2);
    }

    if (pthread_create(&thread_id[2], &attr, subtract, p) != 0) {
            printf("Error in Thread %d \n", 3);
    }

    // join threads
    for (i = 0; i < NUMThreads; i++) {
        pthread_join(thread_id[i],NULL);
    }

    pthread_mutex_destroy(&mutex); // destroy mutex

    sem_destroy(&sem1); // destroy semaphore

    sem_destroy(&sem2); // destroy semaphore

    sem_destroy(&sem3); // destroy semaphore

    free(p); // free allocated memory

    printf("End of Main!\n"); // print final statement

    return 0; // exit program
}
