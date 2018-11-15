CC = gcc
CFLAGS  = -g -Wall


all: myfind rootNode splitterMergerNode leafNode

myfind:  myfind.o nodes.o utils.o
	$(CC) $(CFLAGS) -o myfind myfind.o nodes.o utils.o -lm

rootNode: rootNode.o nodes.o utils.o
	$(CC) $(CFLAGS) -o executables/rootNode rootNode.o nodes.o utils.o -lm

splitterMergerNode: splitterMergerNode.o nodes.o utils.o
	$(CC) $(CFLAGS) -o executables/splitterMergerNode splitterMergerNode.o nodes.o utils.o -lm

leafNode: leafNode.o nodes.o utils.o
	$(CC) $(CFLAGS) -o executables/leafNode leafNode.o nodes.o utils.o -lm

myfind.o:  myfind.c
	$(CC) $(CFLAGS) -c myfind.c

rootNode.o: nodes/rootNode.c
	$(CC) $(CFLAGS) -c nodes/rootNode.c

splitterMergerNode.o: nodes/splitterMergerNode.c
	$(CC) $(CFLAGS) -c nodes/splitterMergerNode.c

leafNode.o: nodes/leafNode.c
	$(CC) $(CFLAGS) -c nodes/leafNode.c

nodes.o:  nodes/nodes.c
	$(CC) $(CFLAGS) -c nodes/nodes.c

utils.o:  utils/utils.c
	$(CC) $(CFLAGS) -c utils/utils.c


clean: 
	rm -rf executables/* *.o