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

char *archivoEntradaGlobal;
enum
{
    THREAD_FAIL,
    THREAD_SUCCESS
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contadorLineasGlobal = 0;

void calculos(void *param)
{
    // Funciones que realicen los calculos necesarios
    while (true)
    {
        
        pthread_mutex_lock(&mutex);
        printf("<<<EnterSC>>>\n");
        // SC
        int chunkAux = atoi(param);
        int fgetsAux = 0;
        char linea[LARGO_CHAR];
        FILE *dctoEntrada = fopen(archivoEntradaGlobal, "r");
        while (fgets(linea, 300, dctoEntrada) != NULL && chunkAux != 0) 
        {
            if (fgetsAux == contadorLineasGlobal)
            {
                printf("linea: %s", linea);
                contadorLineasGlobal++;
                fgetsAux++;
            }
            else
            {
                fgetsAux++;
            }
            
            chunkAux--;
        }
        fclose(dctoEntrada);

        printf("contadorLineasGlobal: %d\n", contadorLineasGlobal);
        
        printf("ExitSC\n");
        pthread_mutex_unlock(&mutex);
        
        break;
    }

    pthread_exit((void *)THREAD_SUCCESS);
}

void descriptorGlobal(char *nombre)
{
    archivoEntradaGlobal = nombre;
}

void validacionArgsOPT(int iFlag, int oFlag, int dFlag, int pFlag, int nFlag, int cFlag, char *argv[])
{
    if (iFlag == 0) // Si no existe el nombre del archivo de entrada
    {
        fprintf(stderr, "%s: missing instruction -i flag parameter\n", argv[0]);
        exit(1);
    }
    if (oFlag == 0) // Si no existe el nombre del archivo de salida
    {
        fprintf(stderr, "%s: missing instruction -o flag parameter\n", argv[0]);
        exit(1);
    }
    if (dFlag == 0) // Si no existe el año de inicio del juego
    {
        fprintf(stderr, "%s: missing instruction -d flag parameter\n", argv[0]);
        exit(1);
    }
    if (pFlag == 0) // Si no existe el precio mínimo del juego
    {
        fprintf(stderr, "%s: missing instruction -p flag parameter\n", argv[0]);
        exit(1);
    }
    if (nFlag == 0) // Si no existe la cantidad de workers a generar
    {
        fprintf(stderr, "%s: missing instruction -n flag parameter\n", argv[0]);
        exit(1);
    }
    if (cFlag == 0) // Si no existe el tamaño del chunk
    {
        fprintf(stderr, "%s: missing instruction -c flag parameter\n", argv[0]);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    /************************************ Lógica de solución - punto 1 ************************************/
    char *nombreArchivoEntrada, *nombreArchivoSalida, *anioInicioJuego, *precioMinimoJuego, *cantidadHebras, *tamanioChunk;
    int iFlag = 0, oFlag = 0, dFlag = 0, pFlag = 0, nFlag = 0, cFlag = 0, bFlag = 0;
    int opt, err;
    while ((opt = getopt(argc, argv, "i:o:d:p:n:c:b")) != -1)
    {
        switch (opt)
        {
        case 'i': // Refiere al nombre del archivo de entrada
            iFlag = 1;
            nombreArchivoEntrada = optarg;
            break;
        case 'o': // Refiere al nombre del archivo de salida
            oFlag = 1;
            nombreArchivoSalida = optarg;
            break;
        case 'd': // Refiere al año desde el cual evaluar/considerar
            dFlag = 1;
            anioInicioJuego = optarg;
            break;
        case 'p': // Refiere al precio mínimo de juego/s
            pFlag = 1;
            precioMinimoJuego = optarg;
            break;
        case 'n': // Refiere a la cantidad de hebras a generar
            nFlag = 1;
            cantidadHebras = optarg;
            break;
        case 'c': // Refiere al tamaño del chunk
            cFlag = 1;
            tamanioChunk = optarg;
            break;
        case 'b': // Indica si los datos de salida deben mostrarse (o no) por pantalla
            bFlag = 1;
            break;
        case '?': // Otro caso
            err = 1;
            break;
        }
    }
    validacionArgsOPT(iFlag, oFlag, dFlag, pFlag, nFlag, cFlag, argv); // Validación de los parámetros ingresados
    printf("Nombre archivo entrada: %s\nNombre archivo salida: %s\nAnio inicio: %s\nPrecio minimo: %s\nCantidad hebras: %s\nTamanio Chunk: %s\nbFlag: %d\n", nombreArchivoEntrada, nombreArchivoSalida, anioInicioJuego, precioMinimoJuego, cantidadHebras, tamanioChunk, bFlag);

    descriptorGlobal(nombreArchivoEntrada);
    printf("Descriptor global: %s\n", archivoEntradaGlobal);

    FILE *dctoEntrada = fopen(archivoEntradaGlobal, "r");
    if (dctoEntrada == NULL)
    {
        printf("%s: error in input file named\n", archivoEntradaGlobal);
        exit(-1);
    }

    pthread_t threads[atoi(cantidadHebras)];
    int status;
    for (int i = 0; i < atoi(cantidadHebras); i++)
    {
        pthread_create(&threads[i], NULL, (void *)calculos, (void *)tamanioChunk);
    }
    for (int i = 0; i < atoi(cantidadHebras); i++)
    {
        pthread_join(threads[i], (void **)&status);
    }

    fclose(dctoEntrada);

    return 0;
}