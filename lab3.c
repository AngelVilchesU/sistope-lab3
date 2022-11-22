#include "funciones.h"

extern TDAlista* listaCompartida;

int main(int argc, char *argv[])
{
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

    descriptorGlobal(nombreArchivoEntrada);

    FILE *dctoEntrada = fopen(nombreArchivoEntrada, "r");
    if (dctoEntrada == NULL)
    {
        printf("%s: error in input file named\n", nombreArchivoEntrada);
        exit(-1);
    }

    // Creación y llamado de hebras
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

    // Promedio de precios y porcentajes
    listaCompartida = actualizarPorcentajes(listaCompartida);
    
    recorrerListaAll(listaCompartida);

    // Creación archivo de salida
    FILE * archivoSalida; 
    archivoSalida = fopen(nombreArchivoSalida, "w"); // Se crea el archivo de salida dado el nombre ingresado por pantalla desde lab2.c
    imprimirEnFlujoDesdeAnio(listaCompartida, archivoSalida, atoi(anioInicioJuego)); // Se imprimen los resultados obtenidos en el flujo
    fclose(archivoSalida); // Se cierra el archivo            
    fclose(dctoEntrada);
    liberarLista(listaCompartida);
    return 0;
}