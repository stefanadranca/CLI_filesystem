build:
	gcc fsminishell.c -o fsminishell -g -Wall
run: fsminishell
	./fsminishell
clean:
	rm fsminishell
