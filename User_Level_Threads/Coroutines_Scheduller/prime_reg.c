#include "routines_orig.c"
#include <stdio.h>
#include <signal.h>

volatile int numbers_left = 0;

int primetest(int number){
    if(number == 1)
        return 0;
    int i;
    for(i = 2; i <= number / 2; i++){
         
        if(number % i == 0){
            return 0;
        }
    }
    return 1;
}

void worker(void * num){

    int res;
    struct thr *run;
    sigprocmask(SIG_BLOCK, &set, NULL);
    run = main_thread->next;
    while(run->running != 1){
        run = run->next;
    }
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    
    while(1){
        my_threads_sem_down(&run->semid);
        if(work_is_done == 1 && (run->num == 0 || run->num == -2) ){
            break;
        }
        //printf("worker going to sleep\n");
        //sleep(1);
        res = primetest(run->num);
        //kill(0, SIGALRM);
        if(res == 0){
            printf("Thread: %d :Number: %d isn't a prime number!\n", run->id, run->num);
        }
        else
        {
            printf("Thread: %d :Number: %d is a prime number!\n", run->id, run->num);
        }
        printf("Thread: %d is now available!\n", run->id);
        run->avail = 0;
        run->num = 0;
    }
    printf("Thread: %d is exiting\n", run->id);
    run->finish = 1;
    done++;
    remove_thread(run);
    
    my_threads_sem_up(&main_thread->semid);
    run->running = 0;
    my_threads_sem_down(&run->semid);
}

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Wrong number of parameters!\n");
        exit(EXIT_FAILURE);
    }
    setvbuf(stdout, NULL, _IONBF, 0);
    int i, pos, j;
    int numbers[argc - 2];

    numbers_left = argc - 2 ;    
    workers = atoi(argv[1]);

    for(i = 0; i < argc - 2; i++){
        numbers[i] = atoi(argv[i + 2]);
    }

    my_threads_init();

    for(i = 0; i < workers; i++){
        thr_t *thread;
        thread = (thr_t*)malloc(sizeof(thr_t));
        my_threads_create(thread, (void*)&worker, NULL );
    }
    
    j = 0;
    i = 0;
    pos = numbers[j];
    
    //assign work till the work is done
    while(numbers_left > 0){

        //find next number for thread to analyze
        while(numbers[j] == -1){
            if(numbers_left == 0)
                break;
            j++;
            pos = numbers[j];
            if(j == argc - 2)
                j = 0;
        }
        i = 0;
        //assign this number to next available thread
        thr_t *available = main_thread->prev;
        while(1){
            if(numbers_left == 0){
                break;
            }
            if(available->avail == 0){
                sigprocmask(SIG_BLOCK, &set, NULL);
                printf("Main gave Thread: %d number: %d and is unavailable!\n", available->id, pos);
                available->avail = 1;
                available->num = pos;
                //kill(0, SIGALRM);
                numbers[j] = -1;
                numbers_left--;
                sigprocmask(SIG_UNBLOCK, &set, NULL);
                my_threads_sem_up(&available->semid);
                break;
            }
            available = available->prev;
            if(available == main_thread){
                available = main_thread->prev;
                
                my_threads_yield();
            }
        }
    }
    printf("**********************************************************\n");
    printf("****************Waiting to become available****************\n");

    //wait for all workers to become available
    struct thr *done = main_thread->next;
    while(done != main_thread){
        if(done->avail == 1){
            my_threads_yield();
        }
        done = done->next;
    }

    printf("**********************************************************\n");
    printf("****************All workers are available!****************\n");

    //inform the threads that they should finish
    work_is_done = 1;

    //wait for them to finish
    done = main_thread->prev;
    while(done != main_thread){
        my_threads_join(done);
        done = done->prev;
    }
    printf("**********************************************************\n");
    printf("****************All threads have finished!****************\n");
    done = main_thread->destroy_prev;

    while(done != main_thread){
        my_threads_destroy(done);
        done = done->destroy_prev;
    }

    printf("TELOS\n");

    return 0;
}