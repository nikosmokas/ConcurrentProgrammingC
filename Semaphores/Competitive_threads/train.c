#include "mysem.c"

int seats, clients;
int main_sem, passenger_sem, full_sem, train_sem, finish_sem, clear_sem, pass;
volatile int left_seats = 0, passengers = 0, queue = 0, count = 0;



void *train(){

    int i;
    while(passengers > 0){
        printf("waiting to get full\n");
        if(left_seats < seats && passengers > 0 && passengers != seats){
            printf("waiting for more passengers\n");
            sleep(2);
            continue;
        }
        if(passengers == seats){
            mysem_up(train_sem);
        }
        mysem_down(train_sem);
        
        printf("Passengers aboard for a ride !\n");
        sleep(1);
        printf("Ride finished\n");
        left_seats = 0;
        for(i = 0; i < seats; i++){
            mysem_up(finish_sem);
        }
        for(i = 0; i < queue; i++){
            mysem_up(full_sem);
        }
        mysem_down(clear_sem);
    }
    mysem_up(main_sem);
    return NULL;
}

void* passenger(){
    mysem_down(pass);
    while(1){

        if(left_seats < seats){
            left_seats++;
            printf("Passenger: %ld got aboard !\n", pthread_self());
            break;
        }
        else if(left_seats == seats){
            mysem_up(train_sem);
        }
        else{
            
            //printf("Passenger: %ld is waiting for the next ride\n", pthread_self());
            queue++;
            mysem_down(full_sem);
        }
    }
    mysem_up(pass);
    mysem_down(finish_sem);
    printf("Passenger: %ld had a wonderfull ride!\n", pthread_self());
    count++;
    if(count == seats){
        count = 0;
        mysem_up(clear_sem);
    }
    passengers--;
    printf("Passengers left: %d\n", passengers);
    return NULL;
}


int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

    seats = atoi(argv[1]);

    clients = atoi(argv[2]);
    passengers = clients;

    pthread_t threads[clients + 1];
    int i;

    main_sem = mysem_create(IPC_PRIVATE, 0);
    passenger_sem = mysem_create(IPC_PRIVATE, 0);
    full_sem = mysem_create(IPC_PRIVATE, 0);
    train_sem = mysem_create(IPC_PRIVATE, 0);
    finish_sem = mysem_create(IPC_PRIVATE, 0);
    clear_sem = mysem_create(IPC_PRIVATE, 0);
    pass = mysem_create(IPC_PRIVATE, 1);

    if(pthread_create(&threads[clients], NULL, train, NULL) == -1){
        fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    passengers = 0;
    for(i = 0; i < clients; i++){
        passengers++;
        if(i%5 == 0){
            sleep(5);
        }
        if(pthread_create(&threads[clients], NULL, passenger, NULL) == -1){
            fprintf(stderr, "Error in semctl: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    //mysem_up(passenger_sem);

    mysem_down(main_sem);

    printf("HOPE YOU ENJOYED YOUR RIDE\n");

    mysem_destroy(finish_sem);
    mysem_destroy(main_sem);
    mysem_destroy(train_sem);
    mysem_destroy(passenger_sem);
    mysem_destroy(full_sem);

    return 0;
}