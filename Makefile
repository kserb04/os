.PHONY: clean
CC=gcc

signals:
	$(CC) -o signals.out signals.c

shell:
	$(CC) -o shell.out shell.c

paging:
	$(CC) -o paging.out paging.c

monitor:
	$(CC) -o montor.out monitor.c

semaphore:
	$(CC) -o semaphore.out semaphore.c

clean:
	rm -f *.out
