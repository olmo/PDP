
CC = mpicxx 
NAME = floyd
CFLAGS = -O3

MODULES = floyd.o Graph.o

all 		:	floyd
Graph.o		:       Graph.cc Graph.h
	$(CC) $(CFLAGS) -c Graph.cc
floyd.o   :	floyd.cc
	$(CC) $(CFLAGS) -c floyd.cc
floyd :		$(MODULES)
	$(CC) $(CFLAGS) $(MODULES) -o $(NAME) $(EXTRALIBS)
clean	:
	rm -f $(MODULES)  $(NAME)
