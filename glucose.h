#ifndef __GLUCOSE_H__
#define __GLUCOSE_H__

#define BDMAX 14 /*Board: 15 * 15*/
#define WELCOMEMAX 50

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#ifndef LINUX
/* For Windows */
#define CLEAR system("cls")
#define PAUSE system("pause")
#else
/* For Linux */
#define CLEAR system("clear")
#define PAUSE getchar()
#endif

	void board_display(void);
	void board_init(void);
	void game_start_2p(void);
	void game_start_cp(void);
	/*int in_board(int x, int y);*/
	int game_judge(void);
	unsigned int value_shallow(int iCd, int x, int y, int breaker_en);
	unsigned int calc_shallow(int iCd, int breaker_en);
	unsigned int calc_breaker(int iCd, int x, int y, int Bd_cp[][BDMAX + 1]);
	unsigned int calc_deep(int iCd);
	unsigned int value_deep(int iCd, int x, int y, int debug_en);
	void fclear(FILE* fp);
	int get_path(char* prog_name);
	void rec_add(char* t);
	int save_file(void);
	int read_config(const char* config_name_got);
	int read_board(const char* board_name_got);

	extern int board[BDMAX + 1][BDMAX + 1]; /*Board[x][y]*/
	extern char player_index;    /*Player 1 or 2*/
	extern int ix0, iy0;
	extern int set_count;     /*Set count*/
	extern int game_mode;      /*1: 2Players; 2: 1Player&Computer, player first; 3: 1Player&Computer, computer first*/
	extern char debug_en;
	extern char noboard_en;
	extern unsigned int calc_depth;
	extern char* board_str[];
	extern unsigned int move_vert;
	extern unsigned int x_size;
	extern char* prog_path;
	extern char rec_buf[8 * (BDMAX + 1) * (BDMAX + 1)];

	extern unsigned int const_space;
	extern double const_val_get_c;
	extern double const_val_get_p;
	extern double const_val_pow;
	extern unsigned int const_val_added;

	extern const char* config_const_name[];

	extern char welcome[];

	enum Board_element {
		LUEDGE = 1, RUEDGE = 2, RLEDGE = 3, LLEDGE = 4, /* Left, right, upper, lower */
		LEDGE = 5, UEDGE = 6, REDGE = 7, OEDGE = 8, INNER = 9,
		PCHESS = 10, PNEW = 11, CCHESS = 20, CNEW = 21 /* Chess and new chess */
	};

#endif
