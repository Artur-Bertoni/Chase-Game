/*
** ####################################
** ####  Chase & Evade base board  ####
** ####################################
** a tile environment for experimenting
** chasing and evading AI techniques.
* :::         part 1 of 2           :::
** by Prof. Filipo Mor - filipomor.com
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <winuser.h>

#define N            10
#define BlockedCell 178
#define FreeCell     32
#define DumbNPCCell 184
#define SmartNPCCell 64
#define PlayerCell  190

typedef struct _tile {
	bool Visible;
	bool Blocked;
} Tile;

typedef enum { DumbNPC = 0, Player = 1, SmartNPC = 2 } CharacterType;

typedef struct _character {
	CharacterType Type;
	bool Active;
	bool Alive;
	int Line;
	int Column;
	int Stamina;
} Character;

typedef struct {
    int x, y;
} Node;

typedef struct {
    Node node;
    int distance;
} QueueNode;

typedef struct {
    QueueNode nodes[N*N];
    int size;
} PriorityQueue;

void resetBoard(Tile Board[N][N], int NumberOfBlockedTiles, Character* characterList) {
	int line, column, k;

	// NPC's position
	int npcLine = characterList[0].Line;
	int npcColumn = characterList[0].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	// reset all tiles
	for (line = 0; line < N; line++) {
		for (column = 0; column < N; column++) {
			Board[line][column].Blocked = false;
			Board[line][column].Visible = true;
		}
	}

	// generate some random blocked tiles
	// ==>> why not switch it for a mazzle generator? ;-)
	for (k = 0; k < NumberOfBlockedTiles; k++) {
		do {
			line   = rand() % N;
			column = rand() % N;
		} while((line == 0 && column == 0) || (line == playerLine && column == playerColumn) || (line == npcLine && column == npcColumn));
		
		Board[line][column].Blocked = true;
	}

}

int countDistanceBetweenPlayerAndNPC(Character* characterList) {
	// NPC's position
	int npcLine = characterList[0].Line;
	int npcColumn = characterList[0].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	int lineDistance,
		columnDistance;

	if (npcLine > playerLine) {
		lineDistance = npcLine - playerLine;
	} else {
		lineDistance = playerLine - npcLine;
	}

	if (npcColumn > playerColumn) {
		columnDistance = npcColumn - playerColumn;
	} else {
		columnDistance = playerColumn - npcColumn;
	}

	return lineDistance + columnDistance;
}

void displayBoard(Tile Board[N][N], Character* characterList, int listNumber, bool EndGame) {
	int i, j;
	char m1 = FreeCell, m2 = FreeCell, m3 = FreeCell; // space

	if (EndGame) system("cls");

	// top border
	for (j = 0; j < N; j++) {
		switch (j) {
		case (0):    printf("%c%c%c%c%c", 201, 205, 205, 205, 203); break;
		case (N - 1):printf("%c%c%c%c", 205, 205, 205, 187); break;
		default:
			printf("%c%c%c%c", 205, 205, 205, 203);
			break;
		}
	}
	printf("\n");

	for (i = 0; i < N; i++) {
		printf("%c", 186);
		for (j = 0; j < N; j++) {
			m1 = FreeCell;
			m2 = FreeCell;
			m3 = FreeCell;

			if (Board[i][j].Blocked) {
				m1 = BlockedCell;
				m2 = BlockedCell;
				m3 = BlockedCell;
			}

			// intermediare column separators
			if (j > 0 && j <= (N - 1)) {
				printf("%c", 186);
			}

			/*
			** is there a character on the current position?
			*/
			int k;
			for (k = 0; k < listNumber; k++) {
				if (characterList[k].Line == i && characterList[k].Column == j) {
					m1 = FreeCell;
					if ((characterList[k].Line == characterList[0].Line && characterList[k].Line == characterList[1].Line)
					   && (characterList[k].Column == characterList[0].Column && characterList[k].Column == characterList[1].Column)) {
						m2 = DumbNPCCell;
					} else if ((characterList[k].Line == characterList[2].Line && characterList[k].Line == characterList[1].Line)
					   && (characterList[k].Column == characterList[2].Column && characterList[k].Column == characterList[1].Column)) {
						m2 = SmartNPCCell;
					} else if (characterList[k].Type == DumbNPC){
						m2 = DumbNPCCell;
					} else if (characterList[k].Type == SmartNPC) {
						m2 = SmartNPCCell;
					} else {
						m2 = PlayerCell;
					}
					m3 = FreeCell;
				}
			}
			printf("%c%c%c", m1, m2, m3);

		}
		printf("%c\n", 186);

		// intermediate lines
		if (i < (N - 1)) {
			for (j = 0; j < N; j++) {
				switch (j) {
				case (0):    printf("%c%c%c%c", 204, 205, 205, 205); break;
				case (N - 1):printf("%c%c%c%c%c", 206, 205, 205, 205, 185); break;
				default:
					printf("%c%c%c%c", 206, 205, 205, 205);
					break;
				}
			}
			printf("\n");
		}	
	}

	// bottom border
	for (j = 0; j < N; j++) {
		switch (j) {
		case (0):    printf("%c%c%c%c%c", 200, 205, 205, 205, 202); break;
		case (N - 1):printf("%c%c%c%c", 205, 205, 205, 188); break;
		default:
			printf("%c%c%c%c", 205, 205, 205, 202);
			break;
		}
	}
	printf("\n\n");

	if (!EndGame) {
		printf("Distance between player and NPC: %.2d\n\n", countDistanceBetweenPlayerAndNPC(characterList));
	} else {
		printf("GAME OVER! You're dead!\n\n");
	}
}

void gotoXY(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

bool dumbNpcMovement(Tile Board[N][N], Character* characterList) {
	// Dumb NPC's position
	int dumbNpcLine = characterList[0].Line;
	int dumbNpcColumn = characterList[0].Column;

	// Smart NPC's position
	int smartNpcLine = characterList[2].Line;
	int smartNpcColumn = characterList[2].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	// performing NPC's movement
	if (dumbNpcLine > playerLine //UP
		&& Board[dumbNpcLine-1][dumbNpcColumn].Blocked == false 
		&& !((dumbNpcLine - 1) == smartNpcLine && dumbNpcColumn == smartNpcColumn) 
		&& abs(dumbNpcLine - 1) < N 
		&& (dumbNpcLine - 1) >= 0)
			characterList[0].Line -= 1;
	else if (dumbNpcLine < playerLine //DOWN
		&& Board[dumbNpcLine+1][dumbNpcColumn].Blocked == false 
		&& !((dumbNpcLine + 1) == smartNpcLine && dumbNpcColumn == smartNpcColumn) 
		&& abs(dumbNpcLine + 1) < N)
			characterList[0].Line += 1;
	else if (dumbNpcColumn > playerColumn //LEFT
		&& Board[dumbNpcLine][dumbNpcColumn-1].Blocked == false 
		&& !(dumbNpcLine == smartNpcLine && (dumbNpcColumn - 1) == smartNpcColumn) 
		&& abs(dumbNpcColumn - 1) < N 
		&& (dumbNpcColumn - 1) >= 0)
			characterList[0].Column -= 1;
	else if (dumbNpcColumn < playerColumn  //RIGHT
		&& Board[dumbNpcLine][dumbNpcColumn+1].Blocked == false 
		&& !(dumbNpcLine == smartNpcLine && (dumbNpcColumn + 1) == smartNpcColumn) 
		&& abs(dumbNpcColumn + 1) < N)
			characterList[0].Column += 1;

	if (characterList[0].Line == characterList[1].Line && characterList[0].Column == characterList[1].Column) {
		characterList[1].Alive = false;
		return true;
	} else return false;
}

bool isValid(Tile Board[N][N], int x, int y) {
    return (x >= 0 && x < N && y >= 0 && y < N && !Board[x][y].Blocked);
}

bool dijkstra(Tile Board[N][N], Node start, Node end, Tile solution[N][N]) {
    int dist[N][N], i, j;
    bool visited[N][N];

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            dist[i][j] = INT_MAX;
            visited[i][j] = false;
            solution[i][j].Blocked = true;
        }
    }

    dist[start.x][start.y] = 0;

    PriorityQueue pq;
    pq.size = 0;

    QueueNode startNode = {start, 0};
    pq.nodes[pq.size++] = startNode;

    while (pq.size > 0) {
        QueueNode node = pq.nodes[0];

        pq.nodes[0] = pq.nodes[--pq.size];

        int parentIndex = 0;
        int childIndex = 1;

        while (childIndex < pq.size) {
            if (childIndex + 1 < pq.size && pq.nodes[childIndex + 1].distance < pq.nodes[childIndex].distance) {
                childIndex++;
            }

            if (pq.nodes[childIndex].distance < pq.nodes[parentIndex].distance) {
                QueueNode tmp = pq.nodes[parentIndex];
                pq.nodes[parentIndex] = pq.nodes[childIndex];
                pq.nodes[childIndex] = tmp;
                parentIndex = childIndex;
                childIndex = parentIndex * 2 + 1;
            } else {
                break;
            }
        }

        int x = node.node.x;
        int y = node.node.y;

        visited[x][y] = true;

        if (x == end.x && y == end.y) {
            return true;
        }

        if (dist[x][y] < node.distance) {
            continue;
        }

        if (isValid(Board, x - 1, y)) {
            if (!visited[x - 1][y]) {
                int alt = dist[x][y] + 1;

                if (alt < dist[x - 1][y]) {
                    dist[x - 1][y] = alt;

                    QueueNode newNode = {{x - 1, y}, alt};
                    pq.nodes[pq.size++] = newNode;
                }
            }
        }
		
		if (isValid(Board, x + 1, y)) {
		    if (!visited[x + 1][y]) {
		        int alt = dist[x][y] + 1;
		
		        if (alt < dist[x + 1][y]) {
		            dist[x + 1][y] = alt;
		
		            QueueNode newNode = {{x + 1, y}, alt};
		            pq.nodes[pq.size++] = newNode;
		        }
		    }
		}
		
		if (isValid(Board, x, y - 1)) {
		    if (!visited[x][y - 1]) {
		        int alt = dist[x][y] + 1;
		
		        if (alt < dist[x][y - 1]) {
		            dist[x][y - 1] = alt;
		
		            QueueNode newNode = {{x, y - 1}, alt};
		            pq.nodes[pq.size++] = newNode;
		        }
		    }
		}
	
		if (isValid(Board, x, y + 1)) {
		    if (!visited[x][y + 1]) {
		        int alt = dist[x][y] + 1;
		
		        if (alt < dist[x][y + 1]) {
		            dist[x][y + 1] = alt;
		
		            QueueNode newNode = {{x, y + 1}, alt};
		            pq.nodes[pq.size++] = newNode;
		        }
		    }
		}
		
		return false;
	}
}

 
Tile** pathFinder(Tile Board[N][N], Tile solution[N][N], Character* characterList) {
	int line, column;

	// Smart NPC's position
	int smartNpcLine = characterList[2].Line;
	int smartNpcColumn = characterList[2].Column;
	
	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	for (line = 0; line < N; line++)
		for (column = 0; column < N; column++)
			solution[line][column].Blocked = true;
			
	Node start = {smartNpcLine, smartNpcColumn};
	Node end = {playerLine,playerColumn};
	
    if (dijkstra(Board, start, end, solution) == false) 
		return NULL;

    return solution;
}
 
bool smartNpcMovement(Tile Board[N][N], Character* characterList, Tile solution[N][N]) {
	// Smart NPC's position
	int smartNpcLine = characterList[2].Line;
	int smartNpcColumn = characterList[2].Column;

	// solving the path and getting the solution
	pathFinder(Board, solution, characterList);

	// performing NPC's movement
	if (smartNpcLine > 0 && solution[smartNpcLine-1][smartNpcColumn].Blocked == false && Board[smartNpcLine-1][smartNpcColumn].Blocked == false) {
		characterList[2].Line -= 1;
	} else if (smartNpcLine < N-1 && solution[smartNpcLine+1][smartNpcColumn].Blocked == false && Board[smartNpcLine+1][smartNpcColumn].Blocked == false) {
		characterList[2].Line += 1;
	} else if (smartNpcColumn > 0 && solution[smartNpcLine][smartNpcColumn-1].Blocked == false && Board[smartNpcLine][smartNpcColumn-1].Blocked == false) {
		characterList[2].Column -= 1;
	} else if (smartNpcColumn < N-1 && solution[smartNpcLine][smartNpcColumn+1].Blocked == false && Board[smartNpcLine][smartNpcColumn+1].Blocked == false) {
		characterList[2].Column += 1;
	}
	
	gotoXY(0,30);
	displayBoard(solution, characterList, 3, false);
	
	// checking if NPC caught the player
	if (characterList[2].Line == characterList[1].Line && characterList[2].Column == characterList[1].Column) {
		characterList[1].Alive = false;
		return true;
	} else {
		return false;
	}
}

void playerMovement(Tile Board[N][N], Character* characterList) {
	fflush(stdin);

	// Dumb NPC's position
	int dumbNpcLine = characterList[0].Line;
	int dumbNpcColumn = characterList[0].Column;

	// Smart NPC's position
	int smartNpcLine = characterList[2].Line;
	int smartNpcColumn = characterList[2].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	if (GetAsyncKeyState (VK_UP)&1 || GetAsyncKeyState(0x57)) { // UP
		if (Board[playerLine-1][playerColumn].Blocked == false 
			&& !((playerLine - 1) == dumbNpcLine && playerColumn == dumbNpcColumn) 
			&& !((playerLine - 1) == smartNpcLine && playerColumn == smartNpcColumn) 
			&& abs(playerLine - 1) < N 
			&& (playerLine - 1) >= 0)
				characterList[1].Line -= 1;
	} else if (GetAsyncKeyState (VK_DOWN)&1 || GetAsyncKeyState(0x53)) { // DOWN
		if (Board[playerLine+1][playerColumn].Blocked == false 
			&& !((playerLine + 1) == dumbNpcLine && playerColumn == dumbNpcColumn) 
			&& !((playerLine + 1) == smartNpcLine && playerColumn == smartNpcColumn) 
			&& abs(playerLine + 1) < N)
				characterList[1].Line += 1;
	} else if (GetAsyncKeyState (VK_LEFT)&1 || GetAsyncKeyState(0x41)) { // LEFT
		if (Board[playerLine][playerColumn-1].Blocked == false 
			&& !(playerLine == dumbNpcLine && (playerColumn - 1) == dumbNpcColumn) 
			&& !(playerLine == smartNpcLine && (playerColumn - 1) == smartNpcColumn) 
			&& abs(playerColumn - 1) < N 
			&& (playerColumn - 1) >= 0)
				characterList[1].Column -= 1;
	} else if (GetAsyncKeyState (VK_RIGHT)&1 || GetAsyncKeyState(0x44)) { // RIGHT
		if (Board[playerLine][playerColumn+1].Blocked == false 
			&& !(playerLine == dumbNpcLine && (playerColumn + 1) == dumbNpcColumn) 
			&& !(playerLine == smartNpcLine && (playerColumn + 1) == smartNpcColumn) 
			&& abs(playerColumn + 1) < N)
				characterList[1].Column += 1;
	}
	
	fflush(stdin);
}

void loopMaster(Tile Board[N][N], Character* characterList) {
	bool EndGame = false;
	bool GameOver = false;
	int countToDumbNPCMovement = 0;
	int countToSmartNPCMovement = 0;
	Tile solution[N][N];

	system("cls");

	do {
		EndGame = GameOver == true ? true : false;

		if (!EndGame) gotoXY(0,0);
		else system("cls");

		displayBoard(Board, characterList, 3, EndGame);
		playerMovement(Board, characterList);

		if (countToSmartNPCMovement == 10) {
			GameOver = smartNpcMovement(Board, characterList, solution);
			countToSmartNPCMovement = 0;
		} else countToSmartNPCMovement++;
		
		if (countToDumbNPCMovement == 50) {
			GameOver = dumbNpcMovement(Board, characterList);
			countToDumbNPCMovement = 0;
		} else countToDumbNPCMovement++;
	} while(!EndGame);
}

int main() {
	Tile Board[N][N];
	srand(time(NULL));

	// character list
	Character* characterList = malloc(sizeof(Character) * 3);
	if (characterList == NULL) {
		printf("\nError allocating memory for game characters. Living now...\n");
		exit(EXIT_FAILURE);
	}

	// NPC (dumb)
	characterList[0].Active  = true;
	characterList[0].Alive   = true;
	characterList[0].Stamina = 10;
	characterList[0].Type    = DumbNPC;
	characterList[0].Line    = 0;
	characterList[0].Column  = 0;

	// NPC (smart)
	characterList[2].Active  = true;
	characterList[2].Alive   = true;
	characterList[2].Stamina = 10;
	characterList[2].Type    = SmartNPC;

	do {
		characterList[2].Line    = rand() % N;
		characterList[2].Column  = rand() % N;
	} while (characterList[2].Line == characterList[0].Line && characterList[2].Column == characterList[0].Column);
	
	// Player (hooman)
	characterList[1].Active  = true;
	characterList[1].Alive   = true;
	characterList[1].Stamina = 10;
	characterList[1].Type    = Player;
	
	do {
		characterList[1].Line    = rand() % N;
		characterList[1].Column  = rand() % N;
	} while ((characterList[1].Line == characterList[0].Line && characterList[1].Column == characterList[0].Column)
			|| (characterList[1].Line == characterList[2].Line && characterList[1].Column == characterList[2].Column));

	resetBoard(Board, 15, characterList);
	loopMaster(Board, characterList);

	return (0);
}
