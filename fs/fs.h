#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

const char fs[] = "FS.txt";

struct info
{
	char del; //флаг для удаления
	char file_name[128]; //имя файла
	int size_name_file;
	int size; //размер файла
};

char* read_file(int fd, off_t size)
{	
	char *file = (char*) malloc(size + 1);
	read(fd, file, size);
	file[size + 1] = '\0';
	free(file);
	return file;
}

void add_file(char name[])
{
	struct info inf;
	inf.del = '*';	    
	strncpy(inf.file_name, name, strlen(name)); //имя файла
	inf.size_name_file = strlen(name);
	int fd = open(name, O_RDWR, O_EXCL);
	if (fd < 0)
	{
        	perror("open");
        	return;
	}
	struct stat st;
	stat(name, &st);
	inf.size = st.st_size; //размер файла

	char buff[64];
    
	char *mass = (char*)malloc(inf.size + 1);
	mass = read_file(fd, inf.size); //считанный файл

	int newfd = open(fs, O_RDWR | O_CREAT | O_APPEND , S_IREAD | S_IWRITE);
	write(newfd, &inf.del, sizeof(char)); //файл на удалении или нет
	sprintf(buff, "%d", inf.size_name_file);
	write(newfd, buff, strlen(buff)); //длина имени файла
	write(newfd, inf.file_name, strlen(name)); //имя файла
	sprintf(buff, "%d", inf.size);
	write(newfd, buff, strlen(buff)); //размер содержимого файла
	write(newfd, mass, inf.size); //содержимое файла

	close(fd);
	close(newfd);
}

void find_file(char name[])
{
	int fd = open(fs, O_RDWR, O_EXCL);
	if (fd < 0)
	{
		perror("open");
		return;
	}

	struct info inf;
	char sz[16], nm[128];
	size_t t = 0; // для хранения всех размеров файлов
	
	while(read(fd, &inf.del, 1)) //считать флаг
	{
		lseek(fd, t + 1, SEEK_SET);
		
		read(fd, sz, sizeof(int)); //считать размер имени
		inf.size_name_file = atoi(sz);
		if(inf.size_name_file < 10)
			t -= 1;
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);

		memset(inf.file_name, '\0', strlen(inf.file_name));
		read(fd, inf.file_name, inf.size_name_file); //считать имя
		t += inf.size_name_file;
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);

		read(fd, sz, sizeof(int)); //считать размер считываемого файла
		inf.size = atoi(sz);
		if (inf.size > 1000)
			t += 1;
		lseek(fd, sizeof(int) + sizeof(int) + t - 2, SEEK_SET);

		char *mass = (char*)malloc(inf.size + 1);
		read(fd, mass, inf.size);
		t += inf.size;

		if (!(strcmp(name, inf.file_name)))
		{
			if (inf.del == '*')
			{
				printf("Файл найден.\n");
				write(1, mass, inf.size);
				free(mass);
				close(fd);
				return;
			}
			else if (inf.del == '~')
			{
				printf("Файл удален.\n");
				free(mass);            	
				close(fd);
				return;	
			}
		}
		lseek(fd, sizeof(int) + sizeof(int) + t - 2, SEEK_SET);
		t += sizeof(int) + sizeof(int) - 2;
		free(mass);
	}
	printf("Файл не найден.\n");
	close(fd);
}

void get_list()
{
	int fd = open(fs, O_RDWR, O_EXCL);
	if (fd < 0)
	{
		perror("open");
		return;
	}

	struct info inf;
	char sz[16], nm[128];
	size_t t = 0; // для хранения всех размеров файлов
	
	while(read(fd, &inf.del, 1)) //считать флаг
	{
		lseek(fd, t + 1, SEEK_SET);
		
		read(fd, sz, sizeof(int));
		inf.size_name_file = atoi(sz);
		if(inf.size_name_file < 10)
			t -= 1;
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);

		memset(inf.file_name, '\0', strlen(inf.file_name));
		read(fd, inf.file_name, inf.size_name_file); 
		t += inf.size_name_file;
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);
		if (inf.del == '*')
			printf("%s\n", inf.file_name); //вывести имя файла

		read(fd, sz, sizeof(int));
		inf.size = atoi(sz);
		t += inf.size;
		if (inf.size > 1000)
			t += 1;

		lseek(fd, sizeof(int) + sizeof(int) + t - 2, SEEK_SET);
		t += sizeof(int) + sizeof(int) - 2;
	}
	close(fd);
}

void deleted(char name[])
{
	int fd = open(fs, O_RDWR, O_EXCL);
	if (fd < 0)
	{
		perror("open");
		return;
	}

	struct info inf;
	char sz[16], nm[128];
	size_t t = 0, s = 0; // для хранения всех размеров файлов
	
	while(read(fd, &inf.del, 1)) //считать флаг
	{
		lseek(fd, t + 1, SEEK_SET);
		s += 1;
		
		read(fd, sz, sizeof(int));
		inf.size_name_file = atoi(sz);
		if(inf.size_name_file < 10)
		{
			t -= 1;
			s -= 1;
		}
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);
		s -= 1;

		memset(inf.file_name, '\0', strlen(inf.file_name));
		read(fd, inf.file_name, inf.size_name_file); 
		t += inf.size_name_file;
		lseek(fd, sizeof(int) + t - 1, SEEK_SET);
		s += inf.size_name_file;

		read(fd, sz, sizeof(int));
		inf.size = atoi(sz);
		t += inf.size;
		s += inf.size;
		if (inf.size > 1000)
		{
			t += 1;
			s += 1;
		}
		
		if (!(strcmp(name, inf.file_name)))
		{
			lseek(fd, t - s, SEEK_SET);
			inf.del = '~';
			write(fd, &inf.del, 1);
			close(fd);
			return;
		}
		
		lseek(fd, sizeof(int) + sizeof(int) + t - 2, SEEK_SET);
		t += sizeof(int) + sizeof(int) - 2;
		s = 0;
	}
	close(fd);	
}

void empty()
{
	int fd = open(fs, O_TRUNC | O_EXCL);
	if (fd < 0)
	{
		perror("open");
		return;
	}
	close(fd);
}

void print_all_file()
{
	int fd = open(fs,  O_RDWR, O_EXCL);
	if (fd < 0)
	{
		perror("open");
		return;
	}
	struct stat st;
	stat(fs, &st);
	off_t size = st.st_size;
	char *mass = (char*) malloc(size + 1);
	read(fd, mass, size);
	mass[size + 1] = '\0';

	write(1, mass, size + 1);
	free(mass);
	close(fd);
}
