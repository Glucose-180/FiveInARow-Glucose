/* Glucose.c: This is the main source file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glucose.h"

static const char* str_date = "2023-01-07";  /* display last change date when started */

int main(int argc, char* argv[])
{
	char cMode = 0;
	char* config_name_got = NULL, *board_name_got = NULL;

	get_path(argv[0]);
	while (--argc)
	{
		++argv;
		if (strcmp(*argv, "-d") == 0 || strcmp(*argv, "--debug") == 0)  /* debug mode */
		{
			debug_en = 1;
			printf("** debug mode **\n");
		}
		else if (strcmp(*argv, "-n") == 0 || strcmp(*argv, "--no-board") == 0)
			noboard_en = 1;
		else if (strncmp(*argv, "--config=", 9) == 0)
			config_name_got = *argv + 9;
		else if (strncmp(*argv, "--board=", 8) == 0)
			board_name_got = *argv + 8;
		else if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)
		{
			printf("Usage:\n\t-d, --debug\n\t\tenable debug mode;\n\t-n, --no-board\n\t\tdon't display board;\n\t--config=<file>\n\t\tset <file> as config file;\n\t--board=<file>\n\t\tset <file> as board file;\n\t-h, --help\n\t\tshow help.\n");
			return 0;
		}
		/*else if (strncmp(*argv, "-s", 2) == 0)
		{
			calc_depth = atoi(*argv + 2);
		}
		else if (strncmp(*argv, "-m", 2) == 0)
		{
			move_vert = atoi(*argv + 2);
		}*/
	}
	if (read_config(config_name_got) != 0)
		fprintf(stderr, "**Error: Failed to read config!\n");
	if (noboard_en == 0 && (read_board(board_name_got) != 0))
		fprintf(stderr, "**Error: Failed to read board!\n");
	if (debug_en != 0)
	{
		printf("%s = %u\n", config_const_name[0], calc_depth);
		printf("%s = %u\n", config_const_name[1], move_vert);
		printf("%s = %u\n", config_const_name[2], const_space);
		printf("%s = %.3lf\n", config_const_name[3], const_val_get_c);
		printf("%s = %.3lf\n", config_const_name[4], const_val_get_p);
		printf("%s = %.3lf\n", config_const_name[5], const_val_pow);
		printf("%s = %u\n", config_const_name[6], const_val_added);
		printf("%s = %u\n", config_const_name[7], x_size);
		PAUSE;
		CLEAR;
	}
	printf("%s @ %s\n", welcome, str_date);
	printf("--  Five in a Row  --\n");
	printf("1 - Two players;\n");
	printf("2 - One player and computer;\n");
	printf("Please enter 1 or 2: ");
	while (!(game_mode == 1 || game_mode == 2)) 
		game_mode = getchar() - '0';
	if (game_mode == 2)
	{
		printf("Do you want to set first? (y/n) ");
		while (!(cMode == 'y' || cMode == 'Y' || cMode == 'n' || cMode == 'N'))
			cMode = getchar();
		if (cMode == 'n' || cMode == 'N')
			game_mode = 3;
	}
	CLEAR;
	fclear(stdin);
#ifndef LINUX
	system("color F0");
#endif
	if (noboard_en == 0)
	{
		board_init();
		board_display();
	}
	set_count = 1;
	if (game_mode == 1)
	{
		game_start_2p();
	}
	else
	{
		game_start_cp();
		if (prog_path == NULL)
			fprintf(stderr, "**Error: Failed to get path!\n");
		else
		{
			if (save_file() != 0)
				fprintf(stderr, "**Error: Failed to save file!\n");
			else
				printf("File has been saved at %s\n", prog_path);
		}
		PAUSE;
	}
	return 0;
}
