/*
Создать область разделяемой памяти. 
Процесс, который создал область должен записывать в эту область строку с текущим временем каждую секунду. 
Если процесс запустился и область разделяемой памяти уже создана, он должен начать читать из нее данные и выводить их на экран.
Доступ к разделяемой памяти синхронизировать с помощью семафора.
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
#include <sys/sem.h>

char *shm;
int shmid, semid;
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
    		semctl(semid, 0, IPC_RMID);
    	}
    	exit(0);    
    }
}


int main(int argc, char *argv[])
{
	signal(SIGINT, signal_);

	char pathname[] = "text";
	key_t key;

	//генерация ключа
	if((key = ftok(pathname,0)) < 0)
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
	
    //сегмент удачно создан
    //отобразить shared memory в адресное пространство текущего проц. 
    if ((shm = shmat(shmid, NULL, 0)) == (char*)(-1))
    {
    	printf("Невозможно присоединиться с shared memory\n");
    	exit(-1);
    }

    struct sembuf sem;
    sem.sem_num = 0;
    sem.sem_flg = 0;

    if (flag == 1)
    {
    	printf("write\n");
		
		if ((semid = semget(key, 1,  IPC_CREAT | IPC_EXCL | 0660)) == -1)
			perror("semget");

		//semctl(semid,semnum,cmd,arg); где четвертый аргумент arg имеет тип union semun
		semctl(semid, 0, SETVAL, 1);

		while (1)
		{
			sem.sem_op = -1;
			if (semop(semid, &sem, 1) == -1)
				perror("semop");

			sleep(1);
    		time_t timer = time(NULL);
    		struct tm* aTm = localtime(&timer);
    	
			sprintf(shm, "%02d:%02d:%02d\n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
			printf("%02d:%02d:%02d\n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec);

			sem.sem_op = 1;
			semop(semid, &sem, 1);

		}

    }
    else
    {
    	printf("read\n");

    	if ((semid = semget(key, 1,  0660)) == -1)
			perror("sem");

		while(1)
		{
			sem.sem_op = -1;
			if (semop(semid, &sem, 1) == -1)
				perror("semop");
			printf("%s\n", shm);
			sem.sem_op = 1;
			semop(semid, &sem, 1);
		}
    }
    return 0;
}