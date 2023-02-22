Use of the C file:
- To execute this program, you must give the following arguments: 
    - path of the C file.
    - name and extension of the .txt file where the program will read the movements.
    - name and extension of the .txt file where the program will print the table if it was an incomplete game.
- The file format of the .txt file must have the following order:
    - Name of the players, followed by the assigned color.
    - Color that started the game.
    - Movements made by the players.
An example of a .txt file is:
    Alejandro,B
    Federico,N
    B
    D6
    C4
    G5

The first two lines indicate the name and color of the players. The third line indicates the color that started the game.
The rest of the lines are the movements made by the players.
An example of command to run the program is: gcc index.c & a.exe movimientos.txt salida.txt

Use of the Python file:
- To execute this program, you must give the following arguments:
    - path of the Python file.
    - name and extension of the .txt file where the program will read the table and the next player.
    - color of your pieces.
    - difficulty of the bot (0 or 1).
- The file format of the .txt file is the same as the output of the C program.
An example of command to run the program is: python othelloBot.py salida.txt N 0