#include "mysem.c"

#define THREAD 
 
volatile int threads_done = 0;
volatile int work_done = 0;
volatile int numbers_left;
volatile int threads_num, sum, finish = 0;
int main_semid, avail_sem, finish_sem;
int thread_sem;

struct workers
{
    int num;
    int semid;
    long int id;
    int finish;
    int avail;
};
struct workers *work;

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

void *worker(){

    while(threads_done == 0);
    int i, res;
    long int id;
    id = pthread_self(); 

    for(i = 0; i < threads_num; i++){
        if(id == work[i].id)
            break;
    }
    while(1){

        if (work_done == 1){
            break;
        }
        
        if(work[i].num == -2){
            continue;
        }
        res = primetest(work[i].num);
        if (res == 0 ){
                printf("Thread: %ld :Number: %d isn't a prime number!\n", work[i].id, work[i].num);
        }
        else{
            printf("Thread: %ld Number: %d is a prime number!\n", work[i].id, work[i].num);
        }
        work[i].num = -2;
        printf("Thread: %ld is now available !\n", work[i].id);
        printf("*********************\n");
        work[i].avail = 0;
        res = mysem_up(thread_sem);
        if(res == -1){
            exit(EXIT_FAILURE);
        }
        if (work_done == 1){
            break;
        }
        if(numbers_left == 0){
            mysem_up(avail_sem);
        }
    }
    printf("Thread %ld has finished !\n", id);
    work[i].finish = 1;
    finish++;
    if(finish == threads_num){
        mysem_up(thread_sem);
    }
    
    return NULL;
}

int main(int argc, char *argv[]){


    int i, pos, j = 0;
    threads_num = atoi(argv[1]);
    numbers_left = argc - 2;
    sum = argc - 2;
    int numbers[argc - 2];
    int res;

    work = (struct workers *) malloc(threads_num * sizeof(struct workers));
    pthread_t threads[threads_num];
    for(i = 0; i < argc - 2; i++){
        numbers[i] = atoi(argv[i+2]);   
    }
    main_semid = mysem_create(IPC_PRIVATE, 1);
    avail_sem = mysem_create(IPC_CREAT, 0);
    for(i = 0; i < threads_num; i++){

        if(pthread_create(&threads[i], NULL, worker, NULL) != 0){
            fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        work[i].id = threads[i];
        work[i].num = -2;
        work[i].finish = 0;
        work[i].avail = 1;
        printf("Thread: %ld has been created\n", work[i].id);
    }
    thread_sem = mysem_create(IPC_PRIVATE, 1);
    if(thread_sem == -1){
        exit(EXIT_FAILURE);
    }
    printf("*********************\nThreads have been created\n*********************\n");

    threads_done = 1;
    i = 0;
    pos = numbers[j];
    while (numbers_left > 0){
        
        if(numbers_left == 0)
            break;
        res = mysem_down(thread_sem);
        if(res ==  0){
            j = 0;
            while(numbers[j] == -1){
                if(numbers_left == 0)
                    break;
                j++;
                pos = numbers[j];
                if(j == sum)
                    j = 0;
            }
            printf("Thread: %ld has taken this number : %d and is unavailable!\n", work[i].id, pos);
            work[i].avail = 1;
            work[i].num = pos;
            numbers[j] = -1;
            numbers_left--;                
        }
        else if(res == -1){
            exit(EXIT_FAILURE);
        }
        i++;
        if(i == threads_num){
            i = 0;
        }
    }
    /*for(i = 0; i < threads_num; i++){
        while(work[i].avail == 1){
            pthread_yield();
        }
    }*/
    printf("waiting for everyone to become available\n");
    mysem_down(thread_sem);
    printf("everyone is available waiting for them to finish\n");
    work_done++;
    /*for(i = 0; i < threads_num; i++){
        while(work[i].finish == 0){
            pthread_yield();
        }
    }*/
    mysem_down(thread_sem);

    mysem_destroy(main_semid);
    mysem_destroy(thread_sem);
    printf("DONE\n");
    free(work);
    return 0;
}