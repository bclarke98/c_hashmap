CC = gcc
CFLAGS = -Wall --std=c99 -g
MAIN = main.o
OBJECTS =  $(MAIN) c_string.o c_hashmap.o

hashmap_driver: $(OBJECTS)
	$(CC) $(CFLAGS) -o hashmap_driver $(OBJECTS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

c_string.o: c_string.c
	$(CC) $(CFLAGS) -c c_string.c -o c_string.o

c_hashmap.o: c_hashmap.c
	$(CC) $(CFLAGS) -c c_hashmap.c -o c_hashmap.o

clean:
	rm hashmap_driver $(OBJECTS)

