#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char color;
    char *movimiento;
    int indiceFila, indiceColumna;
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
        jugada->indiceFila = (atoi(&jugada->movimiento[1]) - 1);
        jugada->indiceColumna = (jugada->movimiento[0] - 'A');
    
    
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
    if ((jugada.indiceFila >= 8) || (jugada.indiceFila < 0) || (jugada.indiceColumna >= 8) || (jugada.indiceColumna < 0)) {
        (*resultado)->jugadaValida = false;
        (*resultado)->color = jugada.color;
        sprintf((*resultado)->error, "La jugada %s esta fuera del tablero y por lo tanto es una jugada invalida.", jugada.movimiento);
    } else {
        (*resultado)->jugadaValida = true;
    }
    return *resultado;
}

// jugadaRepetida: char** -> Jugada -> Resultado** -> Resultado*;
// Controlar que la posicion de la jugada no este ocupada.
Resultado* jugadaRepetida(char **tablero, Jugada jugada, Resultado **resultado) {
    if (tablero[jugada.indiceFila][jugada.indiceColumna] != 'X') {
        (*resultado)->jugadaValida = false;
        (*resultado)->color = jugada.color;
        (*resultado)->error = "La posicion de la jugada ya esta ocupada.";
    } else {
        (*resultado)->jugadaValida = true;
    }

    return *resultado;
}

// controlJugadaValida: char** -> Jugada -> Resultado** -> Resultado*;
// Controlar que la jugada realizada coincide con tener al menos una ficha enemiga alrededor suyo y, que a su vez,
// en esa direccion que se encuentra la ficha enemiga, haya una ficha aliada que encierre la/las enemigas.
Resultado* controlJugadaValida(char **tablero, Jugada jugada, Resultado **resultado) {
    char colorOpuesto;
    bool hayEnemigoAlrededor = false, hayFichasEncerradas = false;

    // Inicializando el almacenamiento de las posibles fichas enemigas encerradas por una aliada.
    Jugadas *fichasEncerradas = malloc(sizeof(Jugadas));
    fichasEncerradas->numeroDeJugadas = 0;
    fichasEncerradas->guardadoJugadas = malloc(sizeof(Jugada));

    (jugada.color == 'N') ? (colorOpuesto = 'B') : (colorOpuesto = 'N');

    for (int i = -1; i <= 1 ; i++) {
        for (int j = -1; j <= 1; j++) {
            // Sacando el control en la posicion de la jugada.
            if (i != 0 || j!= 0) {
                int indiceFilaVerificar = jugada.indiceFila + i;
                int indiceColumnaVerificar = jugada.indiceColumna + j;

                // Controlando que las jugadas a verificar no se salgan del tablero.
                bool fueraDelTablero = (indiceFilaVerificar >= 8) || (indiceFilaVerificar < 0) || (indiceColumnaVerificar >= 8) || (indiceColumnaVerificar < 0);
                
                //Inicializando el almacenamiento de las fichas enemigas.
                Jugadas *fichasEnemigas = malloc(sizeof(Jugadas));
                fichasEnemigas->numeroDeJugadas = 0;
                fichasEnemigas->guardadoJugadas = malloc(sizeof(Jugada));
                // Recorriendo los alrededores de la ficha del oponente para formar una linea (horizontal, vertical u oblicua).
                while (!fueraDelTablero && tablero[indiceFilaVerificar][indiceColumnaVerificar] == colorOpuesto) {
                    // Creando la ficha enemiga en forma de Jugada para poder guardarla.
                    Jugada *fichaEnemiga = malloc(sizeof(Jugada));
                    fichaEnemiga->indiceFila = indiceFilaVerificar;
                    fichaEnemiga->indiceColumna = indiceColumnaVerificar;

                    // Guardando la ficha enemiga en el almacenamiento de las fichas enemigas.
                    fichasEnemigas->guardadoJugadas = realloc(fichasEnemigas->guardadoJugadas, sizeof(Jugada) * (fichasEnemigas->numeroDeJugadas + 1));
                    fichasEnemigas->guardadoJugadas[fichasEnemigas->numeroDeJugadas] = *fichaEnemiga;
                    fichasEnemigas->numeroDeJugadas += 1;

                    // Avanzando de direccion.
                    indiceFilaVerificar += i;
                    indiceColumnaVerificar += j;

                    // Controlando de nuevo por la condicion del loop.
                    fueraDelTablero = (indiceFilaVerificar >= 8) || (indiceFilaVerificar < 0) || (indiceColumnaVerificar >= 8) || (indiceColumnaVerificar < 0);
                }

                // Controlando si hay una ficha enemiga alrededor.
                if (fichasEnemigas->numeroDeJugadas != 0) {
                    hayEnemigoAlrededor = true;

                    // Controlando que haya una ficha aliada que encierre a las enemigas.
                    if (!fueraDelTablero && tablero[indiceFilaVerificar][indiceColumnaVerificar] == jugada.color) {
                        hayFichasEncerradas = true;

                        // Guardando las fichas enemigas encerradas.
                        fichasEncerradas->guardadoJugadas = realloc(fichasEncerradas->guardadoJugadas, sizeof(Jugada) * (fichasEncerradas->numeroDeJugadas + fichasEnemigas->numeroDeJugadas));
                        for (int k = 0; k < fichasEnemigas->numeroDeJugadas; k++) {
                            fichasEncerradas->guardadoJugadas[fichasEncerradas->numeroDeJugadas + k] = fichasEnemigas->guardadoJugadas[k];
                        }
                        fichasEncerradas->numeroDeJugadas += fichasEnemigas->numeroDeJugadas;
                    } else {
                        for (int k = 0; k < fichasEnemigas->numeroDeJugadas; k++) {
                            free(&(fichasEnemigas->guardadoJugadas[k]));
                        }
                        
                        free(fichasEnemigas->guardadoJugadas);
                        free(fichasEnemigas);
                    }
                }
            }
        }
    }

    if (!hayEnemigoAlrededor) {
        (*resultado)->jugadaValida = false;
        (*resultado)->color = jugada.color;
        (*resultado)->error = "No hay fichas enemigos alrededor de la jugada realizada.";

        return *resultado;
    }

    if (!hayFichasEncerradas) {
        (*resultado)->jugadaValida = false;
        (*resultado)->color = jugada.color;
        (*resultado)->error = "Las fichas enemigas no se encierran con una ficha aliada.";

        return *resultado;
    }

    (*resultado)->jugadaValida = true;
    (*resultado)->fichasCapturadas = fichasEncerradas;

    return *resultado;
}

 // aplicarJugada: char** -> Jugada -> void;
 // Dada la jugada ya validada, la aplica sobre el tablero cambiando de color las fichas enemigas encerradas.
 void aplicarJugada(char **tablero, Jugada jugada, Resultado **resultado) {
    // Movimiento recien hecho del jugador.
    tablero[jugada.indiceFila][jugada.indiceColumna] = jugada.color;

    // Cambiando de color las fichas enemigas encerradas.
    for (int i = 0; i < (*resultado)->fichasCapturadas->numeroDeJugadas; i++) {
        tablero[(*resultado)->fichasCapturadas->guardadoJugadas[i].indiceFila][(*resultado)->fichasCapturadas->guardadoJugadas[i].indiceColumna] = jugada.color;

        // Liberando la memoria de las 'Jugada' structs.
        free(&((*resultado)->fichasCapturadas->guardadoJugadas[i]));
    }
    // Liberando la memoria de la 'Jugadas' struct
    free((*resultado)->fichasCapturadas->guardadoJugadas);
    free((*resultado)->fichasCapturadas);
 }

 // controlSalteoJugada:
 // Controlar que el salteo de jugada haya sido la unica opcion para avanzar con el juego.
Resultado* controlSalteoJugada(char **tablero, char colorJugador, Resultado **resultado) {
    Resultado *posibleResultado = malloc(sizeof(Resultado));
    bool jugadaPosible = false;

    int fila = 0;
    // Doble loop para recorrer el tablero.
    while (fila < 8 && !jugadaPosible) {
        int columna = 0;
        while (columna < 8 && !jugadaPosible) {
            // Condicional para verificar solamente posiciones con fichas de los jugadores.
            if (tablero[fila][columna] != 'X') {
                // Doble loop para verificar jugadas alrededor de la ficha.
                int i = -1;
                while (i < 2 && !jugadaPosible) {
                    int j = -1;
                    while (j < 2 &&!jugadaPosible) {
                        // Sumar/restar a los indices para verificar los alrededores.
                        int indiceFilaVerificar = fila + i;
                        int indiceColumnaVerificar = columna + j;

                        // Verificando que las posiciones no se salgan del tablero.
                        bool fueraDelTablero = (indiceFilaVerificar >= 8) || (indiceFilaVerificar < 0) || (indiceColumnaVerificar >= 8) || (indiceColumnaVerificar < 0);

                        // De las jugadas del alrededor, dejando solo las casillas vacias ya que no se pueden tomar
                        // en cuenta posibles jugadas en casillas ocupadas.
                        if (!fueraDelTablero && tablero[indiceFilaVerificar][indiceColumnaVerificar] == 'X'){
                            // Creando la jugada a verificar.
                            Jugada posibleJugada;
                            posibleJugada.color = colorJugador;
                            posibleJugada.movimiento = malloc(sizeof(char) * 3);
                            posibleJugada.movimiento[0] = indiceColumnaVerificar + 'A';
                            posibleJugada.movimiento[1] = indiceFilaVerificar + '1';
                            posibleJugada.movimiento[2] = '\0';
                            posibleJugada.indiceFila = (atoi(&posibleJugada.movimiento[1]) - 1);
                            posibleJugada.indiceColumna = (posibleJugada.movimiento[0] - 'A');

                            // Intentando la jugada.
                            posibleResultado = controlJugadaValida(tablero, posibleJugada, &posibleResultado);

                            // Si la jugada es valida, se encontro una posible jugada y el salto de turno es un error.
                            if (posibleResultado->jugadaValida == true) {
                                // Preparando la devolucion de la funcion.
                                (*resultado)->jugadaValida = false;
                                (*resultado)->color = colorJugador;
                                (*resultado)->error = "Se ha encontrado una posible jugada.";

                                // Evitar seguir probando posibles jugadas.
                                jugadaPosible = true;
                            }
                            free(posibleJugada.movimiento);
                        }
                        j += 1;
                    }
                    i += 1;
                }
            }
            columna += 1;
        }
        fila += 1;
    }
    // Return de la devolucion si la funcion encontro posible jugada.
    if (jugadaPosible) {
        // Liberando la memoria recibida por probar jugadas.
        for (int i = 0; i < posibleResultado->fichasCapturadas->numeroDeJugadas; i++) {
            free(&posibleResultado->fichasCapturadas->guardadoJugadas[i]);
        }
        free(posibleResultado->fichasCapturadas->guardadoJugadas);
        free(posibleResultado->fichasCapturadas);
        free(posibleResultado);
        
        return *resultado;
    }

    // Sino, return del resultado con la jugada validada.
    (*resultado)->jugadaValida = true;

    // Antes de returnear, liberar la memoria del posibleResultado.
    free(posibleResultado);

    return *resultado;
}

// simularJuego: char** -> Jugadas* -> ;
// Maneja la simulacion del juego. Si las jugadas son validas, las aplica.
void simularJuego(char*** tablero, Jugadas* jugadas) {
    int i = 0;
    Resultado *resultado = malloc(sizeof(Resultado));
    resultado->jugadaValida = true;
    while (i < jugadas->numeroDeJugadas && resultado->jugadaValida == true) {
        Jugada jugada = jugadas->guardadoJugadas[i];
        
        printf("  ABCDEFGH \n");
        printf(" ---------- \n");
        for (int j = 0; j < 8; j++) {
            printf("%d|",j+1);
            for (int k = 0; k < 8; k++) {
                printf("%c", (*tablero)[j][k]);
            }
            printf("|%d\n",j+1);
        }
        printf(" ---------- \n");
        printf("  ABCDEFGH \n\n");
        printf("jugada: %s, color: %c\n\n", jugada.movimiento, jugada.color);
        

        if (strcmp(jugada.movimiento, "SL") != 0) {
            // Validar que la jugada propuesta esta dentro del tablero.
            resultado = jugadaDentroTablero(jugada, &resultado);

            if (resultado->jugadaValida == false) {
                continue;
            }
            // Validar que la jugada no este repetida, o sea, que no haya ya una ficha en esa posicion.
            resultado = jugadaRepetida(*tablero, jugada, &resultado);

            if (resultado->jugadaValida == false) {
                continue;
            }

            // Aplicar las reglas de tener fichas enemigas alrededor y, a su vez, que en esa misma direccion
            // haya una ficha aliada que encierre las enemigas.
            resultado = controlJugadaValida(*tablero, jugada, &resultado);
            
            if (resultado->jugadaValida == false){
                continue;
            }
            
            // Aplicar la jugada al tablero.
            aplicarJugada(*tablero, jugada, &resultado);
        } else {
            // Controlar que no haya jugada posible mas que saltear el turno.
            resultado = controlSalteoJugada(*tablero, jugada.color, &resultado);

            if (resultado->jugadaValida == false) {
                continue;
            }
        }

        printf("  ABCDEFGH \n");
        printf(" ---------- \n");
        for (int j = 0; j < 8; j++) {
            printf("%d|",j+1);
            for (int k = 0; k < 8; k++) {
                printf("%c", (*tablero)[j][k]);
            }
            printf("|%d\n",j+1);
        }
        printf(" ---------- \n");
        printf("  ABCDEFGH \n\n");
        printf("\n---------------------------\n");

        
        i += 1;
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