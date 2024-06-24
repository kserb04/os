#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t m;
pthread_cond_t red;
pthread_cond_t red_citaca_ceka;
pthread_cond_t red_citaca_cita;
pthread_cond_t red_brisaca_brise;
pthread_cond_t red_brisaca_ceka;
pthread_cond_t red_pisaca_ceka;
pthread_cond_t red_pisaca_pise;
int brojCitaca = 8;
int brojPisaca = 5;
int brojBrisaca = 2;
int br_citaca_ceka = 0;
int br_citaca_cita = 0;
int br_pisaca_ceka = 0;
int br_pisaca_pise = 0;
int br_brisaca_brise = 0;
int br_brisaca_ceka = 0;
int brEl = 0;
clock_t start;

typedef struct Node {
  int broj;
  struct Node *sljedeci;
} Node;

typedef struct {
  Node *head;
} LinkedList;

LinkedList lista;

Node *novi_clan(int data) {
  Node *novi = (Node *)malloc(sizeof(Node));
  novi->broj = data;
  novi->sljedeci = NULL;
  return novi;
}

void dodaj(int broj, LinkedList *lista) {
  Node *novi = novi_clan(broj);
  if (lista->head == NULL) {
    lista->head = novi;
    return;
  }

  Node *t = lista->head;
  while (t->sljedeci != NULL) {
    t = t->sljedeci;
  }
  t->sljedeci = novi;
}

void isprintaj(LinkedList lista) {
  if (lista.head != NULL) {
    printf("Lista: ");
    Node *t = lista.head;
    while (t != NULL) {
      printf("%d ", t->broj);
      t = t->sljedeci;
    }
    printf("\n\n");
  } else {
    printf("Lista: \n");
  }
}

bool izbrisi(int trazeni, LinkedList *lista) {
  Node *t = lista->head;
  Node *prev = NULL;

  if (t != NULL && t->broj == trazeni) {
    lista->head = t->sljedeci;
    free(t);
    brEl--;
    return true;
  }
  while (t != NULL && t->broj != trazeni) {
    prev = t;
    t = t->sljedeci;
  }

  if (t == NULL)
    return false;
  else if (t->broj == trazeni) {
    prev->sljedeci = t->sljedeci;
    free(t);
    brEl--;
    return true;
  }
  return false;
}

int prebroji(LinkedList *lista) {
  int brojEl = 0;
  if (lista->head != NULL) {
    Node *t = lista->head;
    while (t != NULL) {
      brojEl++;
      t = t->sljedeci;
    }
  }

  return brojEl;
}

int traziInd(int broj) {
  int c = 0;
  if (lista.head != NULL) {
    Node *t = lista.head;
    while (c != broj && t->sljedeci != NULL) {
      t = t->sljedeci;
      c++;
    }
    if (t != NULL)
      return t->broj;
  } else {
    printf("Lista je trenutno prazna\n");
  }
}

void *noviCitac(void *i) {
  int *b = (int *)i;
  int broj = *b;
  printf("Radim novu dretvu citaca broj %d\n", broj);

  while (1) {
    int ind = rand() % brEl;
    pthread_mutex_lock(&m);
    clock_t trenutno = clock();
    int proslo = (trenutno - start) / 1000;
    printf("t = %d	Citac %d zeli citati element %d liste\n", proslo, broj,
           ind);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    br_citaca_ceka++;
    while (br_brisaca_brise + br_brisaca_ceka > 0) {
      pthread_cond_wait(&red_citaca_ceka, &m);
    }
    br_citaca_cita++;
    br_citaca_ceka--;
    int procitan = traziInd(ind);
    trenutno = clock();
    proslo = (trenutno - start) / 1000;
    printf("t = %d	Citac %d cita element %d liste (vrijednosti %d)\n",
           proslo, broj, ind, procitan);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);

    pthread_mutex_unlock(&m);

    int spavaj = rand() % 5 + 5;
    sleep(spavaj);

    pthread_mutex_lock(&m);

    br_citaca_cita--;
    if (br_citaca_cita == 0 && br_brisaca_ceka > 0) {
      pthread_cond_signal(&red_brisaca_ceka);
    }

    trenutno = clock();
    proslo = (trenutno - start) / 1000;
    printf("t = %d	Citac %d vise ne koristi listu\n", proslo, broj);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    pthread_mutex_unlock(&m);

    spavaj = rand() % 5 + 5;
    sleep(spavaj);
  }

  free(b);
  return NULL;
}

void *noviPisac(void *i) {
  int *b = (int *)i;
  int broj = *b;
  printf("Radim novu dretvu pisaca broj %d\n", broj);

  while (1) {
    pthread_mutex_lock(&m);
    int noviClan = rand() % 200;
    clock_t trenutno = clock();
    int proslo = (trenutno - start) / 1000;
    printf("t = %d	Pisac %d zeli dodati vrijednost %d u listu\n", proslo,
           broj, noviClan);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    br_pisaca_ceka++;
    while ((br_brisaca_brise + br_brisaca_ceka > 0) || (br_pisaca_pise > 0)) {
      pthread_cond_wait(&red_pisaca_ceka, &m);
    }
    br_pisaca_pise++;
    br_pisaca_ceka--;
    trenutno = clock();
    proslo = (trenutno - start) / 1000;
    printf(
        "t = %d	Pisac %d zapocinje dodavanje vrijednosti %d na kraj liste\n",
        proslo, broj, noviClan);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    pthread_mutex_unlock(&m);
    int spavaj = rand() % 5 + 5;
    sleep(spavaj);

    pthread_mutex_lock(&m);
    br_pisaca_pise--;
    if (br_pisaca_pise == 0 && br_brisaca_ceka > 0)
      pthread_cond_signal(&red_brisaca_ceka);
    if (br_pisaca_pise == 0 && br_pisaca_ceka > 0)
      pthread_cond_signal(&red_pisaca_ceka);
    trenutno = clock();
    proslo = (trenutno - start) / 1000;
    printf("t = %d	Pisac %d dodao vrijednost %d na kraj liste\n", proslo,
           broj, noviClan);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    dodaj(noviClan, &lista);
    brEl++;
    isprintaj(lista);
    pthread_mutex_unlock(&m);

    spavaj = rand() % 5 + 5;
    sleep(spavaj);
  }
  free(b);
  return NULL;
}

void *noviBrisac(void *i) {
  int *b = (int *)i;
  int broj = *b;
  printf("Radim novu dretvu brisaca broj %d\n", broj);

  while (1) {
    while (brEl == 0)
      sleep(1);
    int ind = rand() % brEl;
    pthread_mutex_lock(&m);
    clock_t trenutno = clock();
    int proslo = (trenutno - start) / 1000;
    printf("t = %d	Brisac %d zeli obrisati element %d iz liste\n", proslo,
           broj, ind);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    br_brisaca_ceka++;
    while ((br_brisaca_brise > 0) || (br_citaca_cita > 0) ||
           (br_pisaca_pise > 0)) {
      pthread_cond_wait(&red_brisaca_ceka, &m);
    }
    br_brisaca_brise++;
    br_brisaca_ceka--;
    int traz = traziInd(ind);
    trenutno = clock();
    proslo = (trenutno - start) / 1000;
    printf("t = %d	Brisac %d zapocinje s brisanjem elementa %d liste "
           "(vrijednost=%d)\n",
           proslo, broj, ind, traz);
    printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
           br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
    isprintaj(lista);
    pthread_mutex_unlock(&m);

    int spavaj = rand() % 5 + 5;
    sleep(spavaj);

    pthread_mutex_lock(&m);
    if (izbrisi(traz, &lista)) {
      trenutno = clock();
      proslo = (trenutno - start) / 1000;
      printf("t = %d	Brisac %d obrisao element %d liste (vrijednost=%d)\n",
             proslo, broj, ind, traz);
      printf("t = %d	Aktivnih: citaca=%d, pisaca=%d, brisaca=%d\n", proslo,
             br_citaca_cita, br_pisaca_pise, br_brisaca_brise);
      isprintaj(lista);
      br_brisaca_brise--;
      isprintaj(lista);
    } else {
      printf("Brisac %d nije obrisao element %d liste vrijednosti %d, vec je "
             "obrisan\n",
             broj, ind, traz);
      isprintaj(lista);
      br_brisaca_brise--;
    }

    if (br_brisaca_brise == 0 && br_brisaca_ceka > 0)
      pthread_cond_signal(&red_brisaca_ceka);
    if (br_brisaca_brise == 0 && br_pisaca_ceka > 0)
      pthread_cond_signal(&red_pisaca_ceka);
    if (br_brisaca_brise == 0 && br_citaca_ceka > 0)
      pthread_cond_broadcast(&red_citaca_ceka);

    pthread_mutex_unlock(&m);

    spavaj = rand() % 5 + 20;
    sleep(spavaj);
  }
  free(b);
  return NULL;
}

int main() {
  srand(time(NULL));
  pthread_mutex_init(&m, NULL);
  pthread_cond_init(&red, NULL);

  start = clock();

  pthread_t tidPisaci[brojPisaca];
  for (int i = 0; i < brojPisaca; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&tidPisaci[i], NULL, noviPisac, arg);
  }

  sleep(10);

  pthread_t tidCitaci[brojCitaca];
  for (int i = 0; i < brojCitaca; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&tidCitaci[i], NULL, noviCitac, arg);
  }

  sleep(10);

  pthread_t tidBrisaci[brojBrisaca];
  for (int i = 0; i < brojBrisaca; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&tidBrisaci[i], NULL, noviBrisac, arg);
  }

  for (int i = 0; i < brojPisaca; i++) {
    pthread_join(tidPisaci[i], NULL);
  }

  for (int i = 0; i < brojCitaca; i++) {
    pthread_join(tidCitaci[i], NULL);
  }

  for (int i = 0; i < brojBrisaca; i++) {
    pthread_join(tidBrisaci[i], NULL);
  }

  return 0;
}
