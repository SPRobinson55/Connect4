#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>

WINDOW *menu;

void renderMenuScreen(int *, int *, char (*)[13]);
void playGame(bool);
void getBoardDimensions(int *, int *);

int main()
{
	int ch;
	char options[3][13] = {"Singleplayer", "Multiplayer", "Exit"};
	int menuHeight, menuWidth;
	initscr();
	cbreak();
	renderMenuScreen(&menuHeight, &menuWidth, options);
	
	noecho();
	int i = 1;
	bool endGame = 0;
	while(endGame == 0)
	{
		curs_set(0);
		ch = wgetch(menu);
		switch(ch){
			case KEY_UP:
				mvwprintw(menu, (menuHeight * i) / 4, menuWidth / 3, "%s", options[i-1]);
				i--;
				if(i<1) i=3;
				wattron(menu, A_STANDOUT);
				mvwprintw(menu, (menuHeight * i) / 4, menuWidth / 3, "%s", options[i-1]);
				wattroff(menu, A_STANDOUT);
				break;
			case KEY_DOWN:
				mvwprintw(menu, (menuHeight * i) / 4, menuWidth / 3, "%s", options[i-1]);
				i++;
				if(i>3) i=1;
				wattron(menu, A_STANDOUT);
				mvwprintw(menu, (menuHeight * i) / 4, menuWidth / 3, "%s", options[i-1]);
				wattroff(menu, A_STANDOUT);
				break;
			case 'e':
				if(i==3) endGame = 1;
				else if (i==1) playGame(0);
				else playGame(1);
				clear();
				renderMenuScreen(&menuHeight, &menuWidth, options);
				i = 0;
				break;
		}
	}

	endwin();
	return 0;
}
//menu width and height are passed through as pointers because the loop that selects an item from the menu is in main and that loop requires these variables, which we determine when the menu is being rendered
void renderMenuScreen(int *menuHeight, int *menuWidth, char (*options)[13])
{
	wclear(menu);
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	*menuHeight = max_y/2;
	*menuWidth = max_x/3;
	clear();
	mvprintw(max_y/10, (max_x/2)-10, "Welcome to Connect 4");
	menu = newwin(*menuHeight, *menuWidth, (*menuHeight)/2, *menuWidth);
	box(menu, 0, 0);
	for(int i = 1; i <=3; i++)
	{
		if(i == 1)
			wattron(menu, A_STANDOUT);
		else
			wattroff(menu, A_STANDOUT);
		mvwprintw(menu, (*menuHeight * i) /4, *menuWidth/3, "%s", options[i-1]);
	}
	keypad(menu, TRUE);
	refresh();
	wrefresh(menu);
	return;
}

void playGame(bool multiplayer)
{
	clear();
	refresh();
	int numRows = 0;
	int numCols = 0;
	while(numRows < 4 || numCols < 4)
	{
		getBoardDimensions(&numRows, &numCols);
	}
	clear();
	refresh();

	WINDOW* selector;
	int ch;
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	int height = max_y/(numRows+2);
	int width = max_x/(numCols+2);
	int selHeight = (height/2 < 2) ? 2 : height/2;
	int xpos = width;
	int ypos = max_y - (selHeight);
	
	printw("Press 'q' to return to menu");
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	
	WINDOW *board[numCols][numRows];
	int i, j;
	for(i = 0; i < numCols; i++)
	{
		for(j = 0; j < numRows; j++)
		{
			board[i][j] = newwin(height, width, height * (j+1), width * (i+1));
			wattron(board[i][j], COLOR_PAIR(1));
			switch(height){
				case 1: 
					wborder(board[i][j], 32, 32, 32, 32, 91, 93, 91, 93);
					break;
				case 2:
					wborder(board[i][j], 32, 32, 32, 32, 0, 0, 0, 0);
					break;
				default:
					box(board[i][j], 0, 0);
					break;
			}
			wattroff(board[i][j], COLOR_PAIR(1));
			wrefresh(board[i][j]);
		}
	}
	noecho();
	refresh();

	selector = newwin(selHeight, width, ypos, xpos);
	box(selector, 0, 0);
	refresh();
	wrefresh(selector);
	
	keypad(selector, true);
	while((ch=wgetch(selector)) != 'q')
	{
		switch(ch){
			case KEY_RIGHT:
			xpos = xpos + width;
				if(xpos >= (max_x - (width * 2)))
					xpos = xpos - width;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			case KEY_LEFT:
				xpos = xpos - width;
				if(xpos <= 0)
					xpos = xpos + width;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			}
	}
	delwin(selector);
	endwin();
	return;
}

void getBoardDimensions(int *numRows, int *numCols)
{	
	WINDOW* dimensionWindow;
	echo();
	curs_set(1);
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	dimensionWindow = newwin(max_y/3, max_x*3/4, max_y/4, max_x/6);
	box(dimensionWindow, 0, 0);
	mvwprintw(dimensionWindow, 1, 1, "Enter the desired number of rows, then columns (at least 4).");
	mvwprintw(dimensionWindow, 2, 1, "very large boards may have trouble rendering.\n");
	wmove(dimensionWindow, 3, 1);
	char numRowstr[4];
	char numColstr[4];
	wgetstr(dimensionWindow, numRowstr);
	wmove(dimensionWindow, 4, 1);
	wgetstr(dimensionWindow, numColstr);
	*numRows = atoi(numRowstr);
	*numCols = atoi(numColstr);
	return;
}
