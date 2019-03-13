#include<ncurses.h>
#include<stdio.h>




int main()
{
	int ch;
	WINDOW *selector;
	initscr();
	cbreak();
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	int height = max_y/6;
	int width = max_x/7;
	int xpos = 0;
	int ypos = max_y - (height/2);
	selector = newwin(height/2, width, ypos, xpos);
	box(selector, 0, 0);
	mvwprintw(selector, 2, 1, "THE BOX");
	refresh();
	wrefresh(selector);
	
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
	
	keypad(selector, true);
	while((ch=wgetch(selector)) != 'q')
	{
		switch(ch){
			case KEY_RIGHT:
				xpos = xpos + width;
				werase(selector);
				wrefresh(selector);
				mvwin(selector, ypos, xpos);
				box(selector, 0, 0);
				wrefresh(selector);
				refresh();
				break;
			case KEY_LEFT:
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
	
