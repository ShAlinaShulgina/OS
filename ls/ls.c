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
            printf("%s%s%s%s%s%s%s%s%s%s %d %5s %5s %5lld %5s %s\n", (S_ISDIR(st.st_mode)) ? "d" : "-",
                    (st.st_mode & S_IRUSR) ? "r" : "-",
                    (st.st_mode & S_IWUSR) ? "w" : "-",
                    (st.st_mode & S_IXUSR) ? "x" : "-",
                    (st.st_mode & S_IRGRP) ? "r" : "-",
                    (st.st_mode & S_IWGRP) ? "w" : "-",
                    (st.st_mode & S_IXGRP) ? "x" : "-",
                    (st.st_mode & S_IROTH) ? "r" : "-",
                    (st.st_mode & S_IWOTH) ? "w" : "-",
                    (st.st_mode & S_IXOTH) ? "x" : "-",
            //количество ссылок
                    st.st_nlink,
            //Имя и группа владельца
                    pw->pw_name,
                    gr->gr_name,
            //размер файла
                    st.st_size,
            //время последней модификации
                    time,
            //имя файла
                    entry->d_name);
        }
        else
        {
            perror("diropen");
            return -1;
        }
    closedir(dir);
    return 0;
}