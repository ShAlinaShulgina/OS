#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

int main()
{
    pid_t pid;
    FILE *file;
    int status;

    switch (pid = fork())
    {
    case -1:
        perror ("fork"); //ошибка
        return 0;

    case 0:
        if ((file = fopen("test.txt", "w")) == NULL)
        {
            printf("Cannot open file.");
            return 0;
        }

        pid_t cpid = getpid();
        pid_t ppid = getppid();

        fprintf (file, "Child pid: %d\nPid of my parent: %d\n\n", cpid, ppid);

        const int size = 10 * 1024 * 1024;
        char *data = NULL;
        data = (char* ) malloc(size);

        fwrite (data, sizeof(char), size, file);
        free(data);
        fclose(file);
        break;

    default:
	printf("");
        struct timeval start, stop; /* struct timeval {long tv_sec; long tv_usec;};
                                       tv_usec -> микросекунды
                                       tv_sec -> секунды */
        gettimeofday(&start, NULL);

        waitpid(pid, &status, 0);

        gettimeofday(&stop, NULL);
        
	printf("Waiting time:%lu\n", stop.tv_usec - start.tv_usec);
	break;
    }
    return 0;
}
