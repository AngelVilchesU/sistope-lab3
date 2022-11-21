#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#define LARGO_CHAR 300

typedef struct nodoGenerico
{
    int anio;
    float precioMasCaro;
    float precioMasBarato;
    char nombreJuegoMasCaro[LARGO_CHAR];
    char nombreJuegoMasBarato[LARGO_CHAR];
    int contadorJuegos;
    float sumaTotalJuegos;
    float promedioPrecioJuegos;
    int contadorW;
    int contadorMC;
    int contadorL;
    float porcentajeW;
    float porcentajeMC;
    float porcentajeL;
    char juegosGratis[LARGO_CHAR];
    struct nodoGenerico *siguiente;
} nodo;

typedef struct listaGenerica
{
    nodo *inicio;
} TDAlista;

void calculos(void *);

void validacionArgsOPT(int iFlag, int oFlag, int dFlag, int pFlag, int nFlag, int cFlag, char *argv[]);
