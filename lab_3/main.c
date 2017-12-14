/*  Создать область разделяемой памяти. 
	Процесс, который создал область должен записывать в эту область строку с текущим временем каждую секунду. 
	Если процесс запустился и область разделяемой памяти уже создана, он должен начать читать из нее данные и выводить их на экран.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

char *shm;
short flag = 1;
int shmid;

void signal_(int sig)
{
	if (sig == SIGINT)
	{
		//отсоединение 
 		shmdt(shm);
 		if(flag == 0)
    		shmctl(shmid, IPC_RMID, NULL);

    	exit(0);
    }
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
	
    if (flag == 1)
    {
    	printf("write\n");
    	while(1)
    	{
    		sleep(1);
    		time_t timer = time(NULL);
    		struct tm* aTm = localtime(&timer);
    	
			sprintf(shm, "%02d:%02d:%02d\n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
			printf("%02d:%02d:%02d\n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    	}
    }
    else
    {
    	printf("read\n");
    	while(1)
    	{
    		sleep(1);
    		printf("%s", shm);
    	}
    }

	return 0;
}