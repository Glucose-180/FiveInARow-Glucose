/* control.c: This is the source file for controlling the game */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "glucose.h"

int board[BDMAX + 1][BDMAX + 1]; /*Board[x][y]*/
char player_index;    /*Player 1 or 2*/
int ix0 = -1, iy0 = -1;		/* point of last step */
int set_count;     /*Set count*/
int game_mode;      /*1-2Players; 2-1Player&Computer, player first; 3-1Player&Computer, computer first*/
char debug_en = 0; /* debug: 1-enabled */
char noboard_en = 0; /* don't display board */

unsigned int move_vert = 0; /* move vertically */
unsigned int x_size = 2; /* x scale wide */

/* board_display: Display the Board */
void board_display()
{
	int i, j;

	for (j = BDMAX; j >= 0; j--)
	{
		printf("%2d", j + move_vert);		/*y scale*/
		for (i = 0; i <= BDMAX; i++)
			printf("%s", board_str[board[i][j]]);
		putchar('\n');
	}
	printf("  ");
	for (i = 'A'; i <= BDMAX + 'A'; i++)
	{
		for (j = 1; j < (int)x_size; ++j)
			putchar(' ');
		putchar(i);           /* x scale: 0 ~ E*/
	}
	printf("\n\n");
}

/* board_init: Init the values of board[][] */
void board_init()
{
	int i, j;

	board[0][0] = LLEDGE;
	board[BDMAX][0] = RLEDGE;
	board[BDMAX][BDMAX] = RUEDGE;
	board[0][BDMAX] = LUEDGE;
	for (j = 1; j < BDMAX; j++)
	{
		board[0][j] = LEDGE;
		board[BDMAX][j] = REDGE;
	}
	for (i = 1; i < BDMAX; i++)
	{
		board[i][0] = OEDGE;
		board[i][BDMAX] = UEDGE;
	}
	for (i = 1; i < BDMAX; i++)
		for (j = 1; j < BDMAX; j++)
			board[i][j] = INNER;
}

/* game_start_2p: Game Starts: 2 Player */
void game_start_2p()
{
	int ix = -1, iy = -1;
	int iR; /*Judge result*/
	player_index = 1;

	while (1)
	{
		if (player_index == 1)
		{
			while (!(ix >= 0 && ix <= BDMAX && iy >= 0 && iy <= BDMAX && board[ix][iy] < 10))
			{
				printf("%d: Player 1 sets: ", set_count);
				if ((ix = getchar()) == 'q' || ix == 'Q')
					return;  /* quit */
				if (scanf("%d", &iy) < 1)
					iy = -1;
				else
					iy -= move_vert;
				fclear(stdin);
				if ((ix -= 'a') < 0)  /* Upper latter */
					ix += 'a' - 'A';
			}
			if (ix0 != -1)
			{
				board[ix0][iy0] = CCHESS;
			}
			board[ix][iy] = PNEW;
			ix0 = ix;
			iy0 = iy;
			player_index = 2;
		}
		else
		{
			while (!(ix >= 0 && ix <= BDMAX && iy >= 0 && iy <= BDMAX && board[ix][iy] < 10))
			{
				printf("%d: Player 2 sets: ", set_count);
				if ((ix = getchar()) == 'q' || ix == 'Q')
					return;  /* quit */
				if (scanf("%d", &iy) < 1)
					iy = -1;
				else
					iy -= move_vert;
				fclear(stdin);
				if ((ix -= 'a') < 0)  /* Upper latter */
					ix += 'a' - 'A';
			}
			if (ix0 != -1)
			{
				board[ix0][iy0] = PCHESS;
			}
			board[ix][iy] = CNEW;
			ix0 = ix;
			iy0 = iy;
			player_index = 1;
		}
		ix = iy = -1;
		set_count++;
		CLEAR;
		if (noboard_en == 0)
			board_display();
		iR = game_judge();
		if (player_index == 2)
		{
			int uB;
			uB = calc_breaker(PCHESS, ix0, iy0, board);
			if (uB == 2 || (uB == 1 && iR != 1))
			{
				printf("Player 1 has trigged Balance Breaker!\n");
				PAUSE;
				if (uB == 2)
					break;
			}
		}
		if (iR != 0)
		{
			printf("Player %d wins with %d steps!\n", iR, set_count - 1);
			PAUSE;
			break;
		}
	}
}

/*game_start_cp: Game Starts: One players and Computer*/
void game_start_cp()
{
	char c;
	int ix = -1, iy = -1;
	int iR; /*Judge result*/
	char rec_temp[12];

	player_index = game_mode - 1;

	if (game_mode == 3)
	{  /* swap board strings */
		char* temp;

		temp = board_str[PCHESS];
		board_str[PCHESS] = board_str[CCHESS];
		board_str[CCHESS] = temp;

		temp = board_str[PNEW];
		board_str[PNEW] = board_str[CNEW];
		board_str[CNEW] = temp;
	}

	while (1)
	{
		if (player_index == 1)
		{
			while (!(ix >= 0 && ix <= BDMAX && iy >= 0 && iy <= BDMAX && board[ix][iy] < 10))
			{
				printf("%d: You set: ", set_count);
				if ((ix = getchar()) == 'q' || ix == 'Q')
					return;  /* quit */
				if (scanf("%d", &iy) < 1)
					iy = -1;
				else
					iy -= move_vert;
				fclear(stdin);
				if ((ix -= 'a') < 0)  /* Upper latter */
					ix += 'a' - 'A';
			}
			if (ix0 != -1)
			{
				board[ix0][iy0] = CCHESS;
			}
			board[ix][iy] = PNEW;
			/* add record */
			sprintf(rec_temp, "P: %c%d\n", ix + 'A', iy);
			rec_add(rec_temp);

			ix0 = ix;
			iy0 = iy;
			player_index = 2;			
			CLEAR;
			if (noboard_en == 0)
				board_display();
		}
		else
		{
			while (!(ix >= 0 && ix <= BDMAX && iy >= 0 && iy <= BDMAX && board[ix][iy] < 10))
			{
				/*			ix = 0;
							scanf("%c%d", &ix, &iy);
							getchar();
							if ((ix = ix - 97) < 0)
								ix += 32;                */
				unsigned uXY;
				uXY = calc_deep(CCHESS);
				ix = uXY / 100;
				iy = uXY % 100;

				if (debug_en != 0)
				{
					while (1)
					{
						value_deep(CCHESS, ix, iy, 1);  /* show values */
						printf("Interfere: ");
						/* get interfere location */
						if (!isalpha(c = tolower(getchar())))
						{
							if (c != '\n')
								fclear(stdin);
							break;
						}
						ix = c - 'a';
						if (scanf("%d%*c", &iy) < 1)
						{
							fclear(stdin);
							break;
						}
					}
				}

			}
			if (ix0 != -1)
			{
				board[ix0][iy0] = PCHESS;
			}
			board[ix][iy] = CNEW;
			/* add record */
			sprintf(rec_temp, "C: %c%d\n", ix + 'A', iy);
			rec_add(rec_temp);

			ix0 = ix;
			iy0 = iy;
			player_index = 1;
			CLEAR;
			if (noboard_en == 0)
				board_display();
			printf("%d: Computer sets: ", set_count);
			printf("%c%d\n", ix + 'A', iy + move_vert);
		}
		ix = iy = -1;
		set_count++;
		iR = game_judge();
		if (game_mode == 2 && player_index == 2)
		{
			int uB = calc_breaker(PCHESS, ix0, iy0, board);
			if (uB == 2 || (uB == 1 && iR != 1))
			{
				printf("You have trigged Balance Breaker!\n");
				rec_add("Breaker\n");
				if (uB != 2) /* 2-longer than 5 */
				{
					c = 0;
					printf("Do you want to ignore and continue? (y/n) ");
					while (c != 'y' && c != 'n')
						c = tolower(getchar());
					if (c == 'n')
						iR = 2; /* terminate game */
					fclear(stdin);
				}
				else
					iR = 2; /* computer wins */
			}
		}
		if (iR != 0)
		{
			printf("You %s! (%d steps)\n", (iR == 1 ? "win" : "LOSE"), set_count - 1);
			sprintf(rec_temp, "%c win(%d)", (iR == 1 ? 'P' : 'C'), set_count - 1);
			rec_add(rec_temp);
			PAUSE;
			break;
		}
	}
}


/* fclear: Clear buffer of fp */
void fclear(FILE* fp)
{
	int c;
	while ((c = fgetc(fp)) != EOF && c != '\n')
		;
}