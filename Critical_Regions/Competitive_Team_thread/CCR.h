#define CCR_DECLARE(label) \
pthread_mutex_t label ## mtx;\
pthread_cond_t label ## c;\

#define CCR_INIT(label) \
pthread_mutex_init(&label ## mtx ,NULL);\
pthread_cond_init( &label ## c,NULL);\


#define CCR_EXEC(label,cond,body)\
	pthread_mutex_lock(&label ## mtx);\
		while(!(cond)) {\
			pthread_cond_wait(&label ## c,&label ## mtx);\
			pthread_cond_signal(&label ## c);\
		}\
	\
	body\
		pthread_cond_signal(&label ## c);\
		pthread_mutex_unlock(&label ## mtx);\
	
	
		
	
		
