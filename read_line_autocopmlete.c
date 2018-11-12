#include "shell.h"

ft_autocomplete(char *line, t_cpos *pos)
{
	static int is_pressed = 0;

	/*
		если мы в слове
			сравниваем начало слова с 
				файлы в path
				файлы в тек. папке
				переменные

		если не в слове, но после слова
			файлы тек. папки

	*/
}