#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>

//begins with a menu screen in which the player selects singleplayer, multiplayer(not yet implemented), or exit
int goMenuScreen();


int main()
{
	int ch;
	WINDOW *selector;
	WINDOW* dimensionWindow;
	int max_x, max_y;
	initscr();
	cbreak();
	curs_set(0);
	
	int selection = goMenuScreen();
	if(selection==3)
	{
		endwin();
		return 0;
	}
	//prompts the user for row and height dimensions and reads those values into numRows and numCols;
	clear();
	refresh();
	curs_set(1);
	getmaxyx(stdscr, max_y, max_x);
	dimensionWindow = newwin(max_y/3, max_x*3/4, max_y/4, max_x/6);
	box(dimensionWindow, 0, 0);
	mvwprintw(dimensionWindow, 1, 1, "Enter the desired number of rows, then columns.");
	mvwprintw(dimensionWindow, 2, 1, "very large boards may have trouble rendering.\n");
	wmove(dimensionWindow, 3, 1);
	char numRowstr[4];
	char numColstr[4];
	echo();
	wgetstr(dimensionWindow, numRowstr);
	wmove(dimensionWindow, 4, 1);
	wgetstr(dimensionWindow, numColstr);
	int numRows = atoi(numRowstr);
	int numCols = atoi(numColstr);

	//cell height and width is scaled to max window size
	//generates the board with at least one height unit of free space above and below the board and one width unit of free space to the sides of the board.
	//also creates a selector at the bottom of the screen which the player controls with right and left arrow keys to decide which column to drop their piece.
	clear();
	curs_set(0);
	int height = max_y/(numRows+2);
	int width = max_x/(numCols+2);
	int xpos = width;
	int selHeight = (height/2 < 2) ? 2 : height/2;
	int ypos = max_y - (selHeight);
	printw("Press 'q' to quit");
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	selector = newwin(selHeight, width, ypos, xpos);
	box(selector, 0,0);
	refresh();
	wrefresh(selector);
	
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
	return 0;
}

int goMenuScreen()
{
	WINDOW *menu;
	int ch;
	char options[3][13] = {"Singleplayer", "Multiplayer", "Exit"};
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	int muHeight = max_y/2;
	int muWidth = max_x/3;

	clear();
	mvprintw(max_y/10, (max_x/2)-10, "Welcome to Connect 4");
	menu = newwin(muHeight, muWidth, max_y/4, max_x/3);
	box(menu, 0, 0);
	for(int i = 1; i <= 3; i++)
	{
		if(i == 1)
			wattron(menu, A_STANDOUT);
		else
			wattroff(menu, A_STANDOUT);
		mvwprintw(menu, (muHeight*i)/4, muWidth/3, "%s", options[i-1]);
	}
	refresh();	
	wrefresh(menu);

	noecho();
	keypad(menu, TRUE);
	int i = 0;
	while((ch=wgetch(menu)) != 'q')
	{
		mvwprintw(menu, (muHeight * i) / 4, muWidth/3, "%s", options[i-1]);
		switch(ch){
			case KEY_UP:
				i--;
				if(i<1) i=3;
				break;
			case KEY_DOWN:
				i++;
				if(i>3) i=1;
				break;
			case 'e':
				return i;
				break;
		}
		wattron(menu, A_STANDOUT);
		mvwprintw(menu, (muHeight * i) / 4, muWidth/3, "%s", options[i-1]);
		wattroff(menu, A_STANDOUT);
	}
	return 1;
}

