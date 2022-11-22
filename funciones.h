#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#define LARGO_CHAR 300
#define LARGO_CHAR_MAX 9999

enum
{
    THREAD_FAIL,
    THREAD_SUCCESS
};

void descriptorGlobal(char *);


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

TDAlista *crearListaVacia();

void liberarLista(TDAlista* );

void recorrerLista(TDAlista *, int);

int esListaVacia(TDAlista *);

void insertarInicio(TDAlista *, int , float , 
                    float , char* ,
                    char* , int ,
                    float ,
                    int , int , int ,
                    char* );


TDAlista *actualizarNodo(TDAlista *, int , float , 
                    float , char* ,
                    char* , int ,
                    float ,
                    int , int , int ,
                    char* );

int aniosEquivalentes(TDAlista* , int);


char* strJuegoMasCaro(float , char* , float , char* );

char* strJuegoMasBarato(float , char* , float , char* );

float promedio(float , float );

float valorMayor(float , float );

float valorMenor(float , float );
TDAlista *actualizarPorcentajes(TDAlista *);

void imprimirEnFlujoDesdeAnio(TDAlista *, FILE *, int );

void recorrerListaAll(TDAlista *);
