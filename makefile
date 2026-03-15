OBJ=$(patsubst %.c,%.o,$(wildcard *.c))
a.exe : $(OBJ)
	gcc -o $@ $^
clean : 
	rm *.exe *.o
