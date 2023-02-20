import sys


# desestructurarJugada:
# Funcion auxiliar para desestructurar el formato de la jugada en color, indice de la columna e indice de la fila.
def desestructurarJugada(jugada):
    (colorDelJugador, posicionDeLaJugada) = jugada
    indiceDeLaColumna = ord(posicionDeLaJugada[0]) - ord('A') # convertir la letra de la columna en el indice adecuado
    indiceDeLaFila = int(posicionDeLaJugada[1]) - 1

    return (colorDelJugador, indiceDeLaFila, indiceDeLaColumna)

# validarJugadaDentroTablero:
#
def validarJugadaDentroTablero(jugada):
    (color, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)
  
    if indiceDeLaFila >= 8 or indiceDeLaFila < 0 or indiceDeLaColumna >= 8 or indiceDeLaColumna < 0:
        return {
        'jugadaValida': False,
        'color': color, 
        'error': f'La posicion {jugada[1]} se sale del tablero.'
        }
    
    return {
        'jugadaValida': True,
    }

# controlarJugadaRepetida:
# Verifica si la posicion de la jugada ya esta ocupada.
def controlarJugadaRepetida(tablero, jugada):
    (color, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)

    if tablero[indiceDeLaFila][indiceDeLaColumna] != " ":
        return {
        'jugadaValida': False, 
        'color': color, 
        'error': f'La posicion {jugada[1]} ya esta ocupada.'
        }

    return {
        'jugadaValida': True,
    }

# controlarJugadaValida:
# Determina si la casilla elegida en la jugada coincide con tener al menos una ficha del oponente alrededor de la nueva ficha
# y si, a su vez, hay una ficha aliada en direccion vertical, horizontal o diagonal que, junto con la ficha a colocar, encierre fichas enemigas.
def controlarJugadaValida(tablero, jugada):
    (colorDelJugador, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)
    colorDelOponente = 'N' if colorDelJugador == 'B' else 'B'
    hayOponenteAlrededor = False
    hayFichasEncerradas = False

    fichasEncerradas = []
    # Recorrer todas las posiciones del tablero que estan alrededor de la jugada realizada.
    for i in range(-1, 2):
        for j in range(-1, 2):
        # Quitando el caso de verificar la posicion de la jugada, vemos las posiciones de alrededor.
            if i != 0 or j != 0:
                indiceDeLaFilaAVerificar = indiceDeLaFila + i
                indiceDeLaColumnaAVerificar = indiceDeLaColumna + j

                # Verificando que las posiciones a comparar no se salgan de los parametros del tablero.
                seSaleDelTablero = indiceDeLaFilaAVerificar >= 8 or indiceDeLaFilaAVerificar < 0 or indiceDeLaColumnaAVerificar >= 8 or indiceDeLaColumnaAVerificar < 0

                fichasEnemigas = []
                # Recorriendo filas, columnas y diagonales mientras haya una ficha enemiga, para determinar si al final
                # de todas las fichas hay una aliada que encierre las enemigas y poder validar la jugada.
                while not seSaleDelTablero and tablero[indiceDeLaFilaAVerificar][indiceDeLaColumnaAVerificar] == colorDelOponente:
                    fichasEnemigas.append((indiceDeLaFilaAVerificar, indiceDeLaColumnaAVerificar))

                    # Avanzar en cada direccion
                    indiceDeLaFilaAVerificar += i
                    indiceDeLaColumnaAVerificar += j

                    seSaleDelTablero = indiceDeLaFilaAVerificar >= 8 or indiceDeLaFilaAVerificar < 0 or indiceDeLaColumnaAVerificar >= 8 or indiceDeLaColumnaAVerificar < 0
                    
                # Hay al menos una ficha enemiga adyacente en esa direccion?
                if fichasEnemigas != []:
                    hayOponenteAlrededor = True
                    
                    # Hay una ficha aliada que encierra a las enemigas en esa direccion?
                    if not seSaleDelTablero and tablero[indiceDeLaFilaAVerificar][indiceDeLaColumnaAVerificar] == colorDelJugador:
                        hayFichasEncerradas = True
                        fichasEncerradas += fichasEnemigas

    if not hayOponenteAlrededor:
        return {
        'jugadaValida': False,
        'color': colorDelJugador,
        'error': f'La posicion {jugada[1]} no tiene ningua ficha del oponente alrededor.'
        }

    if not hayFichasEncerradas:
        return {
        'jugadaValida': False,
        'color': colorDelJugador,
        'error': f'La posicion {jugada[1]} no encierra fichas del oponente.'
        }
    
    return {
        'jugadaValida': True,
        'fichasEncerradas': fichasEncerradas
    }

# aplicarJugada:
#
def aplicarJugada(tablero, jugada, fichasEncerradas):
    (colorDelJugador, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)

    # Ficha que el jugador acaba de colocar
    tablero[indiceDeLaFila][indiceDeLaColumna] = colorDelJugador

    # Cambiar el color de las fichas enemigas encerradas
    for (i, j) in fichasEncerradas:
        tablero[i][j] = colorDelJugador

# validaciones:
# 
def validaciones(tablero, jugada):
    # Mientras la jugada no sea un salto de turno.
    if jugada != "":
        # Controlar que la jugada no se sale del tablero.
        resultado = validarJugadaDentroTablero(jugada)
        
        if not resultado['jugadaValida']:
            return (False, resultado)

        # Controlar que la jugada no esta repetida.
        resultado = controlarJugadaRepetida(tablero, jugada)

        if not resultado['jugadaValida']:
            return (False, resultado)

        # Controlar que la jugada cumpla con las reglas.
        resultado = controlarJugadaValida(tablero, jugada)

        if not resultado['jugadaValida']:
            return (False, resultado)

        # Aplicar la jugada al tablero.
        aplicarJugada(tablero, jugada, resultado['fichasEncerradas'])
    else:
        print("salto de turno")
    
    return (True, resultado)


# pedirJugadas:
# 
def pedirJugadas(tablero, colorJugador):
    jugadaValida = False

    while not jugadaValida:
        jugadaTemp = input("Por favor, ingrese una jugada: ")
        jugada = (colorJugador, jugadaTemp)

        (jugadaValida, resultado) = validaciones(tablero, jugada)
        
        if resultado['error']:
            print(resultado['error'])

    return resultado



# controlarFinJuego:
# Controlar si se pueden hacer mas jugadas. Controlar si se puede saltear el turno y luego
# el otro jugador tiene jugadas posibles.
def controlarFinJuego(colorSiguiente):
    pass


# administrarJuego:
# Controla de quien es el turno y muestra del tablero
def administrarJuego(tablero, colorSiguiente, colorJugador, dificultad):
    finJuego = False
    resultado = {'jugadaValida': True}

    # Continuar el juego hasta que se acaben las posibilidades de jugadas.
    while not finJuego and resultado['jugadaValida']:
        mostrarTablero(tablero)

        # Si el color que sigue es el del jugador, pedir jugadas. Sino, llamar al bot
        # para hacer una jugada.
        if colorJugador == colorSiguiente:
            # Parte del jugador
            resultado = pedirJugadas(tablero, colorJugador)
        else:
            # Parte del bot
            if dificultad == 0:
                pass
            elif dificultad == 1:
                pass
        
        colorSiguiente = "N" if colorSiguiente == "B" else "B"

        # Controlar que se puedan continuar haciendo jugadas. Control de proximo turno o si se puede hacer
        # salto de turno y luego el siguiente jugador puede continuar.
        controlarFinJuego(colorSiguiente)
    if not resultado['jugadaValida']:
        print("Hubo un error con una jugada")



# leerTablero: String -> Tuple(List[List[String]], String)
# Lee el tablero del archivo y el siguiente turno en la partida. Luego devuelve el tablero
# y el color.
def leerArchivo(nombreArchivo):
    tablero = []
    
    archivo = open(nombreArchivo, 'r')

    for linea in archivo:
        if len(linea) == 1:
            colorSiguiente = linea
        else:
            tablero.append(linea[:-1])

    archivo.close()

    return (tablero, colorSiguiente)

# mostrarTablero: List[List[String]] -> None
# Dado como argumento un tablero, muestra el estado del tablero por consola.
def mostrarTablero(tablero):
    for linea in tablero:
        print(linea)

# main:
# 
def main():
    if len(sys.argv) != 4:
        print("Por favor, al correr el programa asegurese de pasar todos los argumentos.")
        exit()

    nombreArchivo = sys.argv[1]
    colorJugador = sys.argv[2]
    dificultad = sys.argv[3]
    
    (tablero, colorSiguiente) = leerArchivo(nombreArchivo)

    administrarJuego(tablero, colorSiguiente, colorJugador, dificultad)

if __name__ == '__main__':
    main()


# Iniciar programa
# Mostrar tablero
# Preguntar jugada
# validar
# agregar y mostrar tablero
# Jugada del bot
# 
