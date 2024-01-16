RESOURCESDIR = Resources
CUSTOMDIR = Custom
SRC = Scripts

SOURCES = $(SRC)/Entry.c $(SRC)/CommandMode.c $(SRC)/AVLTree.c $(SRC)/Custom.c $(SRC)/Screen.c

all:
	gcc Palladiumind.c $(SOURCES) -o Palladiumind -lm -g -Wall
	mkdir -p $(RESOURCESDIR)
	mkdir -p $(CUSTOMDIR)