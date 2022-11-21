#include "funciones.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

TDAlista *listaCompartida;

char *archivoEntradaGlobal;

int contadorLineasGlobal = 0;

void descriptorGlobal(char *nombre)
{
    archivoEntradaGlobal = nombre;
}

// Entradas: Recibe la variable Chunk necesaria para la lectura de líneas
// Salidas: no considera
// Descripción: Se emplea un ciclo while por cada hebra estableciendo su correspondiente...
//              ...entrada y salida de la sección crítica (mutex) de modo que, primeramente...
//              ...obtenemos el largo del archivo de entrada, necesario para establecer la...
//              ...condición de salida del ciclo mencionado anteriormente una vez se lee todo...
//              ...su contenido. En este sentido, se posee una variable global auxiliar que...
//              ...permite mantener constancia de la líneas leídas y en consecuencia, por leer...
//              ...de modo que mediante una lista enlazada compartida/global ingresamos los...
//              ...correspondiente datos de los juegos de acuerdo a su respectivo año...
//              ...insertandole si no existe y comparando en caso contrario (y actualizando)
void calculos(void *param)
{
    // Funciones que realicen los calculos necesarios
    while (true)
    {

        pthread_mutex_lock(&mutex); // ENTERSC
        // SC
        int chunkAux = atoi(param);
        int fgetsAux = 0;
        char lineaAux[LARGO_CHAR];
        char linea[LARGO_CHAR];
        int lineasTotales = 0;
        FILE *dctoEntrada = fopen(archivoEntradaGlobal, "r");

        while (fgets(lineaAux, 300, dctoEntrada) != NULL)
        {
            lineasTotales++;
        }
        rewind(dctoEntrada);
        while (fgets(linea, 300, dctoEntrada) != NULL && chunkAux != 0)
        {
            if (fgetsAux == contadorLineasGlobal)
            {
                if (contadorLineasGlobal == 0)
                {
                    listaCompartida = crearListaVacia();
                }

                int nroJuego = 1;
                int anio;
                float precioJuego;
                char nombreJuego[LARGO_CHAR] = "";
                int contadorJuegos;
                int contadorW = 0;
                int contadorMAC = 0;
                int contadorL = 0;
                char esGratis[LARGO_CHAR_MAX] = "";
                // Posiciones en la linea dada información entregada
                int posicionAct = 0;

                int posicionFinal = 10;
                int posicionEsL = 9;
                int posicionEsMC = 8;
                int posicionEsW = 7;
                int posicionEsGratis = 6;
                int posicionAnio = 5;
                int posicionPrecio = 3;
                int posicionNombreJuego = 1;

                char *token = strtok(linea, ","); // Se fragmente la línea recibida de acuerdo a un coma(,)
                while (token != NULL)             // Mientras queden elementos por evaluar en la línea fragmentada
                {
                    if (posicionAct == posicionNombreJuego) // Aquí token = nombre del juego
                    {
                        strcat(nombreJuego, token);
                    }
                    else if (posicionAct == posicionPrecio) // Aquí token = precio
                    {
                        precioJuego = atof(token);
                    }
                    else if (posicionAct == posicionAnio) // Aquí token = año
                    {
                        anio = atoi(token);
                    }
                    else if (posicionAct == posicionEsGratis) // Aquí token = si el juego es gratis
                    {
                        if (strcmp(token, "True") == 0) // Si el juego es gratis...
                        {
                            strcat(esGratis, nombreJuego);
                            strcat(esGratis, "\n");
                        }
                    }
                    else if (posicionAct == posicionEsW) // Aquí token = si el juego está disponible en Windows
                    {
                        if (strcmp(token, "Yes") == 0) // Si el juegos considera Windows...
                        {
                            contadorW = 1;
                        }
                    }
                    else if (posicionAct == posicionEsMC) // Aquí token = si el juego está disponible en Mac
                    {
                        if (strcmp(token, "Yes") == 0) // Si el juegos considera Mac...
                        {
                            contadorMAC = 1;
                        }
                    }
                    else if (posicionAct == posicionEsL) // Aquí token = si el juego está disponible en Linux
                    {
                        if (strcmp(token, "Yes\n") == 0) // Si el juegos considera Linux...
                        {
                            contadorL = 1;
                        }
                    }
                    token = strtok(NULL, ","); // Se continua al siguiente elemento
                    posicionAct++;             // Aumenta la posición del elemento
                }
                //  Si es lista enlazada es vacia, agrega la información obtenida
                if (esListaVacia(listaCompartida))
                {
                    insertarInicio(listaCompartida, anio, precioJuego,
                                   precioJuego, nombreJuego,
                                   nombreJuego, nroJuego,
                                   precioJuego,
                                   contadorW, contadorMAC, contadorL,
                                   esGratis);
                }
                // Si no está vacia, busca si los años coinciden
                else
                {
                    // Si coiciden, compara y procesa con la información obtenida del worker (deja uno solo)
                    if (aniosEquivalentes(listaCompartida, anio))
                    {
                        listaCompartida = actualizarNodo(listaCompartida, anio, precioJuego,
                                                         precioJuego, nombreJuego,
                                                         nombreJuego, nroJuego,
                                                         precioJuego,
                                                         contadorW, contadorMAC, contadorL,
                                                         esGratis);
                    }
                    else
                    {
                        insertarInicio(listaCompartida, anio, precioJuego,
                                       precioJuego, nombreJuego,
                                       nombreJuego, nroJuego,
                                       precioJuego,
                                       contadorW, contadorMAC, contadorL,
                                       esGratis);
                    }
                }
                contadorLineasGlobal++;
                chunkAux--;
            }
            fgetsAux++;
        }
        if (lineasTotales == contadorLineasGlobal)
        {
            pthread_mutex_unlock(&mutex);
            pthread_exit((void *)THREAD_SUCCESS);
        }
        fclose(dctoEntrada);
        pthread_mutex_unlock(&mutex); // EXITSC()
    }
}

// Entradas: Valor número (0 o 1 como booleanos) que indican el ingreso (o no) de parámetros
// Salidas: No posee salida, sin embargo, la función finaliza sin problemas siempre y cuando...
//          ...se ingresen los parámetros minimos obligatorios (sin considerar bFlag)
// Descripción: Se evalua el correcto ingreso de los parámetros necesarios para las operaciones...
//              ...a realizar durante la ejecución de los procesos, en caso de faltar uno,...
//              ...el proceso es abortado, caso contrario continua su ejecución
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

// Entradas: No considera
// Salidas: Una lista enlazada nula (pero con memoria reservada)
// Descripción: Se reserva la memoria necesaria para generar una lista enlaza inicialmente vacia
TDAlista *crearListaVacia()
{
    TDAlista *lista = (TDAlista *)malloc(sizeof(TDAlista));
    lista->inicio = NULL;
    return lista;
}

// Entradas: Una lista enlazada
// Salidas: No considera
// Descripción: Se libera la memoria utlizada en la lista enlazada
void liberarLista(TDAlista *lista)
{
    free(lista);
}

// Entradas: Una lista enlazada
// Salidas: 1 si la lista está vacia, caso contrario un 0
// Descripción: Se evalua la cabecera de la lista enlazada (nula o no) para determinar si es vacia o no
int esListaVacia(TDAlista *lista)
{
    if (lista->inicio == NULL)
        return 1;
    else
        return 0;
}

// Entradas: Una lista enlazada y un año como entero "base"
// Salidas: No considera
// Descripción: Imprime por pantalla la información en cada nodo de la lista enlazada...
//              ... desde un año especifico (el ingresado)
void recorrerLista(TDAlista *lista, int anioBase)
{
    if (!esListaVacia(lista)) // Si no es lista vacia
    {
        nodo *aux = lista->inicio;
        while (aux != NULL) // Mientras no sea nulo
        {
            if (aux->anio >= anioBase)
            {
                printf("Año %d:\nJuego mas caro: %s %f\nJuego mas barato: %s %f\nPromedio de precios: %f\nWindows: %f%% Mac: %f%% Linux: %f%%\nJuegos gratis:\n%s",
                       aux->anio, aux->nombreJuegoMasCaro, aux->precioMasCaro, aux->nombreJuegoMasBarato, aux->precioMasBarato, aux->promedioPrecioJuegos,
                       aux->porcentajeW, aux->porcentajeMC, aux->porcentajeL, aux->juegosGratis);

                aux = aux->siguiente; // Siguiente nodo
            }
        }
    }
    else // Si es lista vacia
        printf("La lista está vacía\n");
}

// Entradas: Una lista enlazada y todos los datos de acuerdo a su estructura definida en fbroker.h
// Salidas: No considera
// Descripción: Dado los datos ingresados, estos son adicionados en un nuevo nodo de la lista enlazada ingresada
void insertarInicio(TDAlista *lista, int anio, float precioMasCaro,
                    float precioMasBarato, char *nombreJuegoMasCaro,
                    char *nombreJuegoMasBarato, int contadorJuegos,
                    float sumaTotalJuegos,
                    int contadorW, int contadorMC, int contadorL,
                    char *juegosGratis)
{
    nodo *nuevo = (nodo *)malloc(sizeof(nodo));
    nuevo->anio = anio;
    nuevo->precioMasCaro = precioMasCaro;
    nuevo->precioMasBarato = precioMasBarato;
    strcpy(nuevo->nombreJuegoMasCaro, nombreJuegoMasCaro);
    strcpy(nuevo->nombreJuegoMasBarato, nombreJuegoMasBarato);
    nuevo->contadorJuegos = contadorJuegos;
    nuevo->sumaTotalJuegos = sumaTotalJuegos;
    nuevo->contadorW = contadorW;
    nuevo->contadorMC = contadorMC;
    nuevo->contadorL = contadorL;
    strcpy(nuevo->juegosGratis, juegosGratis);
    nuevo->siguiente = lista->inicio;
    lista->inicio = nuevo;
}

// Entradas: Una lista enlazada y todos los datos de acuerdo a su estructura definida en fbroker.h
// Salidas: Una lista enlazada actualizada
// Descripción: Dados los parámetros recibidos, estos con procesados en el nodo en el cual los años...
//              ...coincidan comparando la información existente y la información ingresada considerando...
//              ...los calculos necesarios dado el enunciado del laboratorio
TDAlista *actualizarNodo(TDAlista *lista, int anio, float precioMasCaro,
                         float precioMasBarato, char *nombreJuegoMasCaro,
                         char *nombreJuegoMasBarato, int contadorJuegos,
                         float sumaTotalJuegos,
                         int contadorW, int contadorMC, int contadorL,
                         char *juegosGratis)
{
    if (!esListaVacia(lista)) // si la lista no es vacia...
    {
        nodo *auxiliar = lista->inicio;
        while (auxiliar != NULL) // Mientras no sea nula
        {
            if (auxiliar->anio == anio) // Si el año coincide
            {
                // La información se actualiza en el nodo ya existente en la LE de acuerdo a los parámetros obtenidos
                // Puntos a considerar
                // Referente al año, ninguno
                // Referente al nombre del juego más caro, obtenerlo en la línea de código del precio más caro
                char auxStr[LARGO_CHAR];
                strcpy(auxStr, auxiliar->nombreJuegoMasCaro);
                strcpy(auxiliar->nombreJuegoMasCaro, "");
                strcpy(auxiliar->nombreJuegoMasCaro, strJuegoMasCaro(auxiliar->precioMasCaro, auxStr, precioMasCaro, nombreJuegoMasCaro));

                // Referente al nombre del juego más barato, obtenerlo en la línea de código del precio más barato
                strcpy(auxStr, "");
                strcpy(auxStr, auxiliar->nombreJuegoMasBarato);
                strcpy(auxiliar->nombreJuegoMasBarato, "");
                strcpy(auxiliar->nombreJuegoMasBarato, strJuegoMasBarato(auxiliar->precioMasBarato, auxStr, precioMasBarato, nombreJuegoMasBarato));

                // Referente al precio más caro, comparar entre la info. obtenida y el nodo del mismo año (extraer el mayor)
                auxiliar->precioMasCaro = valorMayor(auxiliar->precioMasCaro, precioMasCaro);

                // Referente al precio más barato, comparar entre la info. obtenida y el nodo del mismo año (extraer el menor)
                auxiliar->precioMasBarato = valorMenor(auxiliar->precioMasBarato, precioMasBarato);

                // Referente al contador de juegos, se le suma 1 al nodo con juegos del mismo año (esta condición if)
                auxiliar->contadorJuegos++;

                // Referente a la suma total de juegos, sumar precio del juego existente en el nodo y el obtenido
                auxiliar->sumaTotalJuegos = auxiliar->sumaTotalJuegos + precioMasCaro; // Es lo mismo que precioJuego

                // Referente al contador de juegos Windows, sumarlo en caso de que corresponda, lo mismo con MC y L
                if (contadorW)
                {
                    auxiliar->contadorW++;
                }
                if (contadorMC)
                {
                    auxiliar->contadorMC++;
                }
                if (contadorL)
                {
                    auxiliar->contadorL++;
                }

                // Referente a los juegos gratis, contatenar nombre del juego obtenido si es gratis con la línea ya...
                // ...existente en el nodo
                strcat(juegosGratis, "\n");
                strcat(auxiliar->juegosGratis, juegosGratis);
                return lista; // Finaliza el proceso y se retorna la lista enlazada actualizada
            }
            auxiliar = auxiliar->siguiente; // Siguiente elemento
        }
        return lista; // Retorno de la lista sin modificación (en caso de no existir año)
    }
    else // Es lista vacia
        return lista;
}

// Entradas: Una lista enlazada y un año
// Salidas: 1 si el años existe en la lista enlazada, 0 si no existe
// Descripción: Se evalua la existencia de un año en los nodos de la lista enlazada...
//              que coincida con el ingresado
int aniosEquivalentes(TDAlista *lista, int anio)
{
    if (!esListaVacia(lista)) // Si la lista no es vacia...
    {
        nodo *auxiliar = lista->inicio;
        while (auxiliar != NULL) // Mientras no sea nula
        {
            if (auxiliar->anio == anio) // Si los años coinciden
            {
                return 1; // retorna 1
            }
            auxiliar = auxiliar->siguiente; // Siguiente nodo
        }
        return 0;
    }
    else // Si la lista es vacia
        return 0;
}

// Entradas: Dos valores flotantes (precios en dolares)
// Salidas: El valor mayor entre los ingresados
// Descripción: Se comparan los valores ingresados en búsqueda del mayor
float valorMayor(float valorA, float valorB)
{
    if (valorA >= valorB)
    {
        return valorA;
    }
    return valorB;
}

// Entradas: Dos valores flotantes (precios en dolares)
// Salidas: El valor menor entre los ingresados
// Descripción: Se comparan los valores ingresados en búsqueda del menor
float valorMenor(float valorA, float valorB)
{
    if (valorA <= valorB)
    {
        return valorA;
    }
    return valorB;
}

// Entradas: Dos valores flotantes (precios en dolares) asociados a nombres (de juego)
// Salidas: El nombre asociado al valor mayor
// Descripción: Se comparan los valores ingresados en búsqueda del mayor retornando su nombre asociado
char *strJuegoMasCaro(float valorA, char *juegoA, float valorB, char *juegoB)
{
    if (valorA >= valorB)
    {
        return juegoA;
    }
    return juegoB;
}

// Entradas: Dos valores flotantes (precios en dolares) asociados a nombres (de juego)
// Salidas: El nombre asociado al valor menor
// Descripción: Se comparan los valores ingresados en búsqueda del menor retornado su nombre asociado
char *strJuegoMasBarato(float valorA, char *juegoA, float valorB, char *juegoB)
{
    if (valorA <= valorB)
    {
        return juegoA;
    }
    return juegoB;
}

TDAlista *actualizarPorcentajes(TDAlista *lista)
{
    if (!esListaVacia(lista)) // Si no es lista vacia
    {
        nodo *aux = lista->inicio;
        while (aux != NULL) // Mientras no sea nulo
        {
            aux->promedioPrecioJuegos = aux->sumaTotalJuegos / aux->contadorJuegos;
            aux->porcentajeW = ((aux->contadorW + 0.0) / aux->contadorJuegos) * 100.0;
            aux->porcentajeMC = ((aux->contadorMC + 0.0) / aux->contadorJuegos) * 100.0;
            aux->porcentajeL = ((aux->contadorL + 0.0) / aux->contadorJuegos) * 100.0;

            aux = aux->siguiente; // Siguiente nodo
        }
        return lista;
    }
    else // Si es lista vacia
        printf("La lista está vacía\n");
}

// Entradas: Una lista enlazada, el flujo de escritura y un año
// Salidas: No considera
// Descripción: Dada la lista enlazada, su contenido es impreso en el flujo ingresado...
//              ...considerando un filtro de los años en cada nodo de la lista enlazada
void imprimirEnFlujoDesdeAnio(TDAlista *lista_enlace, FILE *flujo, int anioBase)
{
    nodo *aux = lista_enlace->inicio;
    while (aux != NULL)
    {
        if (aux->anio >= anioBase)
        {
            fprintf(flujo, "Año %d:\nJuego mas caro: %s %f\nJuego mas barato: %s %f\nPromedio de precios: %f\nWindows: %f%% Mac: %f%% Linux: %f%%\nJuegos gratis:\n%s",
                    aux->anio, aux->nombreJuegoMasCaro, aux->precioMasCaro, aux->nombreJuegoMasBarato, aux->precioMasBarato, aux->promedioPrecioJuegos,
                    aux->porcentajeW, aux->porcentajeMC, aux->porcentajeL, aux->juegosGratis);
        }
        aux = aux->siguiente;
    }
}