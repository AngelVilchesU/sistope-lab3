lab3: lab3.c funciones.o
	gcc -o lab3 lab3.c funciones.c
funciones.o: funciones.c funciones.h
	gcc -c funciones.c