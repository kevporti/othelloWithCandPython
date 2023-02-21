import sys
import random

# desestructurarJugada:
# Funcion auxiliar para desestructurar el formato de la jugada en color, indice de la columna e indice de la fila.
def desestructurarJugada(jugada):
    (colorDelJugador, posicionDeLaJugada) = jugada
    indiceDeLaColumna = ord(posicionDeLaJugada[0]) - ord('A') # convertir la letra de la columna en el indice adecuado
    indiceDeLaFila = int(posicionDeLaJugada[1]) - 1

    return (colorDelJugador, indiceDeLaFila, indiceDeLaColumna)

# validarJugadaDentroTablero:
# Validar que la jugada este dentro del tablero.
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

    if tablero[indiceDeLaFila][indiceDeLaColumna] != "X":
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
# Dada una jugada valida y un conjunto de fichas enemigas encerradas, aplica la jugada al tablero y cambia de color las fichas enemigas encerradas.
def aplicarJugada(tablero, jugada, fichasEncerradas):
    (colorDelJugador, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)

    # Ficha que el jugador acaba de colocar
    tablero[indiceDeLaFila][indiceDeLaColumna] = colorDelJugador

    # # Cambiar el color de las fichas enemigas encerradas
    for (i, j) in fichasEncerradas:
        tablero[i][j] = colorDelJugador

# controlSalteoJugada:
# Verificar que el jugador que salteo el turno no tenia otra opcion, es decir, no podia realizar ninguna jugada valida.
def controlSalteoJugada(tablero, colorDelJugador):
    seEncontroJugadaPosible = False
    posiblesJugadas = []

    # Recorrer posiciones del tablero.
    for nroFila in range(8):
        for nroColumna in range(8):
            # Solo verificar jugadas posibles alrededor de casillas que tengan fichas.
            if tablero[nroFila][nroColumna] != 'X':
                # Intentar jugadas en cada posicion alrededor de la ficha:
                for i in range(-1, 2):
                    for j in range(-1, 2):
                        # Sumar/restar a los indices de la casilla para poder alcanzar las posiciones alrededor de la misma.
                        indiceDeLaFilaAVerificar = nroFila + i
                        indiceDeLaColumnaAVerificar = nroColumna + j
                        
                        # Verificar que las posiciones a comparar no se salgan del rango de indices del tablero.
                        seSaleDelTablero = indiceDeLaFilaAVerificar >= 8 or indiceDeLaFilaAVerificar < 0 or indiceDeLaColumnaAVerificar >= 8 or indiceDeLaColumnaAVerificar < 0

                        # Solo verificar si la jugada es posible en posiciones vacias ya que no es posible colocar una ficha en una casilla ocupada.
                        if not seSaleDelTablero and tablero[indiceDeLaFilaAVerificar][indiceDeLaColumnaAVerificar] == 'X':
                            letraColumna = chr(indiceDeLaColumnaAVerificar + ord('A')) # Convertir indice a letra
                            jugada = (colorDelJugador, letraColumna + str(indiceDeLaFilaAVerificar + 1)) # Armar jugada para probar si hubiera sido valida.

                            # Intentar jugada
                            resultado = controlarJugadaValida(tablero, jugada)
                            if resultado['jugadaValida'] and (jugada, resultado['fichasEncerradas']) not in posiblesJugadas:
                                # Si la jugada es valida y no se habia guardado antes, se encontro una jugada que se podria haber realizado.
                                posiblesJugadas.append((jugada, resultado['fichasEncerradas']))

                                seEncontroJugadaPosible = True

    if seEncontroJugadaPosible:
        # Como se encontro una jugada posible, entonces ha habido un error.
        return {
        'jugadaValida': False,
        'color': colorDelJugador,
        'error': f'Se podria haber colocado una ficha en la posicion {jugada[1]}.',
        'posiblesJugadas': posiblesJugadas
        }

    return {
        'jugadaValida': True
    }

# validaciones:
# 
def validaciones(tablero, jugada):
    # Mientras la jugada no sea un salto de turno.
    if jugada[1] != '':
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
        # Controlar que la unica opcion que haya habido sea pasar el turno.
        resultado = controlSalteoJugada(tablero, jugada[0])

        if not resultado['jugadaValida']:
            return (False, resultado)

    return (True, resultado)


# pedirJugadas:
# Pedirle al usuario una jugada y validarla. Seguir pidiendo si la jugada no es valida.
def pedirJugadas(tablero, colorJugador):
    jugadaValida = False

    while not jugadaValida:
        jugadaTemp = input("Por favor, ingrese una jugada: ")
        jugada = (colorJugador, jugadaTemp)

        (jugadaValida, resultado) = validaciones(tablero, jugada)

        if 'error' in resultado:
            print(resultado['error'])

    return resultado

# botDificultadCero:
# Conseguir todas las jugadas posibles para el Bot y elegir una random.
def botDificultadCero(tablero, colorBot):
    # Usando esta funcion logramos captar todas las posibles jugadas que se pueden realizar usando el color del Bot.
    resultado = controlSalteoJugada(tablero, colorBot)

    if not resultado['jugadaValida']:
        posiblesJugadas = resultado['posiblesJugadas']
        jugadaRandom = random.choice(posiblesJugadas)

        # Mostrar y validar nuevamente la jugada.
        validaciones(tablero, jugadaRandom[0])
        print(f'Bot: "Esto se ve dificil, dejame pensar... {jugadaRandom[0][1]}."')

# segundoItemDeTupla:
# Funcion auxiliar para conseguir la lista de fichas que encierra cada jugada.
def segundoItemDeTupla(tupla):
    return len(tupla[1])

# botDificultadUno:
# Conseguir todas las jugadas posibles para el Bot y elegir la que mas fichas enemigas encierre.
def botDificultadUno(tablero, colorBot):
    # Usando esta funcion logramos captar todas las posibles jugadas que se pueden realizar usando el color del Bot.
    resultado = controlSalteoJugada(tablero, colorBot)

    if not resultado['jugadaValida']:
        posiblesJugadas = resultado['posiblesJugadas']
        mejorJugada = max(posiblesJugadas, key=segundoItemDeTupla)
        validaciones(tablero, mejorJugada[0])
        print(f'Bot: "Mi jugada sera... {mejorJugada[0][1]}!"')

# controlarFinJuego:
# Controlar si se pueden hacer mas jugadas. Controlar si se puede saltear el turno y luego
# el otro jugador tiene jugadas posibles.
def controlarFinJuego(tablero, colorSiguiente):
    # Controlar jugadas con salto de turno de por medio. Si no se puede hacer salto de turno y luego continuar jugando, el juego ha terminado.
    resultado = controlSalteoJugada(tablero, colorSiguiente)

    # Si saltar el turno es posible, comprobar que el siguiente jugador no tenga jugadas posibles.
    if resultado['jugadaValida']:
        futuroSiguienteColor = 'B' if colorSiguiente == 'N' else 'N'
        resultadoDobleSalto = controlSalteoJugada(tablero, futuroSiguienteColor)

        # Si el siguiente jugador no tiene jugadas posibles, entonces es un fin de juego.
        if resultadoDobleSalto['jugadaValida']:
            print('El juego se ha terminado.')
            return True
    # Sino, se puede  seguir jugando.
    return False

# administrarJuego:
# Controla de quien es el turno y muestra del tablero
def administrarJuego(tablero, colorSiguiente, colorJugador, dificultad):
    finJuego = False
    resultado = {'jugadaValida': True}

    mostrarTablero(tablero)
    # Continuar el juego hasta que se acaben las posibilidades de jugadas.
    while not finJuego and resultado['jugadaValida']:

        # Si el color que sigue es el del jugador, pedir jugadas. Sino, llamar al bot
        # para hacer una jugada.
        if colorJugador == colorSiguiente:
            # Parte del jugador
            resultado = pedirJugadas(tablero, colorJugador)
        else:
            # Parte del bot
            if dificultad == '0':
                botDificultadCero(tablero, colorSiguiente)
            elif dificultad == '1':
                botDificultadUno(tablero, colorSiguiente)

        mostrarTablero(tablero)

        # Marcar el color siguiente.
        colorSiguiente = "N" if colorSiguiente == "B" else "B"

        # Controlar que se puedan continuar haciendo jugadas. Control de proximo turno o si se puede hacer
        # salto de turno y luego el siguiente jugador puede continuar.
        finJuego = controlarFinJuego(tablero, colorSiguiente)


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
            fila = list(linea.strip())
            tablero.append(fila)

    archivo.close()

    return (tablero, colorSiguiente)

# mostrarTablero: List[List[String]] -> None
# Dado como argumento un tablero, muestra el estado del tablero por consola.
def mostrarTablero(tablero):
    for linea in tablero:
        for celda in linea:
            print(celda, end=' ')
        print()
    print()

# definirGanador:
# 
def definirGanador(tablero):
    fichasBlancas = 0
    fichasNegras = 0

    # Contar fichas de cada color
    for linea in tablero:
        for casilla in linea:
            if casilla == 'B':
                fichasBlancas += 1
            elif casilla == 'N':
                fichasNegras += 1

    # Devolver resultado
    if fichasBlancas < fichasNegras:
        return 'N'
    elif fichasBlancas > fichasNegras:
        return 'B'
    else:
        return 'Empate'

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

    resultado = definirGanador(tablero)

    if resultado != 'Empate':
        print(f'El ganador es {"usted" if colorJugador == resultado else "la maquina"}.')
    else:
        print('Ha habido un empate.')

if __name__ == '__main__':
    main()