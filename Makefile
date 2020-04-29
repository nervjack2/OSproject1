all: main.c scheduler.c
	gcc -o main main.c scheduler.c
clean:
	rm -f main
