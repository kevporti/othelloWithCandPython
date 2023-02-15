#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            lineTemp = "SL\n";
        }
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

//leerArchivoEntrada: char* ->
// Leer el archivo para desestructurar la informacion de la partida.
int leerArchivoEntrada(char *nombreArchivo) {
    FILE *archivo;
    archivo =  fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("El archivo %s no existe\n", nombreArchivo);
        return -1;
    }

    Jugadores jugadores = leerJugadores(archivo);
    Jugadas *jugadas = leerJugadas(archivo);
    

    // Liberacion de los espacios de memoria asignados.
    liberarMemoria(&jugadores, jugadas);

    fclose(archivo);
}

int main(int argc, char **argv) {
    
    leerArchivoEntrada(argv[1]);


    return 0;
}