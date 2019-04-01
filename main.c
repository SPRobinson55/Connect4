#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>


WINDOW *menu;
WINDOW *scoreboard;
int p1Score = 0;
int p2Score = 0;
int numRows = 0;
int numCols = 0;

struct Node{
	int color;
	struct Node* up;
	struct Node* down;
	struct Node* left;
	struct Node* right;
	struct Node* uleft;
	struct Node* uright;
	struct Node* dleft;
	struct Node* dright;
};

struct Vertex{
	//each vertex points to a node containing information about that slot's color and adjacent slots
	struct Node* slot;
};

struct Graph{
	//the graph holds an array of verticies, each representing a board slot
	//it's like an adjacency list except it stores position too
	int V;
	struct Vertex* array;
};

struct Node* createNode()
{
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node)); 
	newNode->color = 0;
	newNode->up = NULL;
	newNode->down = NULL;
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->uleft = NULL;
	newNode->uright = NULL;
	newNode->dleft = NULL;
	newNode->dright = NULL;
	return newNode;
}

struct Graph* createBoard(int numCols, int numRows)
{
	//first we need to allocate memory for the graph and it's 
	struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
	int verticies = numRows * numCols;
	graph->V = verticies;

	graph->array = (struct Vertex*)malloc(verticies * sizeof(struct Vertex));
	//now we need to create our board of empty slots and link the adjacent ones.
	for(int i = 0; i < verticies; i++)
	{
		graph->array[i].slot = createNode();
		if(i!=0)
		{
			
		if((i % numRows) != 0)
		{
			graph->array[i].slot->up = graph->array[i-1].slot;
			graph->array[i-1].slot->down = graph->array[i].slot;
		}
		if(i>=numRows) 
		{
			graph->array[i].slot->left = graph->array[i-numRows].slot;
			graph->array[i-numRows].slot->right = graph->array[i].slot;
			if((i % numRows) != 0)
			{
				graph->array[i].slot->uleft = graph->array[i-1-numRows].slot;
				graph->array[i-1-numRows].slot->dright = graph->array[i].slot;
			}
			if((i % numRows) != numRows-1)
			{
				graph->array[i].slot->dleft = graph->array[i+1-numRows].slot;
				graph->array[i+1-numRows].slot->uright = graph->array[i].slot;
			}
		}
		
		}
	}
	return graph;
}
void renderMenuScreen(int *, int *, char (*)[13]);
int playGame(bool, bool);
int checkForWin(int , int , int maxRows, struct Graph*);
int findBestSlot(int maxRows, struct Graph*);
int findMaxSameAdj(int , int , int maxRows, struct Graph*);
void getBoardDimensions();


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
	int repeat = 0;
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
				switch(i){
					case 1:
						repeat = playGame(0,0);
						while(repeat == 1)
							repeat = playGame(0,1);
						break;
					case 2:
						repeat = playGame(1,0);
						while(repeat == 1)
							repeat = playGame(1,1);
						break;
					case 3:
						endGame = 1;
						break;}
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
	printw("Use the arrow keys to select");
	mvprintw(1,0, "Press 'e' to select an option");
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

int playGame(bool multiplayer, bool repeat)
{
	clear();
	refresh();
	if(repeat == 0){
		numRows = 0;
		numCols = 0;
		p1Score = 0;
		p2Score = 0;
	}
	while(numRows < 4 || numCols < 4)
		getBoardDimensions();

	WINDOW* selector;
	int ch;
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);
	int height = max_y/(numRows+2);
	int width = max_x/(numCols+1);
	int selHeight = 2;
	int xpos = 3;
	int ypos = max_y - 3;

	//every time we play a new game we have to update the score	
	scoreboard = newwin(height, max_x/2, 0, max_x/2);
	wprintw(scoreboard, "P 1 Score: %d", p1Score);
	mvwprintw(scoreboard, 1, 0, "P 2 Score: %d", p2Score);
	wrefresh(scoreboard);

	printw("Press 'q' to return to menu || press 'e' to place chip"); 
	mvprintw(1, 0, "Use the arrow keys to move the selector");
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	

	//we need a 2D array of windows to render each board slot
	WINDOW *boardWindows[numCols][numRows];
	int i, j;
	
	//now we render our empty board
	for(i = 0; i < numCols; i++)
	{
		for(j = 0; j < numRows; j++)
		{
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

	int h;

	//now we make our empty board and start getting input to move the selector and place chips
	struct Graph* boardVerticies = createBoard(numCols, numRows);
	
	curs_set(0);
	keypad(selector, true);
	while(ch=wgetch(selector))
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
				//calculate the height, h, of the lowest empty slot in the column above the selector
				h = numRows - 1;
				struct Node* cur = boardVerticies->array[((columnNum+1)*numRows)-1].slot;
				while(cur->color != 0)
				{
					h--;
					if(cur->up == NULL)
					{
						if(cur->color != 0) h--;
						break;
					}
					cur = cur->up;
				}
				if(h < 0) break; //the column is full, can't place a chip
				//now we place the chip in the determined slot
				switch (playerTurn) {
					case 1: 
						wattron(boardWindows[columnNum][h], COLOR_PAIR(2));
						boardVerticies->array[columnNum*numRows+h].slot->color = 1;
						playerTurn = 2;
						break;
					case 2:
						wattron(boardWindows[columnNum][h], COLOR_PAIR(3));
						boardVerticies->array[columnNum*numRows+h].slot->color = 2;
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
				switch(checkForWin(columnNum, h, numRows, boardVerticies)){
					case 0:
						break;
					case 1:
						p1Score++;
						mvprintw(2, max_x/4, "Player1 wins, press any key to reset");
						refresh();
						getch();
						delwin(scoreboard);
						return 1;
						break;
					case 2:
						p2Score++;
						mvprintw(2, max_x/4, "Player2 wins, press any key to reset ");
						refresh();
						getch();
						delwin(scoreboard);
						return 1;
						break;
					default:
						break;
				}
				//now if it's singleplayer mode we need to make a turn for the AI because player1 has just taken his turn
				if(multiplayer == 0)
				{
					columnNum = findBestSlot(numRows, boardVerticies);
					h = numRows - 1;
					struct Node* cur = boardVerticies->array[((columnNum+1)*numRows)-1].slot;
					while(cur->color != 0)
					{
						h--;
						if(cur->up == NULL)
							break;
						cur = cur->up;
					}
					wattron(boardWindows[columnNum][h], COLOR_PAIR(3));
					boardVerticies->array[columnNum*numRows+h].slot->color = 2;
					playerTurn = 1;
					
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

					if(checkForWin(columnNum, h, numRows, boardVerticies) == 2)
					{
						p2Score++;
						mvprintw(2, max_x/4 ,"Player2 wins, press any key to reset ");
						refresh();
						getch();
						delwin(scoreboard);
						return 1;
					}
				}

				break;

			case 'q':
				return 0;
				break;

			default:
				break;
			}
	}
	delwin(selector);
	return 0;
}

int checkForWin(int column, int row, int maxRows, struct Graph* board)
{
	int oCol = column;
	int oRow = row;
	struct Node* origin = board->array[column*maxRows + row].slot;
	struct Node* cur = origin;

	int c = cur->color;
	int inaRow = 1;
	//we need to check the 7 adjacent slots to our current slot (don't need top because if we just placed it there then there's no chip on top of it.
	int a = 0;
	//we only need to check below for vertical wins
	while(cur->down != NULL && a < 3)
	{
		if(cur->down->color == c) inaRow++;
		else break;
		cur = cur->down;
		a++;
	}
	if(inaRow == 4) return c;

	//now we check the 7 nodes that form a horizontal line with the origin node at the center, if we find 4 same color chips in a row we have found a win
	inaRow = 1;
	cur = origin;
	a = 0;
	while(cur->left != NULL && a < 3)
	{
		if(cur->left->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->left;
		a++;
	}
	cur = origin;
	a = 0;
	while(cur->right != NULL && a < 3)
	{
		if(cur->right->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->right;
		a++;
	}
	

	//now we check the upleft-downright diagonal line the same way we checked the horizonal
	inaRow = 1;
	cur = origin;
	a = 0;
	while(cur->uleft != NULL && a < 3)
	{
		if(cur->uleft->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->uleft;
		a++;
	}
	cur = origin;
	a = 0;
	while(cur->dright != NULL && a < 3)
	{
		if(cur->dright->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->dright;
		a++;
	}


	//and now the upright-downleft diagonal
	inaRow = 1;
	cur = origin;
	a = 0;
	while(cur->dleft != NULL && a < 3)
	{
		if(cur->dleft->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->dleft;
		a++;
	}
	cur = origin;
	a = 0;
	while(cur->uright != NULL && a < 3)
	{
		if(cur->uright->color == c)
		{
			inaRow++;
			if(inaRow==4) return c;
		}
		else break;
		cur = cur->uright;
		a++;
	}

	return 0; //you didn't find any 4 in a rows :P
}

int findBestSlot(int maxRows, struct Graph* board)
{
	int maxInaRows[numCols];
	int trueMax = -1000;
	int bestSlot[numCols];
	int position = 0;
	for(int i = 0; i < numCols; i++)
	{
		int h = numRows-1;
		struct Node* cur = board->array[((i+1)*numRows)-1].slot;
		while(cur->color != 0)
		{
			h--;
			if(cur->up == NULL)
			{
				if(cur->color != 0) h--;
				break;
			}
			cur = cur->up;
		}
		if(h < 0) // the column is full, we can't place it there
			maxInaRows[i] = -50;
		else
		{
			maxInaRows[i] = findMaxSameAdj(i, h, numRows, board);
			//I don't want to place a chip that will give my opponent a win, but I don't care if placing that chip wins the game for me
			if(maxInaRows[i] != 150)
				if(h > 0)
					if(findMaxSameAdj(i, h-1, numRows, board) == 100)
						maxInaRows[i] = -5;
		}
		if(maxInaRows[i] > trueMax)
		{
			trueMax = maxInaRows[i];
			for(int a = 0; a < numCols; a++)
				bestSlot[a] = 0;
			position = 0;
			bestSlot[position] = i;
		}
		else if(maxInaRows[i] == trueMax)
		{
			position++;
			bestSlot[position] = i;
		}
	}
	int trueBestSlot = rand() % (position+1);
	return bestSlot[trueBestSlot];

}

int findMaxSameAdj(int column, int row, int maxRows, struct Graph* board)
{

	//I want to select a slot to place our chip based on the number of in-a-rows we add to/block.
	//more adjacent same color lines means the slot is more highly contested
	//if I detect 3 in a row of either color adjacent to the slot I'm checking, I NEED to place a chip there, because it either secures a victory or prevents an opponent victory
	//I should prefer blue 3 in a rows over red 3 in a rows because I shouldn't block when I can just win.
	//
	//I can't use a standard depth first search because 4-in-a rows can only be created linearly. so changing direction mid search bunks everyting up
	//
	//
	int trueMax = 0;
	int c;
	int max = 0;
	struct Node* origin = board->array[column*maxRows + row].slot;
	struct Node* cur = origin;

	int a = 0;
	int inaRow = 0;
	while(cur->down != NULL && a < 3)
	{
		if(a == 0) 
		{
			c = cur->down->color;
			if(c == 0) break;
		}
		if(cur->down->color == c){ 
			max++;
			inaRow++;
		}
		else break;
		cur = cur->down;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}
	trueMax = max;
	max = 0;
	inaRow = 0;

	cur = origin;
	a = 0;
	while(cur->left != NULL && a < 3)
	{
		if(a == 0) 
		{
			c = cur->left->color;
			if(c == 0) break;
		}
		if(cur->left->color == c) {
			max++;
			inaRow++;
		}
		else break;
		cur = cur->left;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}
	cur = origin;
	a = 0;
	while(cur->right != NULL && a < 3)
	{
		if(a == 0)
		{
			if(cur->right->color != c) inaRow = 0;	
			c = cur->right->color;
			if(c==0) break;
		}
		if(cur->right->color == c) {
			max++;
			inaRow++;
			if(inaRow == 3)
			{
				if(c==2) return 150;
				else return 100;
			}
		}
		else break;
		cur = cur->right;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}
	inaRow = 0;
	if(max>=trueMax) trueMax = max;
	max = 0;


	cur = origin;
	a = 0;
	while(cur-> dright != NULL && a < 3)
	{
		if(a == 0)
		{
		       c = cur->dright->color;
		       if(c == 0) break;
		}
		if(cur->dright->color == c) {
			max++;
			inaRow++;
		}
		else break;
		cur = cur->dright;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}

	cur = origin;
	a = 0;
	while(cur->uleft != NULL && a < 3)
	{
		if(a == 0)
		{
			if(cur->uleft->color) inaRow = 0;
			c = cur->uleft->color;
			if(c == 0) break;
		}
		if(cur->uleft->color == c) {
			max++;
			inaRow++;
			if(inaRow == 3)
			{
				if(c==2) return 150;
				else return 100;
			}
		}
		else break;
		cur = cur->uleft;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}	
	inaRow = 0;
	if(max>=trueMax) trueMax = max;
	max = 0;


	cur = origin;
	a = 0;
	while(cur->dleft != NULL && a < 3)
	{
		if(a == 0)
		{
			c = cur->dleft->color;
			if(c == 0) break;
		}
		if(cur->dleft->color == c) {
			max++;
			inaRow++;
		}
		else break;
		cur = cur->dleft;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}

	cur = origin;
	a = 0;
	while(cur->uright != NULL && a < 3)
	{
		if(a == 0)
		{
			if(cur->uright->color != c) inaRow = 0;
			c = cur->uright->color;
			if(c == 0) break;
		}
		if(cur->uright->color == c) {
			max++;
			inaRow++;
			if(inaRow == 3)
			{
				if(c==2) return 150;
				else return 100;
			}
		}
		else break;
		cur = cur->uright;
		a++;
	}
	if(inaRow == 3)
	{	
		if(c==2) return 150;
		else return 100;
	}
	if(max>=trueMax) trueMax = max;
	return trueMax;
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
	clear();
	refresh();
	return;
}
