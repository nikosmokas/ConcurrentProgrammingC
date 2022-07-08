#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#define co_t ucontext_t
#define MEM 16384
#define error(msg) perror(msg);\
                    exit(EXIT_FAILURE);

volatile int ids = 1, work_is_done = 0;
volatile int done = 0;
int workers;
sigset_t set;
struct itimerval time = {{0}};
struct sigaction act_alarm = {{0}};
struct sigaction act_user = {{0}};

struct semaphore
{
    int val;
};
typedef struct semaphore sem_t;

struct thr
{
    int running;
    int num;
    int id;
    int avail;
    int finish;
    sem_t semid;
    co_t *rout;
    struct thr *prev;
    struct thr *next;
    struct thr *destroy_prev;
    struct thr *destroy_next;
};
typedef struct thr thr_t;
thr_t  *main_thread;

int my_threads_sem_init(sem_t *s, int val);
int my_threads_sem_down(sem_t *s);
int my_threads_sem_up(sem_t *s);
int my_threads_sem_destroy(sem_t *s);

int my_coroutines_init(co_t *main){
    
    if(getcontext(main) == -1){
        error("getcontext");
    }

    return 0;
}

int my_coroutines_create(co_t *context, void (body)(void *) ,void *arg){

    int res;
    res = getcontext(context);
    if(res == -1){
        error("getcontext");
    }
    context->uc_link = 0;
    context->uc_stack.ss_sp = malloc(MEM);
    context->uc_stack.ss_size = MEM;
    
    makecontext(context, (void*)body, 1, arg);
    return 0;
}


int my_coroutines_switchto(co_t *next){
    
    setcontext(next);
    return 0;
}

int my_coroutines_destroy(co_t *routine){
    int res;

    res = getcontext(routine);
    if(res == -1){ 
        error("getcontext");
    }
    routine->uc_link = 0;
    routine->uc_stack.ss_size = 0;
    free(routine->uc_stack.ss_sp);
    return 0;
}

void user_handler(int sig){
    sigprocmask(SIG_BLOCK,&set,NULL);
    if(main_thread->running == 1){
        printf("BLOCKED!\n");
        main_thread->running = 0;
        main_thread->prev->running = 1;
        printf("Thread running : %d!\n", main_thread->prev->id);
        sigprocmask(SIG_UNBLOCK,&set,NULL);
        if(setitimer(ITIMER_REAL, &time, NULL) == -1){
            error("setitimer");
        }
        
        swapcontext(main_thread->rout, main_thread->prev->rout);
        //my_coroutines_switchto(&main_thread->prev->rout);
    }
    else{
        if(work_is_done == 1){
            main_thread->running = 1;
            printf("Main running !\n");
            sigprocmask(SIG_UNBLOCK,&set,NULL);
            if(setitimer(ITIMER_REAL, &time, NULL) == -1){
                error("setitimer");
            }
            
            my_coroutines_switchto(main_thread->rout);
        }
        printf("BLOCKED!\n");
        thr_t *run = main_thread->prev;
        while(run->running != 1){
            run = run->next;
        }
        run->running = 0;
        run = run->prev;
        if(run == main_thread){
            run = main_thread->next;
            main_thread->running = 1;
            printf("Main running !\n");
            sigprocmask(SIG_UNBLOCK,&set,NULL);
            if(setitimer(ITIMER_REAL, &time, NULL) == -1){
                error("setitimer");
            }
            
            swapcontext(run->rout, main_thread->rout);
            //my_coroutines_switchto(&main_thread->rout);
            
            return ;
        }
        else{
            
            run->running = 1;
            printf("Thread running : %d!\n", run->id);
            sigprocmask(SIG_UNBLOCK,&set,NULL);
            if(setitimer(ITIMER_REAL, &time, NULL) == -1){
                error("setitimer");
            }
            
            swapcontext(run->next->rout, run->rout);
            //my_coroutines_switchto(&run->rout);
        }
    }
}

void alarm_handler(int sig){
    sigprocmask(SIG_BLOCK, &set,NULL);
    if(main_thread->running == 1){
        printf("BEEP!\n");
        main_thread->running = 0;
        main_thread->prev->running = 1;
        printf("Thread running : %d!\n", main_thread->prev->id);
        sigprocmask(SIG_UNBLOCK,&set,NULL);
        if(setitimer(ITIMER_REAL, &time, NULL) == -1){
            error("setitimer");
        }
        
        swapcontext(main_thread->rout, main_thread->prev->rout);
        //my_coroutines_switchto(&main_thread->prev->rout);
    }
    else{
        thr_t *run = main_thread->prev;
        while(run->running != 1){
            run = run->next;
        }
        
        printf("BEEP!\n");
        run->running = 0;
        run = run->prev;
        if(run == main_thread){
            run = main_thread->next;
            main_thread->running = 1;
            printf("Main running !\n");
            sigprocmask(SIG_UNBLOCK,&set,NULL);
            if(setitimer(ITIMER_REAL, &time, NULL) == -1){
                error("setitimer");
            }
            
            swapcontext(run->rout, main_thread->rout);
            //my_coroutines_switchto(&main_thread->rout);
            
            return ;
        }
        else{
            run->running = 1;
            printf("Thread running : %d!\n", run->id);
            sigprocmask(SIG_UNBLOCK,&set,NULL);
            if(setitimer(ITIMER_REAL, &time, NULL) == -1){
                error("setitimer");
            }
            
            swapcontext(run->next->rout, run->rout);
            //my_coroutines_switchto(&run->rout);
        }
    }
}

int my_threads_init(){
    int res;
    
    main_thread = (thr_t*)malloc(sizeof(thr_t));
    main_thread->rout = (co_t*)malloc(sizeof(co_t));
    main_thread->running = 1;
    main_thread->id = -1;
    main_thread->next = main_thread;
    main_thread->prev = main_thread;

    main_thread->destroy_next = main_thread;
    main_thread->destroy_prev = main_thread;

    my_coroutines_init(main_thread->rout);
    my_threads_sem_init(&main_thread->semid, 0);
    printf("Main running !\n");

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGUSR1);
    act_alarm.sa_handler = &alarm_handler;
    act_alarm.sa_flags = SA_RESTART;
    act_user.sa_handler =  &user_handler;
    act_user.sa_flags = SA_RESTART;

    res = sigaction(SIGALRM, &act_alarm, NULL);
    if(res == -1){
        error("sigaction");
    }
    res = sigaction(SIGUSR1, &act_user, NULL);
    if(res == -1){
        error("sigaction");
    }
    time.it_value.tv_sec = 0;
    time.it_value.tv_usec = 10000;
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_usec = 10000;
    res = setitimer(ITIMER_REAL, &time, NULL);
    if(res == -1){
        error("setitimer");
    }
    return 0;
}

int my_threads_create(thr_t *thr, void (body)(void *), void* arg){
    
    thr->num = -2;
    thr->id = ids;
    ids++;
    thr->avail = 0;
    thr->running = 0;
    thr->finish = 0;
    thr->next = main_thread->next;
    thr->prev = main_thread;
    thr->next->prev = thr;
    thr->prev->next = thr;
    
    thr->destroy_next = main_thread->destroy_next;
    thr->destroy_prev = main_thread;
    
    thr->destroy_prev->destroy_next = thr;
    thr->destroy_next->destroy_prev = thr;
    
    thr->rout = (co_t*)malloc(sizeof(co_t));
    my_threads_sem_init(&thr->semid, 0);

    my_coroutines_create(thr->rout, body, arg);
    thr->rout->uc_link = main_thread->rout;

    return 0;
}

int my_threads_yield(){
    int res;
    res = kill(0, SIGUSR1);
    if(res == -1){
        error("kill");
    }
    return 0;
}

int my_threads_join(thr_t *thr){

    while(thr->finish == 0){
        my_threads_sem_up(&thr->semid);
        my_threads_sem_down(&main_thread->semid);
    }
    return 0;
}

int my_threads_destroy(thr_t *thr){
    
    my_coroutines_destroy(thr->rout);
    my_threads_sem_destroy(&thr->semid);
    free(thr);

    return 0;
}

int my_threads_sem_init(sem_t *s, int val){
    s->val = val;
    return 0;
}

int my_threads_sem_down(sem_t *s){
    
    if(s->val >= 0){
        s->val--;
    }
    while(s->val < 0){
        my_threads_yield();
    }
    return 0;
}

int my_threads_sem_up(sem_t *s){
    s->val++;
    return 0;
}

int my_threads_sem_destroy(sem_t *s){
    
    return 0;
}

void remove_thread(thr_t *thr){
    sigprocmask(SIG_BLOCK,&set,NULL);
    thr->prev->next = thr->next;
    thr->next->prev = thr->prev;
    sigprocmask(SIG_UNBLOCK,&set,NULL);
}
