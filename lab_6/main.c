/* Создать область разделяемой памяти. 
Пишущий в разделяемую память процесс должен запустить несколько (2 < n < 10) отдельных потоков, 
которые получают значение времени и записывают в разделяемую память в виде строки "THREAD_ID - время". 
Дочерний процесс должен запускать несколько потоков для чтения. 
Для синхронизации использовать блокировку чтения-записи
*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
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
		pthread_rwlock_wrlock(&rwlock);
		time_t timer = time(NULL);
    	struct tm* aTm = localtime(&timer);

		sprintf(shm, "THREAD_ID - %d -> %02d:%02d:%02d\n", threadid, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
		printf("-> THREAD_ID - %d -> %02d:%02d:%02d\n", threadid, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
		
		sleep(2);
		pthread_rwlock_unlock(&rwlock);	
	}
	return NULL;
}

void *read_thr(void *p)
{
	while(1)
	{
		sleep(1);
		pthread_rwlock_rdlock(&rwlock);
		sleep(2);
		printf("%s", shm);
		sleep(2);
		pthread_rwlock_unlock(&rwlock);
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
    		flag = 0;
    		if((shmid = shmget(key, 128, 0)) < 0)
    		{
    			printf("Невозможно найти shared memory\n");
    			exit(-1);
    		}
    	}
    }
	
    if ((shm = shmat(shmid, NULL, 0)) == (char*)(-1))
    {
    	printf("Невозможно присоединиться с shared memory\n");
    	exit(-1);
    }

	pthread_t thread[4];
    int st;

    if (flag)
    {
    	printf("write\n");
    	for(int i = 0; i < 4; i++)
    	{
    		st = pthread_create(&(thread[i]), NULL, write_thr, NULL);

    	}
    }
    
    else 
    {
    	printf("read\n");
    	for(int i = 0; i < 4; i++)
    	{
   			st = pthread_create(&(thread[i]), NULL, read_thr, NULL);
    	}
    }
    
	for(int i = 0; i < 4; i++)
    	pthread_join(thread[i], NULL);
	return 0;
}
