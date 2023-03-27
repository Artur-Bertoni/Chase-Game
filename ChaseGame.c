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
						m2 = NPCCell;
					} else {
						m2 = (characterList[k].Type == NPC ? NPCCell : PlayerCell);
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

bool npcMovement(Tile Board[N][N], Character* characterList) {
	// NPC's position
	int npcLine = characterList[0].Line;
	int npcColumn = characterList[0].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	// performing NPC's movement
	if (npcLine > playerLine && Board[npcLine-1][npcColumn].Blocked == false && abs(npcLine - 1) < N && (npcLine - 1) >= 0) { //UP
		characterList[0].Line -= 1;
	} else if (npcLine < playerLine && Board[npcLine+1][npcColumn].Blocked == false && abs(npcLine + 1) < N) { //DOWN
		characterList[0].Line += 1;
	} else if (npcColumn > playerColumn && Board[npcLine][npcColumn-1].Blocked == false && abs(npcColumn - 1) < N && (npcColumn - 1) >= 0) { //LEFT
		characterList[0].Column -= 1;
	} else if (npcColumn < playerColumn && Board[npcLine][npcColumn+1].Blocked == false && abs(npcColumn + 1) < N) { //RIGHT
		characterList[0].Column += 1;
	}

	if (characterList[0].Line == characterList[1].Line && characterList[0].Column == characterList[1].Column)
		return true;
	else
		return false;
}

void playerMovement(Tile Board[N][N], Character* characterList) {
	fflush(stdin);

	// NPC's position
	int npcLine = characterList[0].Line;
	int npcColumn = characterList[0].Column;

	// Player's position
	int playerLine = characterList[1].Line;
	int playerColumn = characterList[1].Column;

	if (GetAsyncKeyState (VK_UP)&1) { // UP
		if (Board[playerLine-1][playerColumn].Blocked == false && !((playerLine - 1) == npcLine && playerColumn == npcColumn) && abs(playerLine - 1) < N && (playerLine - 1) >= 0)
				characterList[1].Line -= 1;
	} else if (GetAsyncKeyState (VK_DOWN)&1) { // DOWN
		if (Board[playerLine+1][playerColumn].Blocked == false && !((playerLine + 1) == npcLine && playerColumn == npcColumn) && abs(playerLine + 1) < N)
				characterList[1].Line += 1;
	} else if (GetAsyncKeyState (VK_LEFT)&1) { // LEFT
		if (Board[playerLine][playerColumn-1].Blocked == false && !(playerLine == npcLine && (playerColumn - 1) == npcColumn) && abs(playerColumn - 1) < N && (playerColumn - 1) >= 0)
				characterList[1].Column -= 1;
	} else if (GetAsyncKeyState (VK_RIGHT)&1) { // RIGHT
		if (Board[playerLine][playerColumn+1].Blocked == false && !(playerLine == npcLine && (playerColumn + 1) == npcColumn) && abs(playerColumn + 1) < N)
				characterList[1].Column += 1;
	}
	
	fflush(stdin);
}

void loopMaster(Tile Board[N][N], Character* characterList) {
	bool EndGame = false;
	bool GameOver = false;

	system("cls");

	do {
		EndGame = GameOver == true ? true : false;

		if (!EndGame) gotoXY(0,0);
		else system("cls");

		displayBoard(Board, characterList, 2, EndGame);
		playerMovement(Board, characterList);
		Sleep(1000);
		GameOver = npcMovement(Board, characterList);
	} while(!EndGame);
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

	resetBoard(Board, 15, characterList);
	loopMaster(Board, characterList);

	return (0);
}
