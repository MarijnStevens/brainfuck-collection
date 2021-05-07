CC=gcc
FLAGS=-Os

ARCH=$(shell uname -s)-$(shell uname -m)
BUILDNAME=$(shell tr '[:upper:]' '[:lower:]' <<< $(ARCH))

all: main

main:
	echo -e Building for: $(BUILDNAME)

	$(CC) $(FLAGS) nweb.c -o nweb-$(BUILDNAME)

clean:
	rm -r bin