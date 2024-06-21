default:
	gcc -Wall -Wextra -Werror -pedantic -std=c99 -o child child.c
	gcc -Wall -Wextra -Werror -pedantic -std=c99 -o parent parent.c

clean:
	rm -f parent child
