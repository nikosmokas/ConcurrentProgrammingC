#include <ucontext.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#define co_t ucontext_t
#define MEM 16384
#define error(msg) perror(msg);\
                    exit(EXIT_FAILURE);

int my_coroutines_init(co_t *main){
    
    //printf("here init\n");
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

    //printf("before swapcontext\n");
    //swapcontext(cur, next);
    setcontext(next);
    //printf("after swapcontext\n");
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