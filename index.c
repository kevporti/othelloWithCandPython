/*---------------- DISEÑO DE DATOS ----------------
El TABLERO esta representado como un arreglo de arreglos de tipo char, siendo cada fila un arreglo de caracteres,
y cada casilla siendo un caracter. Dentro de cada casilla se asigna 'X' si es una posicion vacia, o la inicial del
color del jugador que tiene ficha en esa posicion ('B' o 'N').
Los JUGADORES se representan cada uno con una estructura, donde se guarda el nombre del jugador (almacenado con
memoria dinamica), y la inicial del color que tiene asignado en el juego ('B' o 'N').
El conjunto de las JUGADAS se representa con una estructura custom que guarda la cantidad de jugadas y un arreglo
de estructuras de tipo Jugada. Esta nueva estructura Jugada representa cada jugada leida del archivo. Aqui se guarda
el movimiento realizado, el color que lo realizo, y los indices en fila y columna de la jugada realizada (Siendo A=0
, B= 1,... el indice de la columna y el de las filas siendo el numero dado menos uno).
El RESULTADO (creado para realizar un seguimiento de los resultados de la verificacion de reglas en cada jugada), esta
representado con una estructura con varias banderas (con tipo booleano) de jugadaValida, partidaIncompleta, dobleSalto
y partidaValida, con un puntero a una tipo de Jugadas para guardar las fichas encerradas cuando se realizan movmientos
y con el seguimiento del color del jugador (con un tipo char), y el mensaje de error si es que hubo.
La estructura Archivo esta creada para poder devolver a los Jugadores y las Jugadas, para poder crear
funciones menos dependientes unas de otras. Tambien guarda si el archivo fue abierto correctamente o no.
-------------------------------------------------
*/

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
    Jugador *jugadores;
} Jugadores;

typedef struct {
    bool jugadaValida, partidaIncompleta, partidaValida, dobleSalteo;
    char color, *error;
    Jugadas *fichasCapturadas;
} Resultado;

typedef struct {
    Jugadas *jugadasLeidas;
    Jugadores *jugadores;
    int aperturaArchivo;
} Archivo;

// eliminarSaltoDeLinea: char* -> char*;
// Funcion auxiliar para eliminar los saltos de linea indeseados.
char* eliminarSaltoDeLinea(char* linea) {
    int len = strlen(linea);
    if (len > 0 && linea[len - 1] == '\n') {
        linea[len - 1] = '\0';
    }

    return linea;
}

// leerJugadores: FILE* -> Jugadores*;
// Desestructurando los jugadores del archivo. Recibe a una variable de tipo 'FILE*', lee las primeras dos lineas y las desestructura para separar
// En una variable de tipo 'Jugador' el nombre y el color del jugador. Luego junta a los dos jugadores en una variable de tipo 'Jugadores' y la devuelve.
Jugadores* leerJugadores(FILE *archivo) {
    char nameTemp[10], colorTemp;
    Jugadores *jugadores = malloc(sizeof(Jugadores));
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
        jugadores->jugadores[i] = *jugador;
    }

    return jugadores;
}

// leerJugadas: FILE* -> Jugadas*;
// Desestructurando las lineas de jugadas hechas. Guardar cada Jugada struct dentro de Jugadas->guardadoJugadas formando un array.
// Las jugadas que representan salto de turno se guardaran como "SL" para trabajar en el mismo formato que las jugadas normales.
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

    // Recorriendo el resto del archivo hasta el final.
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
        // Almacenando todas las jugadas en una misma estructura y reestableciendo las variables para el Loop.
        jugadas->guardadoJugadas[jugadas->numeroDeJugadas] = *jugada;
        jugadas->numeroDeJugadas++;
        lineTemp = NULL;
    }

    return jugadas;
}

// liberarMemoria: Jugadores* -> Jugadas* -> Resultado* -> void;
// Liberando la memoria obtenida dinamicamente de los jugadores y de las jugadas.
void liberarMemoria(Jugadores *jugadores, Jugadas *jugadas, Resultado *resultado) {
    // Liberacion de memoria de los jugadores.
    for (int i = 0; i < 2; i++) {
        free(jugadores->jugadores[i].nombre);
        free(&(jugadores->jugadores[i]));
    }
    free(jugadores->jugadores);
    free(jugadores);
    
    // Liberacion de memoria de las jugadas.
    for (int i = 0; i < jugadas->numeroDeJugadas; i++) {
        free(jugadas->guardadoJugadas[i].movimiento);
        free(&jugadas->guardadoJugadas[i]);
    }
    free(jugadas->guardadoJugadas);
    free(jugadas);

    // Liberacion de memoria del resultado.
    free(resultado->error);
    free(resultado->fichasCapturadas->guardadoJugadas);
    free(resultado->fichasCapturadas);
}

// jugadaDentroTablero: Jugada -> Resultado** -> Resultado*;
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
// Controlar que la posicion de la jugada no este ocupada, debido a que si lo esta, seria una jugada repetida.
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
                        // Liberando la memoria de las fichasEnemigas ya que no va a ser utilizada.
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
 // Dada la jugada ya validada, la aplica sobre el tablero cambiando de color las fichas enemigas encerradas y asi completando
 // el turno del jugador.
 void aplicarJugada(char **tablero, Jugada jugada, Resultado **resultado) {
    // Movimiento recien hecho del jugador.
    tablero[jugada.indiceFila][jugada.indiceColumna] = jugada.color;

    // Cambiando de color las fichas enemigas encerradas.
    for (int i = 0; i < (*resultado)->fichasCapturadas->numeroDeJugadas; i++) {
        tablero[(*resultado)->fichasCapturadas->guardadoJugadas[i].indiceFila][(*resultado)->fichasCapturadas->guardadoJugadas[i].indiceColumna] = jugada.color;

        // Liberando la memoria de las 'Jugada' structs.
        free(&((*resultado)->fichasCapturadas->guardadoJugadas[i]));
    }
    // Liberando la memoria de la 'Jugadas' struct.
    free((*resultado)->fichasCapturadas->guardadoJugadas);
    free((*resultado)->fichasCapturadas);
 }

 // controlSalteoJugada: char** -> char -> Resultado** -> Resultado*;
 // Controlar que el salteo de jugada haya sido la unica opcion para avanzar con el juego. Para ello hay que verificar que el jugador no
 // haya podido colocar una ficha en ningun lugar del tablero. Recorreremos el tablero, nos posicionamos en los lugares donde hayan fichas
 // de los jugadores y verificamos que a su alrededor hayan posiciones vacias. Luego, en esas posiciones vacias, simular jugadas y al menos
 // una da como valida, significa que si habian movimientos posibles.
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

// simularJuego: char*** -> Jugadas* -> Resultado*;
// Maneja la simulacion del juego. Manda a verificar las reglas de la jugada, y si las cumple, la aplica. Luego determina
// si la simulacion del juego con las jugadas resulto en una partida incompleta o no, si hubo doble salto de turno, o si
// hubo algun error durante el procesamiento de las jugadas.
Resultado* simularJuego(char*** tablero, Jugadas* jugadas) {
    char color;
    int i = 0;
    Resultado *resultado = malloc(sizeof(Resultado));
    resultado->jugadaValida = true;

    while (i < jugadas->numeroDeJugadas && resultado->jugadaValida == true) {
        Jugada jugada = jugadas->guardadoJugadas[i];
        color = jugada.color;

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
        
        i += 1;
    }
    
    // Una vez terminado el recorrido de las jugadas y, si no se produjo ningun error, determinar si la partida 
    // esta completa o no. Si hubo algun error en los movimientos lo reporta.
    resultado->partidaIncompleta = false;

    if (resultado->jugadaValida) {
        char colorSiguiente;
        color == 'B' ? (colorSiguiente = 'N') : (colorSiguiente = 'B');

        // Simular un salteo de jugada para ver si era posible una nueva jugada.
        resultado = controlSalteoJugada(*tablero, colorSiguiente, &resultado);

        if (!(resultado->jugadaValida)) {
            // Si la simulacion da jugada invalida, es porque habia una jugada posible y la partida esta incompleta.
            resultado->partidaIncompleta = true;
            resultado->dobleSalteo = false;
        } else {
            // Si el salteo fue valido, entonces hay que verificar un segundo salteo para comprobar que no se podia
            // seguir jugando.
            colorSiguiente == 'B' ? (colorSiguiente = 'N') : (colorSiguiente = 'B');

            // Simulando el segundo salteo de jugada.
            resultado = controlSalteoJugada(*tablero, colorSiguiente, &resultado);

            if (!(resultado->jugadaValida)) {
                // Si la segunda simulacion da resultado de jugada invalida, entonces habia jugada posible y la partida
                // esta incompleta.
                resultado->partidaIncompleta = true;
                resultado->dobleSalteo = true;
            } else {
                // Si el salteo de la segunda simulacion es valido, entonces la partida no esta incompleta.
                resultado->partidaIncompleta = false;
            }   
        }
    }
    
    if (!(resultado->jugadaValida)) {
        resultado->partidaValida = false;
        
        return resultado;
    }

    resultado->partidaValida = true;
    return resultado;
}

// determinarGanador: char ** -> char;
// Recibe el tablero y contando las fichas del tablero determina que jugador gano o si fue un empate.
char determinarGanador(char **tablero) {
    int blancas = 0, negras = 0;

    // Contar las fichas de cada color.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (tablero[i][j] == 'B') {
                blancas += 1;
            } else if (tablero[i][j] == 'N') {
                negras += 1;
            }
        }
    }

    // Definir un resultado.
    if (blancas > negras) {
        return 'B';
    } else if (blancas < negras) {
        return 'N';
    } else {
        return 'E';
    }
}

// tableroInicial: void -> char**;
// Creando el tablero inicial en forma de doble puntero.
char** tableroInicial(void) {
    char **tablero = malloc(sizeof(char *) * 8);

    // Asignando memoria para todas las casillas.
    for (int i = 0; i < 8; i++) {
        tablero[i] = malloc(sizeof(char) * 8);
    }

    // Colocando las fichas vacias y las iniciales de los colores de los jugadores en el centro del tablero.
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

//leerArchivoEntrada: char* -> Archivo*;
// Leer el archivo para desestructurar la informacion de la partida.
Archivo* leerArchivoEntrada(char *nombreArchivo) {
    Archivo *datosArchivo = malloc(sizeof(Archivo));
    FILE *archivo;
    archivo =  fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("El archivo %s no existe\n", nombreArchivo);
        datosArchivo->aperturaArchivo = -1;
        return datosArchivo;
    }
    datosArchivo->aperturaArchivo = 0;

    // Leer informacion del archivo.
    datosArchivo->jugadores = malloc(sizeof(Jugador) * 2);
    datosArchivo->jugadores = leerJugadores(archivo);
    Jugadas *jugadas = leerJugadas(archivo);
    datosArchivo->jugadasLeidas = malloc(sizeof(Jugada) * jugadas->numeroDeJugadas);
    datosArchivo->jugadasLeidas->numeroDeJugadas = jugadas->numeroDeJugadas;
    datosArchivo->jugadasLeidas->guardadoJugadas = jugadas->guardadoJugadas;

    fclose(archivo);
    return datosArchivo;
}

// volcarTableroEnArchivo: char* -> char** -> char -> void;
// Abrir el archivo de salida para guardar la informacion del tablero y del color del jugador que le toca mover.
void volcarTableroEnArchivo(char* nombreArchivo, char** tablero, char colorSiguiente) {
    FILE *archivoSalida;
    archivoSalida = fopen(nombreArchivo, "w");

    if (archivoSalida == NULL) {
        printf("No se ha podido abrir el archivo %s", nombreArchivo);
    }
    
    // Pasando el formato del tablero al archivo de salida.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            fputc(tablero[i][j], archivoSalida);
        }
        fputc('\n', archivoSalida);
    }

    // Pasando el color del jugador que sigue.
    fputc(colorSiguiente, archivoSalida);
    
    fclose(archivoSalida);
}

// main: int -> char** -> int;
// Maneja la logica general. Manda a desestructurar el archivo, crea el tablero, simula el juego, muestra por consola
// el tablero, determina el ganador o no y en base a eso escribe el archivo de salida y libera la memoria almacenada
// restante.
int main(int argc, char **argv) {
    Archivo *datosArchivo = malloc(sizeof(Archivo));
    Resultado *resultado = malloc(sizeof(Resultado));

    datosArchivo = leerArchivoEntrada(argv[1]);

    if (datosArchivo->aperturaArchivo == -1) {
        printf("Error abriendo el archivo.");
        return 1;
    }
    
    char **tablero = tableroInicial();
    resultado = simularJuego(&tablero, datosArchivo->jugadasLeidas);

    printf("  ABCDEFGH \n");
    printf(" ---------- \n");
    for (int j = 0; j < 8; j++) {
        printf("%d|",j+1);
        for (int k = 0; k < 8; k++) {
            printf("%c", tablero[j][k]);
        }
        printf("|%d\n",j+1);
    }
    printf(" ---------- \n");
    printf("  ABCDEFGH \n\n");

    
    if (resultado->partidaValida) {
        char ganador = determinarGanador(tablero);

        if (ganador != 'E') {
            // Buscando el jugador ganador.
            char *nombreGanador;
            for (int i = 0; i < 2; i++) {
                if (datosArchivo->jugadores->jugadores[i].color == ganador) {
                    nombreGanador = malloc(sizeof(char) * (strlen(datosArchivo->jugadores->jugadores[i].nombre) + 1));
                    nombreGanador = datosArchivo->jugadores->jugadores[i].nombre;
                }
            }
            
            printf("El ganador es: %s", nombreGanador);
        } else {
            printf("Ha habido un empate.");
        }
    } else {
        if (resultado->partidaIncompleta) {
            volcarTableroEnArchivo(argv[2], tablero, resultado->color);
            if (resultado->dobleSalteo == true) {
                printf("Partida incompleta. Se podia saltear turno y luego el siguiente jugador tenia jugadas para hacer.");
            } else {
                printf("Partida incompleta. Se podia continuar jugando.");
            }
        } else {
            printf("ERROR jugada invalida. %s Paso en el turno de %c", resultado->error, resultado->color);
        }
        
        
    }
    
    // Liberacion de los espacios de memoria asignados.
    liberarMemoria(datosArchivo->jugadores, datosArchivo->jugadasLeidas, resultado);

    return 0;
}