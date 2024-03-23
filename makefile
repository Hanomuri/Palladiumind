C = gcc
CFLAGS = -lm -g -lpthread -Wall -Wno-return-type -Wstrict-aliasing -I "include/"
RESOURCESDIR = resources
CUSTOMDIR = custom
SRC = src
SRC_STRUCTURES = src/DataStructures

STRUCTURES = AVLTree.o BookLinkedList.o

SOURCES_STRUCTURES = $(SRC_STRUCTURES)/AVLTree.c $(SRC_STRUCTURES)/BookLinkedList.c
SOURCES_APP = $(SRC)/Entry.c $(SRC)/CommandMode.c $(SRC)/Custom.c $(SRC)/Screen.c $(SRC)/Keyboard.c $(SRC)/NaviMode.c $(SRC)/CoreMode.c

all: Structures.o Palladium Clean
	mkdir -p $(RESOURCESDIR)
	mkdir -p $(CUSTOMDIR)

Structures.o:
	@gcc -c $(SOURCES_STRUCTURES) $^ $(CFLAGS)

Palladium:
	@gcc Palladiumind.c $(STRUCTURES) $(SOURCES_APP) -o Palladiumind $^ $(CFLAGS)
	echo Compilling Palladium...

Clean: Palladium
	rm $(STRUCTURES)