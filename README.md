# Connect4


Made by Scott Robinson.

The game requires the ncurses library.

The game supports singleplayer and multiplayer mode, which are selected from a menu screen.
The game prompts the user for board size before beginning the game.
Multiple games can be played in a row and the score is kept.

The game checks for winning board states and determines the next best move using a graph of verticies
each vertex points to a node which stores a color and pointers to adjacent nodes.
The game uses adjacent node pointers to traverse the graph and check for a win or determine the next best move.


Github: https://github.com/SPRobinson55/Connect4.git
