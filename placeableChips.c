#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>


WINDOW *menu;
int p1Score = 0;
int p2Score = 0;

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
				i = 1;
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
	int width = max_x/(numCols+1);
	int selHeight = 2;
	int xpos = 3;
	int ypos = max_y - 3;
	
	printw("Press 'q' to return to menu");
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	
	//we need a 2D array of windows to render each board slot
	//we need another 2D array to save the colors of each node so we can determine if a winning game state has occurred
	WINDOW *boardWindows[numCols][numRows];
	int boardColors[numCols][numRows];
	int i, j;
	for(i = 0; i < numCols; i++)
	{
		for(j = 0; j < numRows; j++)
		{
			//initialize all colors to 0 to indicate empty slot
			boardColors[i][j] = 0;
			boardWindows[i][j] = newwin(height, width, (height*j) + 3, (width*i) + 3);
			wattron(boardWindows[i][j], COLOR_PAIR(1));
			//this switch handles rendering of smol nodes
			switch(height){
				case 1: 
					wborder(boardWindows[i][j], 32, 32, 32, 32, 91, 93, 91, 93);
					break;
				case 2:
					wborder(boardWindows[i][j], 32, 32, 32, 32, 0, 0, 0, 0);
					break;
				default:
					box(boardWindows[i][j], 0, 0);
					break;
			}
			wattroff(boardWindows[i][j], COLOR_PAIR(1));
			wrefresh(boardWindows[i][j]);
		}
	}
	noecho();
	refresh();
	
	//we need a selector so the player can choose where to drop their piece
	selector = newwin(selHeight, width, ypos, xpos);
	box(selector, 0, 0);
	refresh();
	wrefresh(selector);

	//we need some color pairs to render placed chips
	//and some variables to keep track of who's turn it is and player score
	init_pair(2, COLOR_RED, COLOR_RED);
	init_pair(3, COLOR_BLUE, COLOR_BLUE);
	int playerTurn = 0;
	
	curs_set(0);
	keypad(selector, true);
	while((ch=wgetch(selector)) != 'q')
	{
		//calculate the selector's position
		int columnNum = (xpos-3)/width;
		//read an input to move the selector or place a chip
		switch(ch){
			case KEY_RIGHT:
				//as long as the selector is only incremented in units of 'width' the column num stays an integer
				xpos = xpos + width;
				if(xpos >= (width*numCols) + 3)
					xpos = width*(numCols-1) + 3;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			case KEY_LEFT:
				xpos = xpos - width;
				if(xpos <= 3)
					xpos = 3;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			case 'e':
				printw("%d ", columnNum);
				//calculate the height, h, of the lowest empty slot in the column above the selector
				int i = 0;	
				while(boardColors[columnNum][numRows-1-i]!=0) i++;
				int h = numRows-1-i;
				if (h < 0) break;
				switch (playerTurn) {
					case 0: 
						wattron(boardWindows[columnNum][h], COLOR_PAIR(2));
						boardColors[columnNum][h] = 1;
						playerTurn = 1;
						break;
					case 1:
						wattron(boardWindows[columnNum][h], COLOR_PAIR(3));
						boardColors[columnNum][h] = 2;
						playerTurn = 0;
						break;
					default:
						playerTurn = 0;
						break;
				}
				//if the nodes have 2 height or less then they don't have upper borders to fit the color inside
				if(height <= 2)
					for(int i = 0; i < height; i++)
						for(int j = 1; j < width-1; j++)
							mvwprintw(boardWindows[columnNum][h], i, j, "X");
				for(int i = 1; i < height-1; i++)
					for(int j = 1; j < width-1; j++)
						mvwprintw(boardWindows[columnNum][h], i, j, "X"); 
				wrefresh(boardWindows[columnNum][h]);
				refresh();
				break;
			default:
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
	//we get the max x and max y each time we ask for board dimensions in case the window size has changed since the last played game.
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	dimensionWindow = newwin(max_y/3, max_x*3/4, max_y/4, max_x/6);
	box(dimensionWindow, 0, 0);
	mvwprintw(dimensionWindow, 1, 1, "Enter the desired number of rows, then columns (at least 4).");
	mvwprintw(dimensionWindow, 2, 1, "very large boards may have trouble rendering.\n");
	wmove(dimensionWindow, 3, 1);
	//we don't need arrays larger than 4 elements because the max_x and max_y of our window will never be larger than 5 digits
	char numRowstr[4];
	char numColstr[4];
	wgetstr(dimensionWindow, numRowstr);
	wmove(dimensionWindow, 4, 1);
	wgetstr(dimensionWindow, numColstr);
	*numRows = atoi(numRowstr);
	*numCols = atoi(numColstr);
	return;
}
