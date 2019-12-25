#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
	char name[11];
	int score;
} Top;

void menu(WINDOW *win, int y, int x, Top *top, int **tablaRes, int *scoreRes);
void storeScore(int score, Top *top);
void sortHighscore(Top *top);
void getHighscore(Top *top);
void writeHighscore(Top *top);

void generareCelula(int **tabla)
{
	// Pozitie aleatorie
	int x = rand() % 4, y = rand() % 4, z;
	while (tabla[x][y] != 0){
		x = rand() % 4;
		y = rand() % 4;		
	}
	// Generare aleatorie cu raport 1:3
	z = (rand() & 3) ? 2 : 4;

	tabla[x][y] = z;
}


void credits(WINDOW *win, int y, int x, Top *top, int **tablaRes, int *scoreRes)
{
	box(win, 0, 0);
	wrefresh(win);

	char *autor = "Nume: Vlad Theia-Madalin\nGrupa: 314CC\nEmail: theia98vlad@yahoo.com";
	WINDOW *creditsWin = newwin(3, 30, y / 2 - 3, x / 2 - 10);
	wprintw(creditsWin, "%s", autor);
	wgetch(creditsWin);
	endwin();
	menu(win, y, x, top, tablaRes, scoreRes);
}

void highscoreWin(WINDOW *win, int y, int x, Top *top, int **tablaRes, int *scoreRes)
{
	WINDOW *hsWin = newwin(y - 1, x - 1, 1, 1);
	box(hsWin, 0, 0);
	mvwprintw(hsWin, 9, x / 2 - 10, "#");
	mvwprintw(hsWin, 9, x / 2 - 7, "Name");
	mvwprintw(hsWin, 9, x / 2 + 4, "Score");
	wrefresh(hsWin);

	//Afisarea topului
	for (int i = 0; i < 10; i++) {
		mvwprintw(hsWin, i + 10, x / 2 - 10, "%d", i + 1);
		mvwprintw(hsWin, i + 10, x / 2 - 8, ".");
		mvwprintw(hsWin, i + 10, x / 2 - 7, "%s", top[i].name);
		mvwprintw(hsWin, i + 10, x / 2 + 4, "%d", top[i].score);
	}
	wrefresh(hsWin);

	WINDOW *hsTitle = newwin(3, 20, 3, x / 2 - 10);
	box(hsTitle, 0, 0);
	mvwprintw(hsTitle, 1, 6, "HIGHSCORES");
	wrefresh(hsTitle);

	wgetch(hsWin);
	endwin();
	menu(win, y, x, top, tablaRes, scoreRes);
}

/*
	Mutarea celulelor catre directia selectata pana la intalnirea
	unei margini sau a unei alte celule.
*/
void deplasare(int key, int **tabla)
{
	switch (key){
		
		case KEY_UP:
			for (int i = 1; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (tabla[i][j] != 0)
					{
						int k = i;
						while (k > 0 && tabla[k - 1][j] == 0)
						{
							tabla[k - 1][j] = tabla[k][j];
							tabla[k][j] = 0;
							k--;
						}
					}
			break;
		case KEY_DOWN:
			for (int i = 2; i >= 0; i--)
				for (int j = 0; j < 4; j++)
					if (tabla[i][j] != 0)
					{
						int k = i;
						while (k < 3 && tabla[k + 1][j] == 0)
						{
							tabla[k + 1][j] = tabla[k][j];
							tabla[k][j] = 0;
							k++;
						}
					}
			break;
		case KEY_LEFT:
			for (int i = 0; i < 4; i++)
				for (int j = 1; j < 4; j++) 
					if(tabla[i][j] != 0) {
						int k = j;
						while (k > 0 && tabla[i][k-1] == 0) {
							tabla[i][k-1] = tabla[i][k];
							tabla[i][k] = 0;
							k--;
						}
					}
			break;
		case KEY_RIGHT:
			for (int i = 0; i < 4; i++)
				for (int j = 2; j >= 0; j--)
					if (tabla[i][j] != 0)
					{
						int k = j;
						while (k < 3 && tabla[i][k + 1] == 0) {
							tabla[i][k + 1] = tabla[i][k];
							tabla[i][k] = 0;
							k++;
						}	
					}
			break;	
	}	
}

/*
	In cazul vecinul din directia selectata de catre utilizator
	a unei celule este egal cu celula, acestea vor forma o celula
	cu valoare dubla celor precedente, iar in locul celulei mutate
	se va plasa o celula goala(de valoare 0)
*/
void unire(int key, int **tabla, int *score)
{
	switch (key)
	{
		case KEY_UP:
			for (int i = 1; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (tabla[i][j] == tabla[i - 1][j])
					{
						tabla[i - 1][j] *= 2;
						tabla[i][j] = 0;
						*score += tabla[i-1][j];
					}
			break;
		case KEY_DOWN:
			for (int i = 2; i >= 0; i--)
				for (int j = 0; j < 4; j++)
					if (tabla[i][j] == tabla[i + 1][j])
					{
						tabla[i + 1][j] *= 2;
						tabla[i][j] = 0;
						*score += tabla[i + 1][j];
					}
			break;
		case KEY_LEFT:
			for (int i = 0; i < 4; i++)
				for (int j = 1; j < 4; j++)
					if (tabla[i][j] == tabla[i][j - 1] && tabla[i][j] != 0)
					{
						tabla[i][j - 1] *= 2;
						tabla[i][j] = 0;
						*score += tabla[i][j - 1];
					}
			break;
		case KEY_RIGHT:
			for (int i = 0; i < 4; i++)
				for (int j = 2; j >= 0; j--)
					if (tabla[i][j] == tabla[i][j + 1] && tabla[i][j] != 0)
					{
						tabla[i][j + 1] *= 2;
						tabla[i][j] = 0;
						*score += tabla[i][j + 1];
					}
			break;
	}
}

 /*
	Aceasta functie imbina functiile de deplasare si unire pentru a realiza
	o miscare a celulelor in felul in care utilizatorul o vede(dupa unirea
	celulelor, acestea se vor deplasa din nou in directia selectata, pentru
	a umple golurile formate de functia de unire).
 */
void mutare(int key, int **tabla, int *score)
{
	int aux[4][4], nr = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			aux[i][j] = tabla[i][j];

	deplasare(key, tabla);
	unire(key, tabla, score);
	deplasare(key, tabla);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (aux[i][j] == tabla[i][j])
				nr++;

	if (nr != 16)
		generareCelula(tabla);
}

/*
	Aceasta functie verifica daca mai exista miscari valide prin intermediul 
	unei table auxiliare, pentru a nu modifica valorile din tabla principala.
	Daca mai exista miscari valide returneaza 0, altfel 1.
*/
int gameOver(WINDOW *win, int **tabla)
{
	int ok = 1;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			if(tabla[i][j] == 0)
			ok = 0;
		}

	if (ok) {
		int **aux1, **aux2, **aux3, **aux4;
		
		aux1 = calloc(4, sizeof(int *));
		for (int i = 0; i < 4; i++)
			aux1[i] = calloc(4, sizeof(int));

		aux2 = calloc(4, sizeof(int *));
		for (int i = 0; i < 4; i++)
			aux2[i] = calloc(4, sizeof(int));

		aux3 = calloc(4, sizeof(int *));
		for (int i = 0; i < 4; i++)
			aux3[i] = calloc(4, sizeof(int));

		aux4 = calloc(4, sizeof(int *));
		for (int i = 0; i < 4; i++)
			aux4[i] = calloc(4, sizeof(int));

		int aux_score = 0;
		int aux_ok = 0;
		
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				aux1[i][j] = tabla[i][j];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				aux2[i][j] = tabla[i][j];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				aux3[i][j] = tabla[i][j];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				aux4[i][j] = tabla[i][j];

		unire(KEY_UP, aux1, &aux_score);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 0; j++)
				if(aux1[i][j] == 0)
					aux_ok++;

		unire(KEY_DOWN, aux2, &aux_score);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (aux2[i][j] == 0)
					aux_ok++;

		unire(KEY_LEFT, aux3, &aux_score);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (aux3[i][j] == 0)
					aux_ok++;

		unire(KEY_RIGHT, aux4, &aux_score);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (aux4[i][j] == 0)
					aux_ok++;

		if(aux_ok == 0) {
			return 1;
		}
	}
	return 0;
}

//Aceasta functie realizeaza scrierea in fisierul "highscore.txt" a datelor din top
void writeHighscore(Top *top)
{
	FILE *f;
	f = fopen("highscore.txt", "w");

	for (int i = 0; i < 10; i++)
		fprintf(f, "%s %d\n", top[i].name, top[i].score);

	fclose(f);
}


//Functie de optinere a valorilor din fisier(sau de a-l initializa daca este gol).
void getHighscore(Top *top)
{
	FILE *f;

	if ((f = fopen("highscore.txt", "r")) == NULL) {
		f = fopen("highscore.txt", "w");
		for (int i = 0; i < 10; i++) {
			strcpy(top[i].name, "NONE");
			top[i].score = 0;
			fprintf(f, "%s %d\n", top[i].name, top[i].score);
		}	
		fclose(f);
	}
	else {
		for(int i = 0; i < 10; i++) 
			fscanf(f, "%s%d", top[i].name, &top[i].score);
		fclose(f);
	}
}

//Functie de sortare a valorilor din top.
void sortHighscore(Top *top)
{
	int i, j;
	for (i = 0; i < 10; ++i)
		for (j = 0; j < 10; ++j)
			if (top[j].score < top[j + 1].score) {
				Top tmp = top[j];
				top[j] = top[j + 1];
				top[j + 1] = tmp;
			}
}

/*
	In cazul in care nu mai exista miscari valide, aceasta functie va permite
	stocarea datelor utilizatorului curent, iar daca scorul sau depaseste 
	al zecelea scor din top, va fi stocat, alaturi de nume, in top, urmand 
	ca topul sa fie sortat.
*/
void storeScore(int score,Top *top)
{
	WINDOW *lostWin = newwin(5, 35, 7, 30);
	box(lostWin, 0, 0);
	mvwprintw(lostWin, 1, 1, "Game Over! Press space!");
	wrefresh(lostWin);
	while (1)
	{
		int space = wgetch(lostWin);
		if (space == 32)
			break;
	}
	mvwprintw(lostWin, 2, 1, "Please enter your name: ");
	wrefresh(lostWin);
	char *name = malloc(sizeof(char) * 11);
	echo();
	mvwscanw(lostWin, 2, 25, "%s", name);
	noecho();

	if (score > top[9].score) {
		top[9].score = score;
		strcpy(top[9].name, name);
		sortHighscore(top);
		writeHighscore(top);
	}
}


//Functia ce imbina toate functiile precedente pentru a realiza jocul.
int game(WINDOW *win, int y, int x, Top *top, int **tabla, int *goal, int *score)
{
	WINDOW *legenda = newwin(25, 50, 3, x / 2 + 10);
	box(legenda, 0, 0);
	mvwprintw(legenda, 1, 1, "Press the arrow keys to move the tiles.");
	mvwprintw(legenda, 3, 1, "Press q to quit.");
	struct tm *time_s;
	time_t current_time;
	current_time = time(NULL);
	time_s = localtime(&current_time);
	char text_time[60];
	strftime(text_time, 60, "%d %B %Y %H:%M:%S", time_s);
	mvwprintw(legenda, 5, 1, text_time);
	wrefresh(legenda);

	WINDOW *scoreWin;
	scoreWin = newwin(8, 50, 3, 11);
	box(scoreWin, 0, 0);
	mvwprintw(scoreWin, 2, 3, "Best Score: %d", top[0].score);
	mvwprintw(scoreWin, 5, 3, "Score: %d", *score);
	wrefresh(scoreWin);

	WINDOW *casuta[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			casuta[i][j] = newwin(5, 13, 15 + i * 5, 11 + j * 13);
			box(casuta[i][j], 0, 0);
			wrefresh(casuta[i][j]);
			if (tabla[i][j] != 0){
				int culoare = log2(tabla[i][j]);
				wattron(casuta[i][j], COLOR_PAIR(culoare));
				wbkgd(casuta[i][j], COLOR_PAIR(culoare));
				wattroff(casuta[i][j], COLOR_PAIR(culoare));
				mvwprintw(casuta[i][j], 2, 6, "%d", tabla[i][j]);
			}
			wrefresh(casuta[i][j]);
		}
	refresh();

	while (1){
		int **tablaAux;
		tablaAux = calloc(4, sizeof(int *));
		for (int i = 0; i < 4; i++)
			tablaAux[i] = calloc(4, sizeof(int));
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				tablaAux[i][j] = tabla[i][j];
		int key = wgetch(win);

			switch (key) {

				case KEY_UP:
					mutare(key, tabla, score);
					break;

				case KEY_DOWN:
					mutare(key, tabla, score);
					break;

				case KEY_LEFT:
					mutare(key, tabla, score);
					break;

				case KEY_RIGHT:
					mutare(key, tabla, score);
					break;

				case 'q':
					menu(win, y, x, top, tablaAux, score);
					break;

				default:
					break;
				}

		//Schimbarea highscore-ului in timpul jocului
		if(*score > top[0].score)
			mvwprintw(scoreWin, 2, 15, "%d", *score);
		else
			mvwprintw(scoreWin, 2, 15, "%d", top[0].score);
		mvwprintw(scoreWin, 5, 10, "%d", *score);
		wrefresh(scoreWin);

		//Colorarea ficarei casute in functie de valoare
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++) {
				if (tabla[i][j] != 0) {
					int culoare = log2(tabla[i][j]);
					wattron(casuta[i][j], COLOR_PAIR(culoare));
					wbkgd(casuta[i][j], COLOR_PAIR(culoare));
					wattroff(casuta[i][j], COLOR_PAIR(culoare));
					mvwprintw(casuta[i][j], 2, 1, "           ");
					mvwprintw(casuta[i][j], 2, 6, "%d", tabla[i][j]);
				}
				else {
					wattron(casuta[i][j], COLOR_PAIR(0));
					wbkgd(casuta[i][j], COLOR_PAIR(0));
					wattroff(casuta[i][j], COLOR_PAIR(0));
					mvwprintw(casuta[i][j], 2, 1, "         ");
				}
				wrefresh(casuta[i][j]);
			}

		//Afisarea timpului curent in legenda
		current_time = time(NULL);
		time_s = localtime(&current_time);
		char text_time[60];
		strftime(text_time, 60, "%d %B %Y %H:%M:%S", time_s);
		mvwprintw(legenda, 5, 1, text_time);
		wrefresh(legenda);

		//Verificarea existentei unei miscari legale
		if(gameOver(win, tabla)) {
			storeScore(*score, top);
			*score = 0;
			menu(win, y, x, top, tabla, score);
		}

		//Verificarea existentei unei celule de 2048
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if(tabla[i][j] == 2048)
					(*goal)++;

		if((*goal) == 1) {
			WINDOW *goalWin = newwin(8, 50, 3, 11);
			box(goalWin, 0, 0);
			wbkgd(goalWin, COLOR_PAIR(4));
			mvwprintw(goalWin, 2, 20, "YOU WON!!!");
			mvwprintw(goalWin, 4, 13, "Press space to continue!");
			wrefresh(goalWin);
			while(1) {
				int space = wgetch(goalWin);
				if(space == 32)
					break;
			}

			box(scoreWin, 0, 0);
			wrefresh(scoreWin);
		}
	}
}

void menu(WINDOW *win, int y, int x, Top *top, int **tablaRes, int *scoreRes)
{
	getHighscore(top);

	int goal = 0;

	box(win, 0, 0);
	wrefresh(win);

	WINDOW *win_2048 = newwin(3, 20, 3, x / 2 - 10);
	box(win_2048, 0, 0);
	mvwprintw(win_2048, 1, 8, "2048");
	wrefresh(win_2048);

	// Optiunile meniului
	char* optiuni[5] = { "NEW GAME", "RESUME", "HIGHSCORE", "CREDITS", "QUIT" };
	int optiune;
	int highlight = 0;
	int i;

	// Algoritm de navigare si precizare a optiunii pe care te aflii
	while (1)
	{
		for (i = 0; i < 5; i++)
		{
			if(i == highlight)
				wattron(win, A_STANDOUT | A_BOLD);
			mvwprintw(win, i * 4 + 10, x / 2 - strlen(optiuni[i]) / 2 - 1, "%s", optiuni[i]);
			wattroff(win, A_STANDOUT | A_BOLD);
		}
		optiune = wgetch(win);

		//Navigarea meniului
		switch (optiune)
		{
			case KEY_UP:
				highlight--;
				if (highlight == -1)
					highlight = 4;
				break;

			case KEY_DOWN:
				highlight++;
				if (highlight == 5)
					highlight = 0;
				break;

			//Accesarea fereastrei alease in cazul selectarii unei optiuni	
			case '\n':
				switch (highlight)
				{
					//Fereastra de new game
					case 0:
						clear();
						refresh();
						int score = 0;
						int **tabla;
						tabla = calloc(4, sizeof(int *));
						for (int i = 0; i < 4; i++)
							tabla[i] = calloc(4, sizeof(int));
						generareCelula(tabla);
						generareCelula(tabla);
						game(win, y, x, top, tabla, &goal, &score);
						clear();
						refresh();
						break;
					//Fereastra de resume
					case 1:
						clear();
						refresh();
						if(*scoreRes != 0)
							game(win, y, x, top, tablaRes, &goal, scoreRes);
						else
							menu(win, y, x, top, tabla, scoreRes);
						clear();
						refresh();
						break;
					//Fereastra de highscore	
					case 2:
						werase(win);
						wrefresh(win);
						highscoreWin(win, y, x, top, tablaRes, scoreRes);
						break;
					// Fereastra de credite
					case 3:
						werase(win);
						wrefresh(win);
						credits(win, y, x, top, tablaRes, scoreRes);
						break;
					// Fereastra de exit
					case 4:
						werase(win);
						box(win, 0, 0);
						wrefresh(win);
						mvwprintw(win, y / 2 - 5, x / 2 - 6, "Are you sure?");
					

						char *optiuniExit[2] = { "Yes", "No" };
						int optiuneExit;
						int highlightExit = 0;
						int j;

						while (1)
						{
							for(j = 0; j < 2; j++)
							{
								if(j == highlightExit)
									wattron(win, A_STANDOUT);
								mvwprintw(win, y / 2 - 2, x / 2 - 6 + j * 10 , "%s", optiuniExit[j]);
								wattroff(win, A_STANDOUT);
							}

							optiuneExit = wgetch(win);

							switch (optiuneExit)
							{
								case KEY_RIGHT:
									highlightExit++;
									if(highlightExit == 2)
										highlightExit = 0;
									break;

								case KEY_LEFT:
									highlightExit--;
									if(highlightExit == -1)
										highlightExit = 1;
									break;
								
								case '\n':
									switch(highlightExit)
									{
										case 0:
											werase(win);
											mvwprintw(win, y / 2 - 2, x / 2 - 12, "Goodbye! Press any key..");
											wgetch(win);
											endwin();
											exit(0);
											break;

										case 1:
											werase(win);
											menu(win, y, x, top, tabla, scoreRes);
											break;

										default:
											break;
									}
									break;
							}
						}
					
				}
				break;

			default:
				break;
		}
	}
}	

int main()
{
	// START NCURSES
	initscr();
	cbreak();
	noecho();
	curs_set(false);
	// get screen size
	int yMax,xMax;
	getmaxyx(stdscr, yMax, xMax);

	WINDOW *win = newwin(yMax - 1, xMax - 1, 1, 1);
	keypad(win, TRUE);
	
	//initializare culori pentru fiecare valoare
	start_color();
	init_pair(0, 1, 0);
	init_pair(1, 2, 0);
	init_pair(2, 3, 0);
	init_pair(3, 4, 0);
	init_pair(4, 5, 0);
	init_pair(5, 6, 0);
	init_pair(6, 20, 0);
	init_pair(7, 11, 0);
	init_pair(8, 12, 0);
	init_pair(9, 13, 0);
	init_pair(10, 14, 0);
	init_pair(11, 15, 0);
	init_pair(12, 16, 0);

	srand(time(NULL));
	int score = 0;
	int **tabla;
	tabla = calloc(4, sizeof(int *));
	for (int i = 0; i < 4; i++)
		tabla[i] = calloc(4, sizeof(int));
	Top top[10];
	menu(win, yMax, xMax, top, tabla, &score);

	// END NCURSES
	getch();
	endwin();

	return 0;
}