/* Из родительского процесса создать дочерний.
Дочерний пишет свой pid, а так же pid  родителя в txt файл, и после чего генерирует 10Мб чего-нибудь.
Родительский ждет все это время, и выводит "Я ждал ..."
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

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
        float t = clock();

        waitpid(pid, &status, 0);
	
	printf ("Waiting time: %f%s", t / CLOCKS_PER_SEC , " seconds\n");
	break;
    }
    return 0;
}
