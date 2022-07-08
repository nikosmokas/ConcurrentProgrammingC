#include "mysem.c"

#define blue_turn 0
#define red_turn 1
#define false 0
#define true 1

volatile int blue, red, slots;
volatile int blue_left = 0, red_left = 0;
volatile int slots_left = 0;
volatile int stop = 0;
volatile int count = 0;
volatile int turn = -1;
int blue_sem, red_sem, bridgeB_sem, bridgeR_sem, general_sem, wait_sem, main_sem;
volatile int step = 0, step2 = 0, stepR = 0;

void * thread_red(){
    int i;
    long int id;
    //sleep(1);
    srand(time(NULL));
    id = pthread_self();
    //printf("Red Car: %ld has created\n", id);
    mysem_down(red_sem);
    //printf("here1\n");
    if(step == 0){
        step++;
        printf("red decided\n");
        turn = (rand()%2);
        if(turn != red_turn){
            turn = blue_turn;
            mysem_up(wait_sem);
            mysem_down(general_sem);
        }
    }
    //printf("turn: %d\n", turn);
    if(turn == -2){
        mysem_down(wait_sem);
    }
    if(turn != red_turn){
        mysem_down(general_sem);
    }
    while(1){
        if(turn != red_turn){
            //printf("lathos\n");
            mysem_down(general_sem);
            //mysem_down(general_sem);
            //mysem_down(red_sem);
        }
        if(slots_left == 0 ){
            slots_left++;
            printf("Red cars passing !\n");
            if(red_left == 1){
                printf("The only Red car left %ld has passed\n", id);
                red_left--;
                break;
            }
            printf("The first Red car %ld has arrived at the bridge\n", id);
            mysem_up(red_sem);
            mysem_down(bridgeR_sem);

            printf("Red car %ld has passed\n", id);
            red_left--;
            count++;
            break;
            
        }
        else if(slots_left != slots){
            slots_left++;
            if(red_left - slots < 0){
                if(slots_left == red_left){
                    for(i = 0; i < slots_left - 1; i++){
                        mysem_up(bridgeR_sem);
                        
                    }
                    printf("Red car %ld has passed\n", id);
                    red_left--;
                    break;
                }
                else{
                    printf("Red car: %ld has arrived at the bridge\n", id);
                    mysem_up(red_sem);
                    mysem_down(bridgeR_sem);
                    printf("Red car %ld has passed\n", id);
                    red_left--;
                    count++;
                    break;
                }
            }
            else{            
                if(slots_left == slots){
                    printf("Red car %ld has reached bridge last\n", id);
                    for(i = 0; i < slots - 1; i++){
                        mysem_up(bridgeR_sem);
                    }
                    printf("Red car %ld has passed\n", id);
                    red_left--;
                    count++;
                    break;
                }
                else{
                    printf("Red car: %ld has arrived at the bridge\n", id);
                    mysem_up(red_sem);

                    mysem_down(bridgeR_sem);

                    printf("Red car %ld has passed\n", id);
                    red_left--;
                    count++;
                    break;
                }
            }
        }
        else if(slots_left == slots){
            mysem_down(wait_sem);

            continue;
        }
    }
    //printf("here3\n");

    if(count == slots || red_left == 0){
        if(count == slots){
            printf("Bridge got empty because it was full !\n");
        }
        if(red_left == 0){
            printf("Bridge got empty because cars are over !\n");
        }
        //printf("Red cars left: %d\n", red_left);
        slots_left = 0;
        count = 0;
        turn = (rand()%2);
        //printf("1turn = %d\n", turn);
        if(turn == red_turn){
            if(red_left == 0){
                turn = blue_turn;
                mysem_up(general_sem);
                //mysem_up(blue_sem);
            }
            else{
                //mysem_up(general_sem);
                mysem_up(red_sem);
            }
        }
        if(turn == blue_turn){
            if(blue_left == 0){
                turn = red_turn;
                mysem_up(red_sem);
                //mysem_up(red_sem);
            }
            else{
                if(step2 == 0){
                    step2++;
                    mysem_up(general_sem);
                }
                else{
                    turn = blue_turn;
                    mysem_up(general_sem);
                    mysem_up(blue_sem);
                }
            }    
        }
        //printf("2turn = %d\n", turn);
    }
    stop++;
    if(stop == red + blue){
        mysem_up(main_sem);
    }
    
    return NULL;
}
void * thread_blue(){
    int i;
    long int id;
    srand(time(NULL));
    //sleep(1);
    id = pthread_self();
    //printf("Blue Car: %ld has created\n", id);
    //printf("here2\n");
    mysem_down(blue_sem);
    //printf("edo\n");
    if(step == 0){
        step++;
        printf("blue decided\n");
        turn = (rand()%2);
        if(turn != blue_sem){
            turn = red_turn;
            mysem_up(wait_sem);
            mysem_down(general_sem);
        }
    }
    //printf("turn: %d\n", turn);
    if(turn == -2){
        mysem_down(wait_sem);
    }
    if(turn != blue_turn){
        mysem_down(general_sem);
    }
    //printf("here2\n");
    while(1){
        if(turn != blue_turn){
            //printf("lathos\n");
            mysem_down(general_sem);
        }
        if(slots_left == 0){
            slots_left++;
            printf("Blue cars passing !\n");
            if(blue_left == 1){
                printf("The only left  Blue car %ld has passed\n", id);
                blue_left--;
                break;
            }
            printf("The first Blue car %ld has arrived at the bridge\n", id);
            mysem_up(blue_sem);
            mysem_down(bridgeB_sem);
              
            printf("Blue car %ld has passed\n", id);
            blue_left--;
            count++;
            break;
            
        }
        else if(slots_left != slots){
            slots_left++;
            if(blue_left - slots < 0){
                if(blue_left == slots_left){
                    for(i = 0; i < slots_left - 1; i++){
                        mysem_up(bridgeB_sem);
                    }
                    printf("Blue car %ld has passed\n", id);
                    blue_left--;
                    break;
                }
                else{
                    printf("Blue car: %ld has arrived at the bridge\n", id);
                    mysem_up(blue_sem);
                    mysem_down(bridgeB_sem);
                    printf("Blue car %ld has passed\n", id);
                    blue_left--;
                    count++;
                    break;

                }
            }
            else{
                if(slots_left == slots){
                    printf("Blue car %ld has reached bridge last\n", id);
                    for(i = 0; i < slots - 1; i++){
                        mysem_up(bridgeB_sem);
                    }
                    printf("Blue car %ld has passed\n", id);
                    blue_left--;
                    count++;
                    break;
                }
                else{
                    printf("Blue car: %ld has arrived at the bridge\n", id);
                    mysem_up(blue_sem);
                    mysem_down(bridgeB_sem);
                       
                    printf("Blue car %ld has passed\n", id);
                    blue_left--;
                    count++;
                    break;
                }
            }
        }
        else if(slots_left == slots){
            //printf("Ayto to lathos\n");
            mysem_down(wait_sem);

            continue;
        }
    }
    //printf("here1\n");
    if(count == slots || blue_left == 0){
        if(count == slots){
            printf("Bridge got empty because it was full !\n");
        }
        if(blue_left == 0){
            printf("Bridge got empty because cars are over !\n");
        }
        //printf("Blue cars left: %d\n", blue_left);
        slots_left = 0;
        count = 0;
        turn = (rand()%2);
        //printf("1turn = %d\n", turn);
        if(turn == blue_turn){
            if(blue_left == 0){
                turn = red_turn;
                mysem_up(general_sem);
                //mysem_up(red_sem);
            }
            else{
                //mysem_up(general_sem);
                mysem_up(blue_sem);
            }
        }
        if(turn == red_turn){
            if(red_left == 0){
                turn = blue_turn;
                //mysem_up(general_sem);
                mysem_up(blue_sem);
            }
            else{
                if(step2 == 0){
                    step2++;
                    mysem_up(general_sem);
                }
                else{
                    turn = red_turn;
                    mysem_up(general_sem);
                    mysem_up(red_sem);
                }
            }
        }
        //printf("2turn = %d\n", turn);
        
    }
    
    stop++;
    if(stop == red + blue){
        mysem_up(main_sem);
    }
    return NULL;
}

int main(int argc, char * argv[]){

    if(argc != 4){
        printf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

    int i;

    slots = atoi(argv[1]);
    red = atoi(argv[2]);
    blue = atoi(argv[3]);

    printf("red: %d \nblue: %d\nslots: %d\n", red, blue, slots);

    pthread_t cars_r[red];
    pthread_t cars_b[blue];

    blue_sem = mysem_create(IPC_PRIVATE, 1);
    red_sem = mysem_create(IPC_PRIVATE, 1);
    bridgeB_sem = mysem_create(IPC_PRIVATE, 0);
    bridgeR_sem = mysem_create(IPC_PRIVATE, 0);
    general_sem = mysem_create(IPC_PRIVATE, 0);
    wait_sem = mysem_create(IPC_PRIVATE, 0);
    main_sem = mysem_create(IPC_PRIVATE, 0);


    for(i = 0; i < blue ; i++){
        blue_left++;
        if(pthread_create(&cars_b[i], NULL, thread_blue, NULL) == -1 ){
            fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < red; i++){
        red_left++;
        if(pthread_create(&cars_r[i], NULL, thread_red, NULL) == -1 ){
            fprintf(stderr, "Error in pthread_create: %s !\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    //printf("red cars have been created\n");
    

    //printf("blue cars have been created\n");
    mysem_down(main_sem);
    
    mysem_destroy(main_sem);
    mysem_destroy(blue_sem);
    mysem_destroy(red_sem);
    mysem_destroy(bridgeB_sem);
    mysem_destroy(bridgeR_sem);

    return 0;
}