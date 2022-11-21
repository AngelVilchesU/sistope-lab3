#include "funciones.h"

char *archivoEntradaGlobal;
enum
{
    THREAD_FAIL,
    THREAD_SUCCESS
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contadorLineasGlobal = 0;

void descriptorGlobal(char *nombre)
{
    archivoEntradaGlobal = nombre;
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