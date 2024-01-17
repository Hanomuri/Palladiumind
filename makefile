RESOURCESDIR = Resources
CUSTOMDIR = Custom
SRC = Scripts
SRC_STRUCTURES = Scripts/DataStructures

SOURCES_STRUCTURES = $(SRC_STRUCTURES)/AVLTree.c $(SRC_STRUCTURES)/BookLinkedList.c
SOURCES_APP = $(SRC)/Entry.c $(SRC)/CommandMode.c $(SRC)/Custom.c $(SRC)/Screen.c

all:
	gcc Palladiumind.c $(SOURCES_STRUCTURES) $(SOURCES_APP) -o Palladiumind -lm -g -lpthread -Wall
	mkdir -p $(RESOURCESDIR)
	mkdir -p $(CUSTOMDIR)