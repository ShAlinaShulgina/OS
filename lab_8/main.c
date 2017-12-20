/*
Создать FIFO.
Процесс, который создал FIFO должен записывать в FIFO строку с текущим временем каждую секунду. 
Если процесс запустился и FIFO уже создана, он должен начать читать из него данные и выводить их на экран.
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>

short flag = 1;
int fd, size = 16;
char *name_fifo = "fifo.1";

void signal_(int sig)
{
	if (sig == SIGINT)
	{
		unlink(name_fifo);
		exit(0);
	}

	if (sig == SIGPIPE) //fifo не открыт на чтение ни одинм процессом
	{
		unlink(name_fifo);
		exit(0);
	}
}

int main()
{
	signal(SIGINT, signal_);
	signal(SIGPIPE, signal_);

	struct stat st;
	//(не удалось получить информацию о файле)
	//При успешном заполнении структуры stat возвращается 0. В случае неудачи возвращается —1
	if (stat(name_fifo, &st) != 0)
	{
		flag = 0;
  		if(mkfifo(name_fifo, 0666) < 0)
  		{
  			printf("Ошибка при создании FIFO\n");
			exit(0);
  		}
	}

	int i = 0;
  	if (flag == 1)
	{
		printf("write\n");
		if ((fd = open(name_fifo, O_WRONLY)) < 0)
			exit(0);
		while(1)
		{
			sleep(1);
    		time_t timer = time(NULL);
    		struct tm* aTm = localtime(&timer);
    		char str[size];
	    	sprintf(str, "%02d:%02d:%02d\n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
	    	printf("%s", str);
	    	i = write(fd, str, size);
			printf("Записано %d байт\n", i);
		}
	}
	else
	{
		printf("read\n");
		if ((fd = open(name_fifo, O_RDONLY)) < 0)
			exit(0);
		while(1)
		{
			sleep(1);
			char str[size];
			read(fd, str, size);
			printf("%s\n", str);
		}
	}

	return 0;
}