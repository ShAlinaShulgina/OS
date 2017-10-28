#include "fs.h"

int main(int argc, char *argv[])
{
	if(argc >= 2)
	{
		if (!(strcmp(argv[1], "-add"))) //добавить
			add_file(argv[2]);

		else if (!(strcmp(argv[1], "-f"))) // найти
			find_file(argv[2]);

		else if(!(strcmp(argv[1], "-cl"))) //очистить
			empty();
		else if (!(strcmp(argv[1], "-l"))) // список
			get_list();
		else if (!(strcmp(argv[1], "-d"))) //удалить (пометить)
			deleted(argv[2]);
		else if (!(strcmp(argv[1], "-p"))) // вывести весь файл
			print_all_file();
	}
    return 0;
}
