#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void obradi_sigterm(int sig); // prioritet 4 (najveci)
void obradi_sigint(int sig); // prioritet 3
void obradi_sighup(int sig); // prioritet 2
void obradi_sigusr1(int sig); // prioritet 1

int main() {
    struct sigaction act;

    // maskiranje signala SIGUSR1
    act.sa_handler = obradi_sigusr1;
    sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    // maskiranje signala SIGHUP
    act.sa_handler = obradi_sighup;
    sigaddset(&act.sa_mask, SIGUSR1);
    sigaction(SIGHUP, &act, NULL);

    // maskiranje signala SIGINT
    act.sa_handler = obradi_sigint;
    sigaddset(&act.sa_mask, SIGHUP);
    sigaction(SIGINT, &act, NULL);

    // maskiranje signala SIGTERM
    act.sa_handler = obradi_sigterm;
    sigaddset(&act.sa_mask, SIGINT);
    sigaction(SIGTERM, &act, NULL);

    printf("Program with PID=%ld started\n", (long) getpid());

    int i = 1;
    while(1) {
        printf("Program: iteration %d\n", i++);
        sleep(1);
    }

    printf("Program with PID=%ld finished\n", (long) getpid());

    return 0;
}

void obradi_sigusr1(int sig) {
    int i;
    printf("Starting processing signal SIGUSR1 (P1)\n");
		for (i = 1; i <= 5; i++) {
        printf("Processing signal: %d/5\n", i);
        sleep(1);
    }
    printf("Finished processing signal SIGUSR1\n");
}

void obradi_sighup(int sig) {
    int i;
    printf("Starting processing signal SIGHUP (P2)\n");
    for (i = 1; i <= 5; i++) {
        printf("Processing signal: %d/5\n", i);
        sleep(1);
    }
    printf("Finished processing signal SIGHUP\n");
}

void obradi_sigint(int sig) {
    int i;
    printf("Starting processing signal SIGINT (P3)\n");
    for (i = 1; i <= 5; i++) {
        printf("Processing signal: %d/5\n", i);
        sleep(1);
    }
    printf("Finished processing signal SIGINT\n");
}

void obradi_sigterm(int sig) {
    int i;
    printf("Starting processing signal SIGTERM (P4)\n");
    for (i = 1; i <= 5; i++) {
        printf("Processing signal: %d/5\n", i);
        sleep(1);
    }
    printf("Ended processing signal SIGTERM\n");
}
