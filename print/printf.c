#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>

//https://ru.wikipedia.org/wiki/Itoa_(%D0%A1%D0%B8)
//reverse:  переворачиваем строку s на месте
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
// itoa:  конвертируем n в символы в s
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  // записываем знак
         n = -n;          // делаем n положительным числом
     i = 0;
     do {       // генерируем цифры в обратном порядке
         s[i++] = n % 10 + '0';   // берем следующую цифру
     } while ((n /= 10) > 0);     // удаляем
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void print(char *format, ...)
{
    char *p; //для va_list
    char ch, *s, str[10]; //для символа, '%s', itoa и 'c'
    char buff[1024]; //для итоговой строки
    int in = 0, j = 0;
    double d = 0;
    va_list args;
    va_start(args, format);
    for(p = format; *p; p++)
    {
        if (*p != '%')
        {
            ch = *p;
            buff[j] = ch;
            j++;
            continue;
        }
        switch(*++p)
        {
        case 'd':
            in = va_arg(args, int);
            itoa(in, str);
            for(size_t i = 0; i < strlen(str); i++)
            {
                buff[j] = str[i];
                j++;
            }
            break;
        case 'c':
            ch = va_arg(args, int);
           	buff[j] = ch;
            j++;
            break;
        case 's':
            for(s = va_arg(args, char *); *s; s++)
            {
                ch = *s;
                buff[j] = ch;
                j++;
            }
            break;
        default:
            ch = *p;
            buff[j] = ch;
            j++;
            break;
        }
    }
    va_end(args);
    write(1, buff, j);
}

int main()
{
    char a[] = "Hello%c %d -> %s \n";
    print(a, '!', -12, "12s");
    print("%s%c%d%c%d \n", "KMBO", '-',02, '-', 15);
    print("\n");
    char filename[] = "test.txt";
    int fd = open(filename, O_RDONLY, O_EXCL);
    if (fd < 0)
    {
    	perror("open");
    	return 1;
    }
    struct stat st;
    stat(filename, &st);
    long long size = st.st_size;
    char file[size];
    if(size > 1024)
    {
    	int  j = 0, count = (int)(size / 1024) - 1;
    	for(int i = 0; i < count; i++)
    	{
    		read(fd, file, 1024);
    		long long int l = lseek(fd,  j += 1024, SEEK_SET);
    		print(file);	
   		}
   		read(fd, file, size - 1024 * count);
   		printf(file);
   	}
   	else
   	{
   		read(fd, file, size);
   		print(file);
   	}
    print("\n");
    close(fd);
    return 0;
}