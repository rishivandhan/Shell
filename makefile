FLAGS: -Wall -pedantic-errors

cl: shell.c
	gcc -o shell shell.c
clean:
	rm -f shell
run: cl
	./shell