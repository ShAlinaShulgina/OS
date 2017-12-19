/*
Создать область разделяемой памяти.
В рамках одного процесса запустить один поток, который должен писать в shmem блоки данных по 16 КБ, и несколько потоков читающих из shmem. 
Синхронизировать обмен данными с помощью условных переменных.
*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_bl = PTHREAD_COND_INITIALIZER; 

char *shm;
int shmid, size = 16 * 1024;
short flag = 1, full = 0;
char *data;

void signal_(int sig)
{
	if (sig == SIGINT)
	{
		//отсоединение 
 		shmdt(shm);
        int r;
        if ((r = shmctl(shmid, IPC_RMID, NULL)) < 0)
           printf("error\n");
    	exit(0);    
    }
}

void *write_thr(void *p)
{
	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&mutex);
		for(int i = 0; i < size - 1; i++)
		{
			shm[i] = 'a';
		}
		shm[size - 1] = '!';

		full = 0;
		sleep(2);
		pthread_cond_signal(&cond_bl);
		printf("\n\n\ngood write\n\n\n");
		pthread_mutex_unlock(&mutex);	
	}
	return NULL;
}

void *read_thr(void *p)
{
	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&mutex);
		while(full == 1)
			pthread_cond_wait(&cond_bl, &mutex); //освобождает мьютекс и блокирует нить до момента вызова другой нитью pthread_cond_signal.
		
		printf("\n%s\n", shm);
		full = 1;
		sleep(1);
		pthread_mutex_unlock(&mutex);
		
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, signal_);

	char pathname[] = "text";
	key_t key;

	if((key = ftok(pathname,0)) < 0)
	{
        printf("Ключ не сгенерирован\n");
        exit(-1);
    }
    printf("Ключ сгенерирован\n");

    //создание сегмента разделяемой памяти, 16 Kбайт, 0666 - чтение и запись разрешены для всех
    if((shmid = shmget(key, size + 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
    	//EEXIST если значение IPC_CREAT | IPC_EXCL было указано, а сегмент уже существует.
    	if(errno == EEXIST)
    	{
    		printf("Сегмент существует\n");
    		if((shmid = shmget(key, size + 1, 0)) < 0)
    		{
    			printf("Невозможно найти shared memory\n");
    			exit(-1);
    		}
    		flag = 0;
    	}
    }
	
    if ((shm = shmat(shmid, NULL, 0)) == (char*)(-1))
    {
    	printf("Невозможно присоединиться с shared memory\n");
    	exit(-1);
    }

	pthread_t thread[4];
	pthread_t thr;
    int st;

    printf("read\n");
    for(int i = 0; i < 4; i++)
    {
    	st = pthread_create(&(thread[i]), NULL, read_thr, NULL);
    }
	
    printf("write\n");
    st = pthread_create(&thr, NULL, write_thr, NULL);
 	pthread_join(thr, NULL);

	for(int i = 0; i < 4; i++)
    	pthread_join(thread[i], NULL);

    pthread_cond_destroy(&cond_bl);
	return 0;
}