#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

char okvir[10][64];
short tp[10][16];
char disk[10][16][64];
int n, m;
int c = 0;
int cijiJe[10];
int flag = 1;

void printBin(short broj) {
		for (int i = 15; i >= 0; i--) {
				short maska = 1 << i;
				short bit = maska & broj;
				if (bit != 0) {
						printf("1");
				} else {
						printf("0");
				}
		}
}

void printTP(short tp[][16]) {
		printf("Tablice prevodenja: \n");
		for (int i = 0; i < n; i++) {
				printf("PROCES %d				", i+1);
		}
		printf("\n");
		for (int i = 0; i < 16; i++) {
				for (int j = 0; j < n; j++) {
							printBin(tp[j][i]);
							printf("			");
				}
				printf("\n");
		}
}

void printRAM(char okvir[][64]) {
		printf("\nRadna memorija: \n");
		for (int i = 0; i < m; i++) {
				for (int j = 0; j < 64; j++) {
						if (okvir[i][j] == 0) {
								printf("0");
						} else {
								printf("%c", okvir[i][j]);
						}
				}
				printf("\n");
		}
}

int ubaci(int p, int x) {
		int maskaOkvir = 0b0000001111000000;
		int indeksOkvira = (maskaOkvir & x) >> 6; // u tp gledam taj redni broj
		printf("(redni broj u tablici prevodenja: %d)\n", indeksOkvira);
		int maskaPomak = 0b0000000000111111;
		int pomak = maskaPomak & x;
		int konacna, upis, stari = c;

		if ((tp[p][indeksOkvira] & (1 << 5)) != 0) { // postoji u RAM-u, samo procitaj
				int maska = 0b1111000000000000;
				int rbr = (maska & tp[p][indeksOkvira])>>12;
				printf("Pogoden\n");
				konacna = rbr << 6;
				konacna = konacna | pomak;
				stari = rbr;
		} else {
				printf("Promasaj\n");
				for (int i = 0; i < 64; i++) {
						char sDiska = disk[p][indeksOkvira][i];
						okvir[c][i] = sDiska;
				}
				upis = c << 6;
				c = (c + 1) % m;
				if (flag == 1) { // prvi put, nikoga se ne izbacuje, normalno sve upisi
						konacna = upis | pomak;
						tp[p][indeksOkvira] = (konacna << 6) | (1 << 5);
						cijiJe[stari]=p;
						if (c == 0) flag = 0;
				} else { // izbacujemo, promijeni u cijiJe i tam bP menjaj
						konacna = upis | pomak;
						tp[p][indeksOkvira] = (konacna << 6) | (1 << 5);
						int ciji = cijiJe[stari];
						cijiJe[stari]=p;
						for (int i = 0; i < 16; i++) {
								if ((((tp[ciji][i] & 0b1111000000000000)>>12) == stari) && ((tp[ciji][i] & (1<<5))!=0)) {
										tp[ciji][i] = tp[ciji][i] & 0b1111111111011111;
										printf("Izbacujem stranicu %d procesa %d\n", i, ciji+1);
										break;
								}
						}
				}
				printf("Dodijeljen okvir 0x%x\n", stari);

		}
		printf("Fizicka adresa: 0x%x (redni broj: %d), sadrzaj: %c \n", konacna, stari, okvir[stari][pomak]);
		printTP(tp);
		return konacna;
}

int main(void) {
		srand(time(NULL));
		printf("Unesi broj okvira M: ");
		scanf("%d", &m);

		int ind = ceil(log(m)/log(2));

		printf("Unesi broj procesa N: ");
		scanf("%d", &n);

		for (int i = 0; i < n; i++) {
				for (int j = 0; j < 16; j++) {
						for (int k = 0; k < 64; k++)	{
								char random = rand() % 25 + 65;
								disk[i][j][k] = random;
						}
				}
		} // inicijaliziranje diska -> random slovo na svako mjesto

		for (int i = 0; i < n; i++) {
				for (int j = 0; j < 16; j++) {
						tp[i][j] = 0;
				}
		} // inicijaliziranje tablice prevodenja -> sve 0 na pocetku

		for (int i = 0; i < m; i++) {
				for (int j = 0; j < 8; j++) {
						okvir[i][j] = 0;
				}
		} // inicijaliziranje okvira -> sve 0 na pocetku

		//generiranje random logicke adrese:
		int maks = pow(2, ind+6) - 1;

		while(1) {
				for (int i = 0; i < n; i++) {
						printf("\nProces: %d\n", i+1);
						int odabran = rand() % maks;
						printf("Logicka adresa: 0x%x ", odabran);

						ubaci(i, odabran);
						printRAM(okvir);

						sleep(3);
					}
		}
		return 0;
}
