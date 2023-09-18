#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

// Defined macros to be used in the program
#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phil_num + 4) % N
#define RIGHT (phil_num + 1) % N

// Initialized state and set of philosophers
int state[N];
int phil[N] = {0, 1, 2, 3, 4};

// Creating mutex
sem_t mutex;
sem_t S[N];


void run_Phil(int phil_num) {
    // If philosopher is hungry and the left and right philosophers are not eating
    if (state[LEFT] != EATING && state[RIGHT] != EATING && state[phil_num] == HUNGRY) {
        // Philosopher's state changed to eating
        state[phil_num] = EATING;
        sleep(2);
        // Print philosopher takes fork
        printf("Philosopher %d takes fork %d & %d\n", phil_num + 1, LEFT + 1, phil_num + 1);
        sem_post(&S[phil_num]);
    }
}

void pickup_forks(int phil_num) {
    sem_wait(&mutex);
    // Philosopher's state changed to hungry
    state[phil_num] = HUNGRY;
    // Print the philosopher who is taking the fork
    printf("Philosopher %d's hungry\n", phil_num + 1);
    run_Phil(phil_num);
    sem_post(&mutex);
    sem_wait(&S[phil_num]);
    sleep(2);
}

void return_forks(int phil_num) {
    sem_wait(&mutex);
    // Philosopher's state is changed to thinking
    state[phil_num] = THINKING;
    printf("Philosopher %d's putting fork %d & %d down\n", phil_num + 1, LEFT + 1, phil_num + 1);
    run_Phil(LEFT);
    run_Phil(RIGHT);
    sem_post(&mutex);
}

void *philospher(void *num) {
    // Counter that controls the number of times the philosophers eat
    // Initialize the counter with 0
    int count = 0;
    // While the counter is less than the limit, the philosophers can eat
    while (count != 5) {
        int *i = num;
        sleep(1);
        pickup_forks(*i);
        sleep(0);
        return_forks(*i);
        // Increment the counter
        count++;
    }
}

int main() {
    int i;
    pthread_t thread_id[N];
    sem_init(&mutex, 0, 1);
    
    // Initializing N amount of semaphores
    for (i = 0; i < N; i++) {
        sem_init(&S[i], 0, 0);
    }

    // Creating multiple threads
    for (i = 0; i < N; i++) {
        pthread_create(&thread_id[i], NULL, philospher, &phil[i]);
        printf("Philosopher %d's thinking\n", i + 1);
    }

    // Joining the threads together
    for (i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL);
    }
}