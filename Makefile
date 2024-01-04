CFLAGS = -g -Wall -Wextra -Werror -fstack-protector
OBJ = main.o building.o util.o ghost.o logger.o

all: final

final: $(OBJ)
	gcc $(CFLAGS) -o final $(OBJ) -lpthread

main.o: main.c defs.h
	gcc $(CFLAGS) -c main.c

building.o: building.c defs.h
	gcc $(CFLAGS) -c building.c

util.o: util.c defs.h
	gcc $(CFLAGS) -c util.c

clean:
	rm -f main.o building.o util.o final
