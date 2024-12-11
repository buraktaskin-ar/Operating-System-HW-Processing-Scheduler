CC = gcc

main: process_scheduler.c
	$(CC) -o process_scheduler process_scheduler.c
