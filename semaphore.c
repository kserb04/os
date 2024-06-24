#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#define BUD 20
#define BRD 20
#define BID 20
#define VEL 20

int bud, brd, bid, vel;

char ums[BRD][VEL];
char ims[BID][VEL];
int ulazums[BRD];
int izlazums[BRD];
int ulazims[BID];
int izlazims[BID];

sem_t sem_od_umsa[BRD];
sem_t sem_od_imsa[BID];
sem_t osem_ums[BRD];
sem_t sem_ispis;

void ispisUms() {
		printf("UMS[]: ");
		for (int i = 0; i < brd; i++) {
				for (int j = 0; j < vel; j++) {
						printf("%c", ums[i][j]);
				}
				printf(" ");
		}
		printf("\n");
}

void ispisIms() {
		printf("IMS[]: ");
		for (int i = 0; i < bid; i++) {
				for (int j = 0; j < vel; j++) {
						printf("%c", ims[i][j]);
				}
				printf(" ");
		}
		printf("\n");
}


void *tFja(void *i) {
		int *b = (int *)i;
		int broj = *b;
		printf("radim dretvu broj %d\n", broj);

		free(b);
		return NULL;
}

void *novaUlazna(void *i) {
		int *b = (int *)i;
		int broj = *b;
		printf("Radim novu ulaznu dretvu %d\n", broj);

		while(1) {
				char rand_veliko = 'A' + rand()%26; // ovo je ulazni podatak
				int indeks = rand()%brd;
				sem_wait(&sem_ispis);
				printf("U%d: napravi podatak->'%c'; pohrani u spremnik UMS[%d]\n", broj, rand_veliko, indeks);
				sem_post(&sem_ispis);
				sem_wait(&sem_od_umsa[indeks]); // cekamo da mozemo upisati u ums
				if (ums[indeks][ulazums[indeks]] ==  '-') {
						ums[indeks][ulazums[indeks]] = rand_veliko; // upisujemo podatak
						ulazums[indeks] = (ulazums[indeks] + 1)%vel; // kruzni ms
						sem_wait(&sem_ispis);
						ispisUms();
						ispisIms();
						sem_post(&sem_ispis);
						sem_post(&sem_od_umsa[indeks]); // spremnik je spreman za sljedece citanje/pisanje
						sem_post(&osem_ums[indeks]);
				} else {
						ums[indeks][ulazums[indeks]] = rand_veliko; // PREpisujemo podatak
						ulazums[indeks] = (ulazums[indeks] + 1)%vel; // kruzni ms
						izlazums[indeks] = (izlazums[indeks] + 1)%vel; // kruzni ms
						sem_wait(&sem_ispis);
						ispisUms();
						ispisIms();
						sem_post(&sem_ispis);
						sem_post(&sem_od_umsa[indeks]); // spremnik je spreman za sljedece citanje/pisanje
				}
				sleep(3);
		}

		free(b);
}

void *novaRadna(void *i) {
		int *b = (int *)i;
		int broj = *b;
		printf("Radim novu radnu dretvu %d\n", broj);

		while(1) {
				sem_wait(&osem_ums[broj]);
				sem_wait(&sem_od_umsa[broj]);
				char procitan = ums[broj][izlazums[broj]];
				char malo = procitan + 32; // procitan podatak 'obradujemo' - veliko u malo slovo
				ums[broj][izlazums[broj]] = '-';
				izlazums[broj] = (izlazums[broj] + 1)%vel; // kruzni ms
				sem_post(&sem_od_umsa[broj]); // procitan je podatak, spremnik je spreman za novo
				int indeks = rand()%bid;
				sem_wait(&sem_ispis);
				printf("R%d: procitaj podatak->'%c'; pohrani u spremnik IMS[%d]\n", broj, procitan, indeks);
				sem_post(&sem_ispis);
				sem_wait(&sem_od_imsa[indeks]); // cekamo da mozemo upisati u ims
				ims[indeks][ulazims[indeks]] = malo; // upisujemo podatak
				ulazims[indeks] = (ulazims[indeks] + 1)%vel; // kruzni ms
				sem_wait(&sem_ispis);
				ispisUms();
				ispisIms();
				sem_post(&sem_ispis);
				sem_post(&sem_od_imsa[indeks]); // spremnik je spreman za novo pisanje
				sleep(3);
		}

		free(b);
}

void *novaIzlazna(void *i) {
		int *b = (int *)i;
		int broj = *b;
		printf("Radim novu izlaznu dretvu %d\n", broj);
		while(1) {
				sem_wait(&sem_od_imsa[broj]);
				char procitan = ims[broj][izlazims[broj]];
				ims[broj][izlazims[broj]] = '-';
				izlazims[broj] = (izlazims[broj] + 1)%vel; // kruzni ms
				sem_wait(&sem_ispis);
				if (procitan == '-') {
						printf("I%d: citam podatak '0'\n", broj);
				} else {
						printf("I%d: citam podatak '%c'\n", broj, procitan);
				}
				ispisUms();
				ispisIms();
				sem_post(&sem_ispis);
				sem_post(&sem_od_imsa[broj]);
				sleep(3);
		}

		free(b);
		return NULL;
}

int main() {
		printf("Unesi broj ulaznih, radnih i izlaznih dretvi: ");
		scanf("%d %d %d", &bud, &brd, &bid);

		printf("Unesi velicinu meduspremnika: ");
		scanf("%d", &vel);

		sem_post(&sem_ispis);
		srand(time(NULL));
		for (int i = 0; i < brd; i++) {
				sem_post(&sem_od_umsa[i]);
		}

		for (int i = 0; i < bid; i++) {
				sem_post(&sem_od_imsa[i]);
		}

		for (int i = 0; i < brd; i++) {
				ulazums[i] = 0;
		}

		for (int i = 0; i < bid; i++) {
				ulazims[i] = 0;
		}

		for (int i = 0; i < bid; i++) {
				izlazims[i] = 0;
		}

		for (int i = 0; i < brd; i++) {
				izlazums[i] = 0;
		}


		for (int i = 0; i < brd; i++) {
				for (int j = 0; j < vel; j++) {
						ums[i][j] = '-';
				}
		}

		printf("UMS[]: ");
		for (int i = 0; i < brd; i++) {
				for (int j = 0; j < vel; j++) {
						printf("%c", ums[i][j]);
				}
				printf(" ");
		}


		for (int i = 0; i < bid; i++) {
				for (int j = 0; j < vel; j++) {
						ims[i][j] = '-';
				}
		}

		printf("\nIMS[]: ");
		for (int i = 0; i < bid; i++) {
				for (int j = 0; j < vel; j++) {
						printf("%c", ims[i][j]);
				}
				printf(" ");
		}
		printf("\n");

		int ulaz = 0;
		int izlaz = 0;

		pthread_t tidul[bud];
		for (int i = 0; i < bud; i++) {
				int *arg = malloc(sizeof(int));
				*arg = i;
				pthread_create(&tidul[i], NULL, novaUlazna, arg);
		}

		sleep(15);

		pthread_t tidra[brd];
		for (int i = 0; i < brd; i++) {
				int *arg = malloc(sizeof(int));
				*arg = i;
				pthread_create(&tidra[i], NULL, novaRadna, arg);
		}

		sleep(15);

		pthread_t tidiz[bid];
		for (int i = 0; i < bid; i++) {
				int *arg = malloc(sizeof(int));
				*arg = i;
				pthread_create(&tidiz[i], NULL, novaIzlazna, arg);
		}

		for (int i = 0; i < bud; i++) {
				pthread_join(tidul[i], NULL);
		}

		for (int i = 0; i < brd; i++) {
				pthread_join(tidra[i], NULL);
		}

		for (int i = 0; i < bid; i++) {
				pthread_join(tidiz[i], NULL);
		}

		return 0;
}
