/*  Изменить лаб3 следующим образом: пишущий в разделяемую память процесс должен запустить несколько (2 < n < 10) отдельных потоков, 
	которые получают значение времени и записывают в разделяемую память в виде строки "THREAD_ID - время". 
	Дочерний процесс должен запускать несколько потоков для чтения. 
	Доступ к разделяемой памяти должен быть синхронизирован в рамках каждого процесса используя мьютекс;
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

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char *shm;
int shmid;
short flag = 1;

void signal_(int sig)
{
    if (sig == SIGINT)
    {
        //отсоединение 
        shmdt(shm);
        if (flag == 1)
        {
            int r;
            if ((r = shmctl(shmid, IPC_RMID, NULL)) < 0)
               printf("error\n");
        }
    exit(0);    
    }
}

void *write_thr(void *p)
{
	pthread_t id =  pthread_self();
    int threadid = *((int *) id);
	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&mutex);
		time_t timer = time(NULL);
    	struct tm* aTm = localtime(&timer);

		sprintf(shm, "THREAD_ID - %d -> %02d:%02d:%02d\n", threadid, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
		printf("-> THREAD_ID - %d -> %02d:%02d:%02d\n", threadid, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
		sleep(2);
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
		
		printf("%s", shm);
		sleep(2);
		pthread_mutex_unlock(&mutex);
		
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, signal_);
	
	char pathname[] = "text";
	key_t key;

	if((key = ftok(pathname, 1)) < 0)
	{
        printf("Ключ не сгенерирован\n");
        exit(-1);
    }
    printf("Ключ сгенерирован\n");

    //создание сегмента разделяемой памяти, 128 байт, 0666 - чтение и запись разрешены для всех
    if((shmid = shmget(key, 128, 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
    	//EEXIST если значение IPC_CREAT | IPC_EXCL было указано, а сегмент уже существует.
    	if(errno == EEXIST)
    	{
    		printf("Сегмент существует\n");
    		if((shmid = shmget(key, 128, 0)) < 0)
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

	pthread_t thread[6];
    int st;
    int id[6];

    if (flag)
    {
    	printf("write\n");
    	for(int i = 0; i < 6; i++)
    		st = pthread_create(&(thread[i]), NULL, write_thr, NULL);
   		
    }
    else
    {
    	printf("read\n");
    	for(int i = 0; i < 6; i++)
    		st = pthread_create(&(thread[i]), NULL, read_thr, NULL);
    }
    
	for(int i = 0; i < 6; i++)
    	pthread_join(thread[i], NULL);
	return 0;
}
