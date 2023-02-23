/* fileIO.c: This is the source file for reading and writing files on disk  */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "glucose.h"

char* prog_path = NULL;  /* the path of the program */
char rec_buf[8 * (BDMAX + 1) * (BDMAX + 1)] = { '\0' }; /* record buff */

const char config_name[] = "config.ini";
const char* config_const_name[] = {
	"calc_depth",
	"move_vert",
	"const_space",
	"const_val_get_c",
	"const_val_get_p",
	"const_val_pow",
	"const_val_added",
	"x_size"
};

const char board_name[] = "board.ini";
char* board_str[22] = {
	NULL,"©³","©·","©¿","©»",
	"©Ä","©Ó","©Ì","©Û","©à",
	"¡ñ","¡ø",NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,"¡ò","¡÷"
}; /* board strings read from file */

char welcome[WELCOMEMAX];

int prog_path_pt = 0;

/* get_path: extract the path of this program from argv[0]
0-succeeded; 1-failed. */
int get_path(char* prog_name)
{
	int i = 0;

	while (prog_name[i] != '\0')
	{
		if (prog_name[i] == '/' || prog_name[i] == '\\')
			prog_path_pt = i;
		i++;
	}
	prog_path = (char*)malloc(prog_path_pt + 2);
	if (prog_path != NULL)
	{
		strncpy(prog_path, prog_name, prog_path_pt + 1);
		prog_path[prog_path_pt + 1] = '\0';
		return 0;
	}
	else
		return 1;
}

/* rec_add: add one step to the record buffer */
void rec_add(char* t)
{
	static char* s = rec_buf;
	while (*t != '\0')
		*s++ = *t++;
}

struct tm* gettime()
{
	time_t tm;
	time(&tm);
	return localtime(&tm);
}

/* save_file: save the log file to disk */
int save_file()
{
	char* file_path_name = (char*)malloc(prog_path_pt + 2 + 20);
	char file_name[24];
	struct tm* utm;
	FILE* fp;

	if (file_path_name == NULL)
		return 1;
	utm = gettime();
	sprintf(file_name, "%4d-%.2d-%.2d_%.2d-%.2d.log", utm->tm_year + 1900, utm->tm_mon + 1, utm->tm_mday, utm->tm_hour, utm->tm_min);
	strcpy(file_path_name, prog_path);
	strcat(file_path_name, file_name);
	fp = fopen(file_path_name, "w");
	if (fp == NULL)
		return 1;
	fprintf(fp, "%s", rec_buf);
	fclose(fp);
	return 0;
}

/* read_config: read the config file from disk and set external variables
if config_name_got is NULL, then read from the default file name;
otherwise, read from the given file name */
int read_config(const char* config_name_got)
{
	char temp_str[20] = { '\0' };
	double temp_val = 0.0;
	char* config_path_name = NULL;
	FILE* fp;
	unsigned int i = 0;

	if (prog_path == NULL)
		return 1;
	config_path_name = (char*)malloc(prog_path_pt + 2 + (config_name_got == NULL ? sizeof(config_name) : strlen(config_name_got)));
	if (config_path_name == NULL)
		return 1;
	strcpy(config_path_name, prog_path);
	strcat(config_path_name, config_name_got == NULL ? config_name : config_name_got);
	fp = fopen(config_path_name, "r");
	if (fp == NULL)
		return 1;
	while (fscanf(fp, "%s = %lf", temp_str, &temp_val) == 2 || strcmp(temp_str, "welcome") == 0)
	{
		for (i = 0; i < sizeof(config_const_name) / sizeof(const char*); i++)
		{
			if (strcmp(config_const_name[i], temp_str) == 0)
				break;
		}
		switch (i)
		{
		case 0:
			calc_depth = (unsigned int)temp_val;
			break;
		case 1:
			move_vert = (debug_en != 0 ? 0 : (unsigned int)temp_val);
			break;
		case 2:
			const_space = (unsigned int)temp_val;
			break;
		case 3:
			const_val_get_c = temp_val;
			break;
		case 4:
			const_val_get_p = temp_val;
			break;
		case 5:
			const_val_pow = temp_val;
			break;
		case 6:
			const_val_added = (unsigned int)temp_val;
			break;
		case 7:
			x_size = (unsigned int)temp_val;
			break;
		default: /* welcome string */
			if (fscanf(fp, "%s", welcome) == 0)
				welcome[0] = '\0';
			temp_str[0] = '\0';
			break;
		}
	}
	fclose(fp);
	free(config_path_name);
	return 0;
}

/* read_board: read the board element strings from disk */
int read_board(const char* board_name_got)
{
	char* board_path_name = NULL;
	FILE* fp;
	int size = -1;  /* size of every board element */
	int i; /* index of every string */

	if (prog_path == NULL)
		return 1;
	board_path_name = (char*)malloc(prog_path_pt + 2 + (board_name_got == NULL ? sizeof(board_name) : strlen(board_name_got)));
	if (board_path_name == NULL)
		return 1;
	strcpy(board_path_name, prog_path);
	strcat(board_path_name, board_name_got == NULL ? board_name : board_name_got);
	fp = fopen(board_path_name, "r");
	if (fp == NULL)
		return 1;

	if (fscanf(fp, "%*s%d %d", &size, &i) < 2 || size <= 0)
		return 2;  /* invalid file data */

	while (i >= 0 && i < sizeof(board_str) / sizeof(char*))
	{
		if ((board_str[i] = (char*)malloc(size + 1)) == NULL)
			return 1;
		if (fscanf(fp, "%s", board_str[i]) < 1)
		{
			free(board_str[i]);
			board_str[i] = NULL;
		}
		if (fscanf(fp, "%d", &i) < 1)
			i = -1;
	}
	fclose(fp);
	return 0;
}