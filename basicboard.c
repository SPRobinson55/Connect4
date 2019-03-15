#include<ncurses.h>
#include<stdio.h>



//creates a 6x7 connect4 board with a selector box at the bottom of the screen controlled by the left and right arrowkeys
int main()
{
	int ch;
	WINDOW *selector;
	initscr();
	cbreak();
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	
	//each board slot should be scaled to the size of the window
	int height = max_y/6;
	int width = max_x/7;
	int xpos = 0;
	int ypos = max_y - (height/2);

	//create the selector box
	selector = newwin(height/2, width, ypos, xpos);
	box(selector, 0, 0);
	mvwprintw(selector, 2, 1, "THE BOX");
	refresh();
	wrefresh(selector);
	
	//create the board using a 2D array of windows.
	WINDOW *board[7][6];
	int i, j;
	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < 6; j++)
		{
			board[i][j] = newwin(height, width, height * j, width * i);
			box(board[i][j], 0, 0);
			wrefresh(board[i][j]);
		}
	}
	refresh();
	//enable keypad use for the selector and then read inputs to control the selector until the user inputs 'q'
	keypad(selector, true);
	while((ch=wgetch(selector)) != 'q')
	{
		switch(ch){
			case KEY_RIGHT: //erase previous selector position and move the selector 1 unit of width to the right
				xpos = xpos + width;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			case KEY_LEFT: //1 unit of width to the left
				xpos = xpos - width;
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
	
