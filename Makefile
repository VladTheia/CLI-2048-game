build: 2048
	
2048: 2048.o
	gcc -Wall 2048.o -o 2048 -lncurses -lm

2048.o: 2048.c
	sudo apt-get install libncurses5-dev libncursesw5-dev
	gcc -Wall -c 2048.c

run: 2048
	./2048

clean:
	rm -f 2048.o 2048 highscore.txt