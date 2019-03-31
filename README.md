# Connect4


Made by Scott Robinson. 

The game requires the ncurses library. 

The game supports singleplayer and multiplayer mode, which are selected from a menu screen. 
The game prompts the user for board size before beginning the game. 
Multiple games can be played in a row and the score is kept. 

The game checks for winning board states and determines the next best move using a graph of verticies. 
each vertex points to a node which stores a color and pointers to adjacent nodes. 
The game uses adjacent node pointers to traverse the graph and check for a win or determine the next best move. 

The singleplayer vs. computer algorithm determines the next best move by, firstly, determining every slot it could possibly place a chip.
Then, the algorithm calculates the number of adjacent in-a-row nodes of either color. Thereby prioritizing nodes that complete longer strings of consecutive same color chips.
The algorithm prioritizes nodes that result in a win for either player, always choosing to complete a 4-in-a-row or block an opponents 4-in-a-row. It prioritizes winning over blocking so it doesn't throw a victory to block.
The algorithm also avoids placing a chip in a slot that would allow the opponent to complete a 4-in-a-row, so it doesn't give away wins.


Github: https://github.com/SPRobinson55/Connect4.git  
