#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>


WINDOW *menu;
WINDOW *scoreboard;
int p1Score = 0;
int p2Score = 0;
int numRows = 0;
int numCols = 0;

void renderMenuScreen(int *, int *, char (*)[13]);
void playGame(bool, bool);
void getBoardDimensions();
int checkForWin(int , int , int maxRows, int (*)[maxRows]);

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
				else if (i==1) playGame(0,0);
				else playGame(1,0);
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

void playGame(bool multiplayer, bool repeat)
{
	clear();
	refresh();
	if(repeat == 0){
		numRows = 0;
		numCols = 0;
	}
	while(numRows < 4 || numCols < 4)
		getBoardDimensions();
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
	int playerTurn = 1;
	
	curs_set(0);
	keypad(selector, true);
	while((ch=wgetch(selector)) != 'q')
	{
		int i = 0;
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
				//calculate the height, h, of the lowest empty slot in the column above the selector
				i = 0;
				while(boardColors[columnNum][numRows-1-i] != 0) i++;
				int h = numRows-1-i;
				if(h<0) break; //the column is full, can't place a chip
				//now we place the chip in the determined slot
				switch (playerTurn) {
					case 1: 
						wattron(boardWindows[columnNum][h], COLOR_PAIR(2));
						boardColors[columnNum][h] = 1;
						playerTurn = 2;
						break;
					case 2:
						wattron(boardWindows[columnNum][h], COLOR_PAIR(3));
						boardColors[columnNum][h] = 2;
						playerTurn = 1;
						break;
					default:
						playerTurn = 1;
						break;
				}
				//if the nodes have 2 height or less then they don't have upper borders to fit the color inside
				if(height <= 2)
					for(int i = 0; i < height; i++)
						for(int j = 1; j < width-1; j++)
							mvwprintw(boardWindows[columnNum][h], i, j, "X");
				else
				for(int i = 1; i < height-1; i++)
					for(int j = 1; j < width-1; j++)
						mvwprintw(boardWindows[columnNum][h], i, j, "X"); 
				wrefresh(boardWindows[columnNum][h]);
				refresh();
				
				//now we need to check to see if the chip that was just placed resulted in a 4-in-a-row
				switch(checkForWin(columnNum, h, numRows, boardColors)){
					case 0:
						break;
					case 1:
						p1Score++;
						printw("P1 wins ");
						refresh();
						break;
					case 2:
						p2Score++;
						printw("P2 wins ");
						refresh();
						break;
					default:
						break;
				}

				break;

			default:
				break;
			}
	}
	delwin(selector);
	return;
}

int checkForWin(int column, int row, int maxRows, int (*bColors)[maxRows])
{
	int oCol = column;
	int oRow = row;
	int c = bColors[column][row];
	int inaRow = 1;
	//we need to check the 8 adjacent slots to our current slot
	//let's check the horizontal and vertical first because those are easier
	int i = row + 1;
	int a = 0;
	//we only need to check below for vertical wins
	while(i <= numRows - 1 && a < 3)
	{
		if(bColors[column][i] == c) inaRow++;
		else break;
		i++;
		a++;
	}
	if(inaRow == 4) return c;

	inaRow = 1;
	i = column;
	for(a = 0; a < 3; a++)
		if(i>0) i--;
	a = 0;
	while(i<=numCols-1 && a < 7)
	{
		if(bColors[i][row] == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else inaRow = 0;
		i++;
		a++;
	}

	inaRow = 1;
	if(column>0 && row>0) //we can check up left
		if(bColors[column-1][row-1] == c)
		{
			column--;
			row--;
			inaRow++;
			if(column>0 && row>0)
				if(bColors[column-1][row-1] == c)
				{
					column--;
					row--;
					inaRow++;
					if(column>0 && row>0)
						if(bColors[column-1][row-1] == c)
							inaRow++;
				}
		}
	column = oCol;
	row = oRow;
	if(column < numCols-1 && row < numRows-1)//we can check bottom right, in line with up left
		if(bColors[column+1][row+1] == c)
		{
			column++;
			row++;
			inaRow++;
			if(column < numCols-1 && row < numRows-1)
				if(bColors[column+1][row+1] == c)
				{
					column++;
					row++;
					inaRow++;
					if(column < numCols-1 && row < numRows-1)
						if(bColors[column+1][row+1] == c)
							inaRow++;
				}
		}
	if(inaRow>=4) return c;

	inaRow = 1;
	//now we do the same with the other diagonal
	if(column > 0 && row <= numRows-1) //bottom left
		if(bColors[column-1][row+1] == c)
		{
			column--;
			row++;
			inaRow++;
			if(column > 0 && row <= numRows-1)
				if(bColors[column-1][row+1] == c)
				{
					column--;
					row++;
					inaRow++;
					if(column > 0 && row <= numRows-1)
						if(bColors[column-1][row+1] == c)
							inaRow++;
				}
		}
	column = oCol;
	row = oRow;
	if(column <= numCols-1 && row > 0) //up right
		if(bColors[column+1][row-1] == c)
		{
			column++;
			row--;
			inaRow++;
			if(column <= numCols-1 && row > 0)
				if(bColors[column+1][row-1] == c)
				{
					column++;
					row--;
					inaRow++;
					if(column <= numCols-1 && row > 0)
						if(bColors[column+1][row-1] == c)
							inaRow++;
				}
		}
	if(inaRow>=4) return c;
	return 0; //you didn't find any 4 in a rows :P
}

void getBoardDimensions()
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
	numRows = atoi(numRowstr);
	numCols = atoi(numColstr);
	return;
}
