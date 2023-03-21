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

#define N            10
#define BlockedCell 178
#define FreeCell     32
#define NPCCell     184
#define PlayerCell  190

typedef struct _tile {
	bool Visible;
	bool Blocked;
} Tile;

typedef enum { NPC = 0, Player = 1 } CharacterType;

typedef struct _character {
	CharacterType Type;
	bool Active;
	bool Alive;
	int Line;
	int Column;
	int Stamina;
} Character;

void resetBoard(Tile Board[N][N], int NumberOfBlockedTiles) {
	int line, column, k;

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
		} while(line == 0 && column == 0);
		
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

	return (lineDistance + columnDistance) - 1;
}

void displayBoard(Tile Board[N][N], Character* characterList, int listNumber) {
	int i, j;
	char m1 = FreeCell, m2 = FreeCell, m3 = FreeCell; // space

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
					m2 = (characterList[k].Type == NPC ? NPCCell : PlayerCell);
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

	printf("Shorter distance between player and NPC: %d\n\n", countDistanceBetweenPlayerAndNPC(characterList));
}

void gotoXY(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void npcMovement(Tile Board[N][N], Character* characterList) {
	// NPC's position
	int npcLine = characterList[0].Line;
	int npcColumn = characterList[0].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	// performing NPC's movement
	if (npcLine > playerLine && Board[npcLine-1][npcColumn].Blocked != true) {
		characterList[0].Line -= 1;
	} else if (npcLine < playerLine && Board[npcLine+1][npcColumn].Blocked != true) {
		characterList[0].Line += 1;
	} else if (npcColumn > playerColumn && Board[npcLine][npcColumn-1].Blocked != true) {
		characterList[0].Column -= 1;
	} else if (npcColumn < playerColumn && Board[npcLine][npcColumn+1].Blocked != true) {
		characterList[0].Column += 1;
	}
}

void loopMaster(Tile Board[N][N], Character* characterList) {
	do {
		gotoXY(0,0);
		displayBoard(Board, characterList, 2);
		npcMovement(Board, characterList);
		Sleep(1000);
	} while(true);
}

int main() {
	Tile Board[N][N];
	srand(time(NULL));

	// character list
	Character* characterList = malloc(sizeof(Character) * 2);
	if (characterList == NULL) {
		printf("\nError allocating memory for game characters. Living now...\n");
		exit(EXIT_FAILURE);
	}

	// NPC
	characterList[0].Active  = true;
	characterList[0].Alive   = true;
	characterList[0].Stamina = 10;
	characterList[0].Type    = NPC;
	characterList[0].Line    = 0;
	characterList[0].Column  = 0;
	
	// Player (hooman)
	characterList[1].Active  = true;
	characterList[1].Alive   = true;
	characterList[1].Stamina = 10;
	characterList[1].Type    = Player;
	
	do {
		characterList[1].Line    = rand() % N;
		characterList[1].Column  = rand() % N;
	} while (characterList[1].Line == characterList[0].Line
			&& characterList[1].Column == characterList[0].Column);

	resetBoard(Board, 15);
	loopMaster(Board, characterList);

	return (0);
}
