#include "funciones.h"

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
                chunkAux--;
            }
            fgetsAux++;        
        }
        fclose(dctoEntrada);

        printf("contadorLineasGlobal: %d\n", contadorLineasGlobal);
        
        printf("ExitSC\n");
        pthread_mutex_unlock(&mutex);
        
        break;
        
    }

    pthread_exit((void *)THREAD_SUCCESS);
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