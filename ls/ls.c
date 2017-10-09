#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>

int main()
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;

    dir = opendir(".");
    if (dir)
        while ((entry = readdir(dir)) != NULL)
        {
            uid_t uid = getuid();
            struct group *gr = getgrgid(uid);
            struct passwd *pw = getpwuid(uid);
            
            stat(entry->d_name, &st);
            
            char time[40];
            strftime(time, 40, "%b %d %H:%M", localtime(&(st.st_mtime)));
            
            //разрешение файлов
            printf("%s", (S_ISDIR(st.st_mode)) ? "d" : "-");
            printf("%s", (st.st_mode & S_IRUSR) ? "r" : "-");
            printf("%s", (st.st_mode & S_IWUSR) ? "w" : "-");
            printf("%s", (st.st_mode & S_IXUSR) ? "x" : "-");
            printf("%s", (st.st_mode & S_IRGRP) ? "r" : "-");
            printf("%s", (st.st_mode & S_IWGRP) ? "w" : "-");
            printf("%s", (st.st_mode & S_IXGRP) ? "x" : "-");
            printf("%s", (st.st_mode & S_IROTH) ? "r" : "-");
            printf("%s", (st.st_mode & S_IWOTH) ? "w" : "-");
            printf("%s", (st.st_mode & S_IXOTH) ? "x" : "-");
            //количество ссылок
            printf(" %d  ", st.st_nlink);
            //Имя и группа владельца
            printf("%5s  ", pw->pw_name);
            printf("%5s  ", gr->gr_name);
            //размер файла
            printf("%5lld  ", st.st_size);
            //время последней модификации
            printf("%5s  ", time);
            //имя файла
            printf("%s\n", entry->d_name);
        }
        else
        {
            perror("diropen");
            return -1;
        }
    closedir(dir);
    return 0;
}
