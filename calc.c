/* calc.c: This is the source file for calculation,
including judging, decision, balance breaker and etc. */

#include <stdio.h>
#include <math.h>
#include "glucose.h"

unsigned int const_space = 1;  /* value added because of space */
double const_val_get_c = 0.04;  /* value multed for computer per step */
double const_val_get_p = 0.05;  /* value multed for player per step */
double const_val_pow = 7.6;  /* value(shallow) powered */
unsigned int const_val_added = 15;  /* value added once */

unsigned int calc_depth = 0; /* how many turns */

int board_cp[BDMAX + 1][BDMAX + 1];  /* board copy */

int direction[][2] = {  /* Scanning direction vector */
	{1, 0},  /*Right*/
	{0, 1},  /*Up*/
	{1, -1}, /*RightDown*/
	{1, 1}   /*RightUp*/
};

/* in_board: Is (x, y) in the board?
1-in; 0-out. */
static inline int in_board(int x, int y)
{
	if (x >= 0 && x <= BDMAX && y >= 0 && y <= BDMAX)
		return 1;
	else
		return 0;
}

/*game_judge: Judge if anyone wins
1-Player1; 2-Player2; 0-Nobody. */
int game_judge()
{
	int i, j;
	int iC1, iC2;  /* Count 1, 2 */
	iC1 = iC2 = 0;

	for (j = 0; j <= BDMAX; j++)  /*Scanning Direction:  -  */
	{
		iC1 = iC2 = 0;
		for (i = 0; i <= BDMAX; i++)
		{
			if (board[i][j] == PCHESS || board[i][j] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[i][j] == CCHESS || board[i][j] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	for (i = 0; i <= BDMAX; i++)  /*Scanning Direction:  |  */
	{
		iC1 = iC2 = 0;
		for (j = 0; j <= BDMAX; j++)
		{
			if (board[i][j] == PCHESS || board[i][j] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[i][j] == CCHESS || board[i][j] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	for (i = 4; i <= BDMAX; i++)  /*Scanning Direction:  LeftUp  */
	{
		iC1 = iC2 = 0;
		for (j = 0; j <= i; j++)
		{
			if (board[i - j][j] == PCHESS || board[i - j][j] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[i - j][j] == CCHESS || board[i - j][j] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	for (j = 1; j <= BDMAX - 4; j++)  /*Scanning Direction:  LeftUp  */
	{
		iC1 = iC2 = 0;
		for (i = 0; i <= BDMAX - j; i++)
		{
			if (board[BDMAX - i][j + i] == PCHESS || board[BDMAX - i][j + i] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[BDMAX - i][j + i] == CCHESS || board[BDMAX - i][j + i] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	for (i = BDMAX - 4; i >= 0; i--)  /*Scanning Direction:  RightUp  */
	{
		iC1 = iC2 = 0;
		for (j = 0; j <= BDMAX - i; j++)
		{
			if (board[i + j][j] == PCHESS || board[i + j][j] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[i + j][j] == CCHESS || board[i + j][j] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	for (j = 1; j <= BDMAX - 4; j++)  /*Scanning Direction: RightUp  */
	{
		iC1 = iC2 = 0;
		for (i = 0; i <= BDMAX - j; i++)
		{
			if (board[i][j + i] == PCHESS || board[i][j + i] == PNEW)
				iC1++;
			else
				iC1 = 0;
			if (board[i][j + i] == CCHESS || board[i][j + i] == CNEW)
				iC2++;
			else
				iC2 = 0;
			if (iC1 >= 5)
				return 1;
			if (iC2 >= 5)
				return 2;
		}
	}
	return 0; /*Nobody wins  */
}

/* value_shallow: calculate the value of board[x][y]
iCd: Which player, 10(Black) or 20(White); breaker_en: 1-enabled 0-disenabled*/
unsigned int value_shallow(int iCd, int x, int y, int breaker_en)
{
	int iCdo = (iCd == CCHESS ? PCHESS : CCHESS);  /*other side*/
	unsigned int iR1 = 0, iR2 = 0, iR = 0; /*return value*/
	int iCw, iCb, iCw1, iCb1, iCw2, iCb2; /*Count how many without blank, white|black; 1 or 2 refers to the direction, + or - */
	char cStopw = 0, cStopb = 0; /* Stop count if 1 */
	int i, j;
	int Bd1 = 0, Bd2 = 0;
	int iBd1 = 0, iBd2 = 0;  /*Block Distance */
	double dx, dy;

	/* The situation about 3 or 4 */
	int uB = calc_breaker(iCd, x, y, board_cp);
	int uBo = calc_breaker(iCdo, x, y, board_cp);

	if (breaker_en != 0)
	{
		switch (uB)
		{
		case 2:/* Longer than 5 */
			return 0;
			break;
		case 3:
			iR = max(iR, 1100); /* single 3 and 4 */
			break;
		default:
			break;
		}
		switch (uBo)
		{
		case 1:
		case 3:
			iR = max(iR, 900);  /* defend double 3, 4 and 3 + 4 */
			break;
		case 2: /* longer than 5 */
			iR = max(iR, 1400);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (uB)
		{
		case 1:
		case 3: /* Double 3 or 4, or Single 3 and 4 */
			iR = max(iR, 1100);
			break;
		case 2: /* longer than 5 */
			iR = max(iR, 1600);
			break;
		default:
			break;
		}
		switch (uBo)
		{
		case 3:
			iR = max(iR, 900); /*defend single 3 and 4 */
			break;
		default:
			break;
		}
	}

	for (i = 0; i < 4; i++)  /* Scanning */
	{
		int px, py;
		iCw = iCb = iCw1 = iCb1 = iCw2 = iCb2 = 0;
		iR1 = iR2 = 0;
		/*Consider boundary*/
		if (in_board(x - direction[i][0], y - direction[i][1]) == 0)
		{
			Bd2 = iCdo;  /*If it is boundary, make it Black */
			Bd1 = board_cp[x + direction[i][0]][y + direction[i][1]];
		}
		else if (in_board(x + direction[i][0], y + direction[i][1]) == 0)
		{
			Bd1 = iCdo;
			Bd2 = board_cp[x - direction[i][0]][y - direction[i][1]];
		}
		else
		{
			Bd1 = board_cp[x + direction[i][0]][y + direction[i][1]];
			Bd2 = board_cp[x - direction[i][0]][y - direction[i][1]];
		}

		cStopw = cStopb = 0;
		for (j = 0;; j++) /*Scanning Direction: +direction[i] */
		{
			px = x + j * direction[i][0];
			py = y + j * direction[i][1];
			if (in_board(px, py) == 0)
				break;  /* out of boundary */

			if (Bd1 != iCdo && Bd1 != iCdo + 1) /* attack */
			{
				if (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1) /*Black */
				{
					iBd1 = j - 1;
					break;
				}
				if (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1) /*White */
					iR1 += const_val_added / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1) / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1);
			}
			else  /* defend */
			{
				if (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1) /*Black */
					iR1 += const_val_added / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1);
				if (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1) /*White */
				{
					iBd1 = j - 1;
					break;
				}
			}
			if (in_board(px + direction[i][0], py + direction[i][1]) == 0)
				iBd1 = j;
			if (j > 0)  /*ignoring Bd_cp[x][y] itself*/
			{
				if (board_cp[px][py] != iCd && board_cp[px][py] != iCd + 1)
					cStopw = 1;
				if (board_cp[px][py] != iCdo && board_cp[px][py] != iCdo + 1)
					cStopb = 1;
				if (cStopw == 0 && (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1))
					iCw1++;
				if (cStopb == 0 && (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1))
					iCb1++;
			}
		}

		cStopw = cStopb = 0;
		for (j = 0;; j++) /*Scanning Direction: -direction[i] */
		{
			px = x - j * direction[i][0];
			py = y - j * direction[i][1];
			if (in_board(px, py) == 0)
				break;  /* out of boundary */

			if (Bd2 != iCdo && Bd2 != iCdo + 1) /* attack */
			{
				if (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1) /*Black */
				{
					iBd2 = j - 1;
					break;
				}
				if (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1) /*White */
					iR2 += const_val_added / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1) / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1);
			}
			else  /* defend */
			{
				if (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1) /*Black */
					iR2 += const_val_added / (j > 2 ? 2 : 1) / (j > 4 ? 2 : 1);
				if (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1) /*White */
				{
					iBd2 = j - 1;
					break;
				}
			}
			if (in_board(px - direction[i][0], py - direction[i][1]) == 0)
				iBd2 = j;
			if (j > 0)  /*ignoring Bd_cp[x][y] itself*/
			{
				if (board_cp[px][py] != iCd && board_cp[px][py] != iCd + 1)
					cStopw = 1;
				if (board_cp[px][py] != iCdo && board_cp[px][py] != iCdo + 1)
					cStopb = 1;
				if (cStopw == 0 && (board_cp[px][py] == iCd || board_cp[px][py] == iCd + 1))
					iCw2++;
				if (cStopb == 0 && (board_cp[px][py] == iCdo || board_cp[px][py] == iCdo + 1))
					iCb2++;
			}
		}

		iCw = iCw1 + iCw2;
		iCb = iCb1 + iCb2;
		if ((Bd1 != iCdo && Bd1 != iCdo + 1 && Bd2 != iCdo && Bd2 != iCdo + 1) || ((Bd1 == iCdo || Bd1 == iCdo + 1) && (Bd2 == iCdo || Bd2 == iCdo + 1)))
		{
			if (iBd1 + iBd2 >= 4)
			{
				iR += iR1 + iR2 + (iBd1 + iBd2 - 4) * const_space;
				if (iCw >= 4) /*4 White*/
					iR = 1600;
				else if (iCb >= 4) /*4 Black*/
					iR = max(iR, 1400);
				else if (iCw == 3 && iBd1 > iCw1 && iBd2 > iCw2) /*Confirm that there isn't any block on the terminal*/
					iR = max(iR, 1200);
				else if (iCb == 3 && iBd1 > iCb1 && iBd2 > iCb2)
					iR = max(iR, 1000);
				/*			if (iR >= 1000)*/
				/*				return iR;*/
			}
		}
		else /*One side is attack, the other is defence*/
		{
			if (iBd1 >= 4)
			{
				iR += iR1 + (iBd1 - 4) * const_space;
				if (Bd1 == iCdo || Bd1 == iCdo + 1)
				{
					if (iCb >= 4)
						iR = max(iR, 1400);
					else if (iCb == 3 && iBd1 > iCb1 && iBd2 > iCb2)
						iR = max(iR, 1000);
					/*				if (iR >= 1000)*/
					/*					return iR;*/
				}
				if (Bd1 == iCd || Bd1 == iCd + 1)
				{
					if (iCw >= 4)
						iR = 1600;
					else if (iCw == 3 && iBd1 > iCw1 && iBd2 > iCw2)
						iR = max(iR, 1200);
					/*				if (iR >= 1000)*/
					/*					return iR;*/
				}
			}
			if (iBd2 >= 4)
			{
				iR += iR2 + (iBd2 - 4) * const_space;
				if (Bd2 == iCdo || Bd2 == iCdo + 1)
				{
					if (iCb >= 4)
						iR = max(iR, 1400);
					else if (iCb == 3 && iBd1 > iCb1 && iBd2 > iCb2)
						iR = max(iR, 1000);
					/*				if (iR >= 1000)*/
					/*					return iR;*/
				}
				if (Bd2 == iCd || Bd2 == iCd + 1)
				{
					if (iCw >= 4)
						iR = 1600;
					else if (iCw == 3 && iBd1 > iCw1 && iBd2 > iCw2)
						iR = max(iR, 1200);
					/*				if (iR >= 1000)*/
					/*					return iR;*/
				}
			}
		}
	}

	if (breaker_en == 1 && uB == 1 && iR < 1600)
		return 0;  /*Balance Breaker*/

	if (iR < 900) /* general situation */
		iR = pow(900, 1 - 1 / const_val_pow) * pow(iR, 1 / const_val_pow);
		/* use a concave function iR = 900 ^ (1-1/k) * iR ^ (1/k) to transform */

	/* calculate the distance to the center */
	dx = (double)x - (double)(BDMAX / 2);
	dy = (double)y - (double)(BDMAX / 2);
	iR += 20 - sqrt(dx * dx + dy * dy); /* set chess as closer to center as possible */

	return iR;
}

/*calc_shallow: Calc the next step and max value
return format: xxyyvvvv */
unsigned int calc_shallow(int iCd, int breaker_en)
{
	int i, j;
	unsigned int uMaxV = 0, uV = 0; /*MaxValue*/
	int mvx = 0, mvy = 0;  /*the x,y of the uMaxV*/

	for (j = 0; j <= BDMAX; j++)
	{
		for (i = 0; i <= BDMAX; i++)
		{
			if (board_cp[i][j] >= 10)  /*skip occupied points*/
				continue;
			uV = value_shallow(iCd, i, j, breaker_en);
			if (uV > uMaxV)
			{
				uMaxV = uV;
				mvx = i;
				mvy = j;
			}
		}
	}
	return (100 * mvx + mvy) * 10000 + uMaxV;
}

/*calc_breaker: Judge if (x, y) is balance breaker
0-None; 1-Double 3 or 4; 2-Longer than 5; 3-Single 3 and 4*/
unsigned int calc_breaker(int iCd, int x, int y, int Bd_cp[][BDMAX + 1])  /*iCd: Which side, 10(Black) or 20(White)*/
{
	int i, j;
	int iCb1 = 0, iCb2 = 0;  /*Blank count*/
	int iCdo;  /*Other Side*/
	int iCL3 = 0, iC4 = 0;  /*Count Live 3 & 4*/
	int iC1 = 0, iC2 = 0, iCg1 = 0, iCg2 = 0;  /*Count link; g:Link without blank*/
	int iTmn1, iTmn2;  /*Terminal: Blank or Other's chess?*/
	iCdo = (iCd == CCHESS ? PCHESS : CCHESS);

	for (i = 0; i < 4; i++)  /*Scanning*/
	{
		int px, py;
		iCb1 = iCb2 = iC1 = iC2 = iCg1 = iCg2 = 0;
		iTmn1 = iTmn2 = 1;
		for (j = 1;; j++) /*Scanning Direction: +direction[i] */
		{
			px = x + j * direction[i][0];
			py = y + j * direction[i][1];
			if (in_board(px, py) == 0)
				break;  /* out of boundary */

			if (Bd_cp[px][py] < 10)
				iCb1++;
			if (iCb1 >= 2)
			{
				iTmn1 = 0; /*Blank*/
				break;
			}
			if (Bd_cp[px][py] == iCdo || Bd_cp[px][py] == iCdo + 1)
			{
				iTmn1 = 1;  /*Other's*/
				break;
			}
			if (Bd_cp[px][py] == iCd || Bd_cp[px][py] == iCd + 1)
			{
				iC1++;
				if (iCb1 == 0)
					iCg1++;
			}
		}

		for (j = 1;; j++) /*Scanning Direction: -direction[i] */
		{
			px = x - j * direction[i][0];
			py = y - j * direction[i][1];
			if (in_board(px, py) == 0)
				break;  /* out of boundary */

			if (Bd_cp[px][py] < 10)
				iCb2++;
			if (iCb2 >= 2)
			{
				iTmn2 = 0; /*Blank*/
				break;
			}
			if (Bd_cp[px][py] == iCdo || Bd_cp[px][py] == iCdo + 1)
			{
				iTmn2 = 1;  /*Other's*/
				break;
			}
			if (Bd_cp[px][py] == iCd || Bd_cp[px][py] == iCd + 1)
			{
				iC2++;
				if (iCb2 == 0)
					iCg2++;
			}
		}

		if (iCg1 + iC2 == 2 && iTmn2 == 0 && iCb1 > 0)
			iCL3++;  /*Live 3*/
		if (iCg2 + iC1 == 2 && iTmn1 == 0 && iCb2 > 0 && (iCg1 != iC1 || iCg2 != iC2))
			iCL3++;  /*Live 3*/
		if (iCg1 + iC2 == 3 && (iTmn1 == 0 || iCb2 > 0 || iTmn2 == 0 || iCb1 > 0))
			iC4++;  /*4*/
		if (iCg2 + iC1 == 3 && (iTmn1 == 0 || iCb2 > 0 || iTmn2 == 0 || iCb1 > 0) && (iCg1 != iC1 || iCg2 != iC2))
			iC4++;  /*4*/
		if (iCg1 + iCg2 > 4)  /*Longer than 5*/
			return 2;
	}

	if (iCL3 >= 2 || iC4 >= 2)
		return 1;  /*Double 3 or double 4*/
	else if (iCL3 == 1 && iC4 == 1)
		return 3;
	else
		return 0;
}

/*calc_deep: Calc the next step(Consider more STEPs)*/
unsigned int calc_deep(int iCd)
{
	int i, j;
	unsigned int uMaxV = 0, uV = 0; /*MaxValue*/
	int mvx = 0, mvy = 0;  /*the x,y of the uMaxV*/

	for (i = 0; i <= BDMAX; i++)
	{
		for (j = 0; j <= BDMAX; j++)
		{
			if (board[i][j] >= 10)  /*skip occupied points*/
				continue;
			uV = value_deep(iCd, i, j, 0);
			if (uV > uMaxV)
			{
				uMaxV = uV;
				mvx = i;
				mvy = j;
			}
		}
	}
	return (100 * mvx + mvy);
}

/*value_deep: Consider more STEPs
debug_en: !0-print correlative values */
unsigned int value_deep(int iCd, int x, int y, int debug_en)
{
	int iCdo = (iCd == CCHESS ? PCHESS : CCHESS);  /*other side*/
	int i, j;
	unsigned int ct;
	int calc_rt;
	unsigned int rt = 10000;  /* base */
	unsigned int val = 0, val_get_c = 0, val_get_p = 0; /* value sum, computer and person*/

	/* copy the board for calculation */
	for (i = 0; i <= BDMAX; i++)
		for (j = 0; j <= BDMAX; j++)
			board_cp[i][j] = board[i][j];

	/* if I set here(x,y)... */
	val = value_shallow(iCd, x, y, game_mode - 2);
	board_cp[x][y] = iCd;

	if (debug_en != 0)
		printf("[%c%d: %u]\n", 'A' + x, y, val);

	if (val > 900) /* important cases */
	{
		rt += max(calc_depth, 1) * val;
		return rt;
	}
	else if (val == 0)  /*breaker*/
		return 0;
	else
		rt += val;
	for (ct = 2; ct <= 2 * calc_depth; ct++)
	{
		if (ct % 2 != 0) /* computer */
		{
			calc_rt = calc_shallow(iCd, game_mode - 2);
			val = calc_rt % 10000;
			calc_rt /= 10000;
			board_cp[calc_rt / 100][calc_rt % 100] = iCd;
			if (debug_en != 0)
				printf("[%c%d: %u]\n", 'A' + calc_rt / 100, calc_rt % 100, val);
			if (val > 1550) /* win */
			{
				rt = (unsigned int)(10000.0 + ((double)calc_depth * (double)val) * const_val_get_c);
				return rt;
			}
			val_get_c += val * pow(0.5, (double)ct - 2.0);
		}
		else /* player */
		{
			calc_rt = calc_shallow(iCdo, 3 - game_mode);
			val = calc_rt % 10000;
			calc_rt /= 10000;
			board_cp[calc_rt / 100][calc_rt % 100] = iCdo;
			if (debug_en != 0)
				printf("[%c%d: %u]\n", 'A' + calc_rt / 100, calc_rt % 100, val);
			if(val > 1550) /* win */
			{
				rt = (unsigned int)(10000.0 - ((double)calc_depth * (double)val) * const_val_get_p);
				return rt;
			}
			val_get_p += val * pow(0.5, (double)ct - 2.0);
		}
	}
	rt += (unsigned int)(const_val_get_c * (double)val_get_c);
	rt -= (unsigned int)(const_val_get_p * (double)val_get_p);
	return rt;
}