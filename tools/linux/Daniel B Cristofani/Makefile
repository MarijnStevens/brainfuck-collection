CC=gcc
FLAGS=-Os

all: main

main:

	$(CC) $(FLAGS) qdb.c -o ./qdb
	$(CC) $(FLAGS) dbc.c -o ./dbc
	$(CC) $(FLAGS) sbi.c -o ./sbi

clean:
	rm -r bin