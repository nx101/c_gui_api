CC = gcc
EXEC = ArrayListUnitTest
OBJs_PATH = ArrayList.o ArrayListUnitTest.o ../IO/BasicErrorsHandling.o
OBJS = ArrayList.o ArrayListUnitTest.o BasicErrorsHandling.o

COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJs_PATH)
	$(CC) $(OBJS) -o $@

ArrayListUnitTest.o: ArrayListUnitTest.c ArrayList.h
	$(CC) $(COMP_FLAG) -c $*.c

ArrayList.o: ArrayList.c ArrayList.h ../IO/BasicErrorsHandling.h
	$(CC) $(COMP_FLAG) -c $*.c

../IO/BasicErrorsHandling.o: ../IO/BasicErrorsHandling.c ../IO/BasicErrorsHandling.h
	$(CC) $(COMP_FLAG) -c $*.c

clean:
	rm -f *.o $(EXEC)
