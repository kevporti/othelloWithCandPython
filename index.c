#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char color;
    char *movimiento;
} Jugada;

typedef struct {
    Jugada *guardadoJugadas;
    int numeroDeJugadas;
} Jugadas;

typedef struct {
    char *nombre;
    char color;
} Jugador;

typedef struct {
    Jugador *jugadorUno;
    Jugador *jugadorDos;
} Jugadores;

typedef struct {
    bool jugadaValida;
    char color, *error;
    Jugadas *fichasCapturadas;
} Resultado;

// eliminarSaltoDeLinea: char* -> char*;
// Funcion auxiliar para eliminar los saltos de linea indeseados.
char* eliminarSaltoDeLinea(char* linea) {
    int len = strlen(linea);
    if (len > 0 && linea[len - 1] == '\n') {
        linea[len - 1] = '\0';
    }

    return linea;
}

// leerJugadores: FILE* -> Jugadores;
// Desestructurando los jugadores del archivo. Recibe a una variable de tipo 'FILE*', lee las primeras dos lineas y las desestructura para separar
// En una variable de tipo 'Jugador' el nombre y el color del jugador. Luego junta a los dos jugadores en una variable de tipo 'Jugadores' y la devuelve.
Jugadores leerJugadores(FILE *archivo) {
    char nameTemp[10], colorTemp;
    Jugadores jugadores;
    // Loop para leer dos lineas, desestructurarlas y almacenarlas.
    for (int i = 0; i < 2; i++) {
        Jugador *jugador = malloc(sizeof(Jugador));

        // Escaneando el nombre del jugador.
        fscanf(archivo, "%[^,]", nameTemp);
        jugador->nombre = malloc(sizeof(char) * strlen(nameTemp) + 1);
        strcpy(jugador->nombre, nameTemp);
        fgetc(archivo);

        // Escaneando el color del jugador.
        colorTemp = fgetc(archivo);
        jugador->color = colorTemp;
        fgetc(archivo);

        // Guardando el jugador en la estructura a devolver.
        if (i == 0) {
            jugadores.jugadorUno = jugador;
        } else {
            jugadores.jugadorDos = jugador;
        }
    }

    return jugadores;
}

// leerJugadas: FILE* -> Jugadas;
// Desestructurando las lineas de jugadas hechas. Guardar las Jugada struct dentro de Jugadas->guardadoJugadas[].
Jugadas* leerJugadas(FILE *archivo) {
    Jugadas *jugadas = malloc(sizeof(Jugadas));
    jugadas->numeroDeJugadas = 0;
    jugadas->guardadoJugadas = malloc(sizeof(Jugada));
    char *lineTemp = NULL, colorInicial, colorOpuesto;
    size_t lineSize = 3;

    // Estableciendo los colores del juego.
    colorInicial = fgetc(archivo);
    (colorInicial == 'N') ? (colorOpuesto = 'B') : (colorOpuesto = 'N');
    fgetc(archivo);


    while (getline(&lineTemp, &lineSize, archivo) != -1) {
        Jugada *jugada = malloc(sizeof(Jugada));
        jugada->movimiento = malloc(sizeof(char) * (lineSize + 1));

        // Estableciendo la jugada.
        (jugadas->numeroDeJugadas % 2 == 0) ? (jugada->color = colorInicial) : (jugada->color = colorOpuesto);
        // Cambiando el contenido de lineTemp si es un paso de turno.
        if (*lineTemp == '\n') {
            lineTemp = "SL";
        }
        lineTemp = eliminarSaltoDeLinea(lineTemp);
        
        strcpy(jugada->movimiento, lineTemp);
    
    
        jugadas->guardadoJugadas = realloc(jugadas->guardadoJugadas, sizeof(Jugada) * (jugadas->numeroDeJugadas + 1));
        // Almacenando todas las jugadas en una misma estructura y seteando las variables para el Loop.
        jugadas->guardadoJugadas[jugadas->numeroDeJugadas] = *jugada;
        jugadas->numeroDeJugadas++;
        lineTemp = NULL;
    }

    return jugadas;
}

// liberarMemoria: Jugadores* -> Jugadas* -> void;
// Liberando la memoria obtenida dinamicamente de los jugadores y de las jugadas.
void liberarMemoria(Jugadores *jugadores, Jugadas *jugadas) {
    // Liberacion de memoria de los jugadores.
    free(jugadores->jugadorDos->nombre);
    free(jugadores->jugadorUno->nombre);
    free(jugadores->jugadorDos);
    free(jugadores->jugadorUno);

    for (int i = 0; i < jugadas->numeroDeJugadas; i++) {
        free(jugadas->guardadoJugadas[i].movimiento);
        free(&jugadas->guardadoJugadas[i]);
    }
        free(jugadas->guardadoJugadas);
        free(jugadas);
}

// jugadaDentroTablero: Jugada -> ;
// Validar que la jugada hecha este dentro de los parametros delimitados por el tablero.
Resultado* jugadaDentroTablero(Jugada jugada, Resultado **resultado) {
    int indiceFilaJugada = (atoi(&jugada.movimiento[1]) - 1);
    int indiceColumnaJugada = (jugada.movimiento[0] - 'A');

    if ((indiceFilaJugada >= 8) || (indiceFilaJugada < 0) || (indiceColumnaJugada >= 8) || (indiceColumnaJugada < 0)) {
        printf("Error con la jugada: %s\n", jugada.movimiento);
        (*resultado)->jugadaValida = false;
        (*resultado)->color = jugada.color;
        sprintf((*resultado)->error, "La jugada %s esta fuera del tablero y por lo tanto es una jugada invalida.", jugada.movimiento);
    } else {
        (*resultado)->jugadaValida = true;
    }
    return *resultado;
}

// simularJuego: char** -> Jugadas* -> ;
// Maneja la simulacion del juego. Si las jugadas son validas, las aplica.
void simularJuego(char*** tablero, Jugadas* jugadas) {
    int i = 0;
    Resultado *resultado = malloc(sizeof(Resultado));

    while (i < jugadas->numeroDeJugadas && (i == 0 || resultado->jugadaValida == true)) {
        Jugada jugada = jugadas->guardadoJugadas[i];

        if (strcmp(jugada.movimiento, "SL") != 0) {
            // Validar que la jugada propuesta esta dentro del tablero.
            resultado = jugadaDentroTablero(jugada, &resultado);

            if (resultado->jugadaValida == false) {
                continue;
            }
            
            // Validar que la jugada no este repetida, o sea, que no haya ya una ficha en esa posicion.

            // Aplicar las reglas de tener fichas enemigas alrededor y, a su vez, que en esa misma direccion
            // haya una ficha aliada que encierre las enemigas.

            // Aplicar la jugada al tablero.
        } else {
            
        }
        
        i++;
    }
    
}

// tableroInicial: void -> char**;
// Creando el tablero inicial en forma de doble puntero.
char** tableroInicial() {
    char **tablero = malloc(sizeof(char *) * 8);

    for (int i = 0; i < 8; i++) {
        tablero[i] = malloc(sizeof(char) * 8);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i == 3 && j == 3) || (i == 4 && j == 4)) {
                tablero[i][j] = 'B';
            } else if ((i == 3 && j == 4) || (i == 4 && j == 3)) {
                tablero[i][j] = 'N';
            } else {
                tablero[i][j] = 'X';
            }
        }
    }

    return tablero;
}

//leerArchivoEntrada: char* ->
// Leer el archivo para desestructurar la informacion de la partida.
int leerArchivoEntrada(char *nombreArchivo) {
    FILE *archivo;
    archivo =  fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("El archivo %s no existe\n", nombreArchivo);
        return -1;
    }

    // Leer informacion del archivo.
    Jugadores jugadores = leerJugadores(archivo);
    Jugadas *jugadas = leerJugadas(archivo);
    
    char **tablero = tableroInicial();

    simularJuego(&tablero, jugadas);


    // Liberacion de los espacios de memoria asignados.
    liberarMemoria(&jugadores, jugadas);

    fclose(archivo);
}

int main(int argc, char **argv) {
    
    leerArchivoEntrada(argv[1]);


    return 0;
}