#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

char *imena[128];
pid_t pids[128];
int c = 1;
struct sigaction prije;

void obradi_dogadjaj(int sig) {
  printf("\n[signal SIGINT] proces %d primio signal %d\n", (int)getpid(), sig);
}

void obradi_signal_zavrsio_neki_proces_dijete(int id) {
  pid_t pid_zavrsio = waitpid(-1, NULL, WNOHANG);
  if (pid_zavrsio > 0) {
    if (kill(pid_zavrsio, 0) == -1) {
      printf("\n[roditelj %d - SIGCHLD + waitpid] dijete %d zavrsilo s radom\n",
             (int)getpid(), pid_zavrsio);
      for (int i = 0; i < c; i++) {
        if (pids[i] == pid_zavrsio) {
          imena[i] = NULL;
          pids[i] = 0;
        }
      }
    }
  }
}

pid_t pokreni_program(char *arg[], int fb, int index, int c, char *imena[]) {
  pid_t pid_novi;
  if (fb == 1) {
    arg[index - 1] = NULL;
  }
  if ((pid_novi = fork()) == 0) {
    printf("[dijete %d] krenuo s radom\n", (int)getpid());
    sigaction(SIGINT, &prije, NULL);
    setpgid(pid_novi, pid_novi);
    if (!fb) {
      tcsetpgrp(STDIN_FILENO, getpid());
    }
    execvp(arg[0], arg);
    perror("Nisam pokrenuo");
    exit(1);
  }
  return pid_novi;
}

void promijeni_dir(char *arg[]) {
  if (chdir(arg[1]) == 0) {
    chdir(arg[1]);
    printf("Mijenjam direktorij\n");
  } else {
    printf("cd nije uspio\n");
  }
}

void ps(char *arg[]) {
  printf("ISPIS SVIH POKRENUTIH PROCESA: \n");
  for (int i = 0; i < c; i++) {
    if (pids[i] != 0)
      printf("Ime: %s, PID: %d\n", imena[i], pids[i]);
  }
}

void ubijanje(char *arg[]) {
  int ubijeni = atoi(arg[1]);
  int signal = atoi(arg[2]);
  for (int i = 0; i < c; i++) {
    if (ubijeni == pids[i]) {
      pids[i] = 0;
      imena[i] = NULL;
      printf("Saljem signal %d procesu %d\n", signal, ubijeni);
      if (kill(ubijeni, signal) == 0) {
        printf("Ubijeni: %d, signal: %d\n", ubijeni, signal);
      }
    }
  }
}

int main() {

  size_t vel = 128;
  char buffer[vel];
  int max = 5;
  char *argv[5];
  int argc = 0;

  struct sigaction act;
  pid_t pid_novi;

  for (int i = 0; i < 128; i++) {
    pids[i] = 0;
    imena[i] = NULL;
  }

  pids[0] = getpid();
  imena[0] = "roditelj";
  printf("roditelj[PID = %d] krenuo s radom\n", pids[0]);
  ;

  act.sa_handler = obradi_dogadjaj;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGINT, &act, &prije);
  act.sa_handler = obradi_signal_zavrsio_neki_proces_dijete;
  sigaction(SIGCHLD, &act, NULL);
  act.sa_handler = SIG_IGN;
  sigaction(SIGTTOU, &act, NULL);

  struct termios shell_term_settings;
  tcgetattr(STDIN_FILENO, &shell_term_settings);

  do {
    printf("[roditelj] Unesi naredbu: ");

    if (fgets(buffer, vel, stdin) != NULL) {
      argc = 0;
      argv[argc] = strtok(buffer, " \t\n");
      while (argv[argc] != NULL) {
        argc++;
        argv[argc] = strtok(NULL, " \t\n");
      }

      if (strcmp(argv[0], "cd") == 0) {
        promijeni_dir(argv);
      } else if (strcmp(argv[0], "ps") == 0) {
        ps(argv);
      } else if (strcmp(argv[0], "kill") == 0) {
        ubijanje(argv);
      } else if (strcmp(argv[0], "exit") == 0) {
        printf("exit\n");
      } else if (strcmp(argv[argc - 1], "&") == 0) {
        pid_t drugi = pokreni_program(argv, 1, argc, c, imena);
        pids[c] = drugi;
        char ime[25];
        strcpy(ime, argv[0]);
        imena[c] = ime;
        c++;
      } else {
        pid_t novi;
        printf("[roditelj] Pokrecem program\n");
        novi = pokreni_program(argv, 0, argc + 1, c, imena);
        printf("[roditelj] Cekam da zavrsi\n");
        pid_t pid_zavrsio;

        do {
          pid_zavrsio = waitpid(novi, NULL, 0);
          if (pid_zavrsio > 0) {
            if (kill(novi, 0) == -1) {
              printf("[roditelj] Dijete %d zavrsilo s radom\n", pid_zavrsio);
              tcsetpgrp(STDIN_FILENO, getpgid(0));
              tcsetattr(STDIN_FILENO, 0, &shell_term_settings);
            } else {
              novi = (pid_t)0;
            }
          } else {
            // printf("[roditelj] waitpid gotov ali ne daju informaciju\n");
            break;
          }
        } while (pid_zavrsio <= 0);
      }
    }
  } while (strncmp(buffer, "exit", 4) != 0);

  return 0;
}
