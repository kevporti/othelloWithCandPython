from othelloBot import *

def test_desestructurarJugada():
  jugadaUno = ('N', 'D3')
  jugadaDos = ('B', 'H7')
  jugadaTres = ('B', 'A1')

  assert desestructurarJugada(jugadaUno) == ('N', 2, 3)
  assert desestructurarJugada(jugadaDos) == ('B', 6, 7)
  assert desestructurarJugada(jugadaTres) == ('B', 0, 0)
  assert desestructurarJugada(jugadaTres) != ('B', 1, 1)

def test_validarJugadaDentroTablero():
  jugadaUno = ('N', 'D3')
  jugadaDos = ('B', 'H9')
  jugadaTres = ('B', 'Z1')

  assert validarJugadaDentroTablero(jugadaUno) == {
    'jugadaValida': True
  }
  assert validarJugadaDentroTablero(jugadaDos) == {
    'jugadaValida': False, 
    'color': jugadaDos[0], 
    'error': f'La posicion {jugadaDos[1]} se sale del tablero.'
  }
  assert validarJugadaDentroTablero(jugadaTres) != {
    'jugadaValida': True
  }
  assert validarJugadaDentroTablero(jugadaTres) == {
    'jugadaValida': False, 
    'color': jugadaTres[0], 
    'error': f'La posicion {jugadaTres[1]} se sale del tablero.'
  }

def test_controlarJugadaRepetida():
  tablero = []
  archivo = open('salida.txt', 'r')

  for linea in archivo:
    if len(linea) != 1:
      fila = list(linea.strip())
      tablero.append(fila)
  
  archivo.close()

  jugadaUno = ('B', 'G1')
  jugadaDos = ('N', 'B7')
  jugadaTres = ('N', 'B8')

  assert controlarJugadaRepetida(tablero, jugadaUno) == {
    'jugadaValida': True
  }

  assert controlarJugadaRepetida(tablero, jugadaDos) == {
    'jugadaValida': True
  }
  
  controlarJugadaRepetida(tablero, jugadaUno) == {
    'jugadaValida': False,
    'color': jugadaUno[0],
    'error': f'La posicion {jugadaUno[1]} ya esta ocupada.'
  }

  controlarJugadaRepetida(tablero, jugadaTres) == {
    'jugadaValida': False,
    'color': jugadaTres[0],
    'error': f'La posicion {jugadaTres[1]} ya esta ocupada.'
  }

def test_controlarJugadaValida():
  tablero = []
  archivo = open('salida.txt', 'r')

  for linea in archivo:
    if len(linea) != 1:
      fila = list(linea.strip())
      tablero.append(fila)
  
  archivo.close()

  # Ficha enemiga alrededor pero no encierra
  jugadaTres = ('N', 'B7')
  assert controlarJugadaValida(tablero, jugadaTres) == {
    'jugadaValida': False, 
    'color': jugadaTres[0], 
    'error': f'La posicion {jugadaTres[1]} no encierra fichas del oponente.'
  }

  # Ficha enemiga alrededor y encierra. Devuelve coordenadas de la ficha encerrada.
  jugada = ('B', 'G1')
  assert controlarJugadaValida(tablero, jugada) == {
    'jugadaValida': True, 
    'fichasEncerradas': [(0, 5), (0, 4), (0, 3), (0, 2), (0, 1), (1, 6)]
  }

def test_definirGanador():
  tablero = []
  archivo = open('salida.txt', 'r')

  for linea in archivo:
    if len(linea) != 1:
      fila = list(linea.strip())
      tablero.append(fila)
  
  archivo.close()
  
  tablero[3][2:4] = ['N', 'N']
  assert definirGanador(tablero) == 'N'

def test_controlSalteoJugada():
  tablero = []
  for i in range(8):
    fila = []
    for j in range(8):
      if (i == j == 3) or (i == j == 4):
        fila.append("B")
      elif (i == 3 and j == 4) or (i == 4 and j == 3):
        fila.append("N")
      else:
        fila.append("X")
    tablero.append(fila)

  tableroAntesDelSaltoDeTurno = [('B', 'A3'), ('B', 'B3'), ('B', 'C3'), ('B', 'D4'), ('B', 'E3'), ('B', 'F3'), ('B', 'G3'), ('B', 'E1'), ('B', 'E2'), ('B', 'C4'), ('B', 'C5'), ('B', 'D3'), ('B', 'D4'), ('B', 'D5'), ('B', 'E4'), ('B', 'E5'), ('B', 'F4'), ('B', 'F2'), ('N', 'H3'), ('N', 'H4'), ('N', 'H5')]

  for jugada in tableroAntesDelSaltoDeTurno:
    (colorDelJugador, indiceDeLaFila, indiceDeLaColumna) = desestructurarJugada(jugada)
    tablero[indiceDeLaFila][indiceDeLaColumna] = colorDelJugador

  assert controlSalteoJugada(tablero, 'N') == {
    'jugadaValida': True
  }
  assert controlSalteoJugada(tablero, 'B') == {
    'jugadaValida': True
  }

  tablero[2][0] = 'X'

  assert controlSalteoJugada(tablero, 'N') == {
    'jugadaValida': False,
    'color': 'N',
    'error': f'Se podria haber colocado una ficha en la posicion H6.',
    'posiblesJugadas': [(('N', 'A3'), [(2, 1), (2, 2), (2, 3), (2, 4), (2, 5), (2, 6)])]
  }
  assert controlSalteoJugada(tablero, 'B') == {
    'jugadaValida': True
  }

def test_validaciones():
  (tablero, _) = leerArchivo("salida.txt")

  assert validaciones(tablero, ('B', 'G1')) == (True, {'jugadaValida': True, 'fichasEncerradas': [(0, 5), (0, 4), (0, 3), (0, 2), (0, 1), (1, 6)]})
  assert validaciones(tablero, ('N', 'B7')) == (False, {'jugadaValida': False, 'color': 'N', 'error': 'La posicion B7 no encierra fichas del oponente.'})

def test_controlarFinJuego():
  (tablero, _) = leerArchivo("salida.txt")
  
  assert controlarFinJuego(tablero, 'N') == False
  assert controlarFinJuego(tablero, 'B') == False

def test_leerArchivo():
  assert leerArchivo("salida.txt") == ([['B', 'N', 'N', 'N', 'N', 'N', 'X', 'N'], ['B', 'B', 'N', 'B', 'B', 'B', 'N', 'N'], ['B', 'N', 'B', 'N', 'B', 'N', 'B', 'N'],
  ['B', 'B', 'N', 'B', 'N', 'B', 'N', 'N'], ['B', 'N', 'N', 'N', 'B', 'N', 'B', 'N'], ['B', 'N', 'N', 'B', 'N', 'B', 'N', 'N'], ['B', 'X', 'N', 'N', 'N', 'N', 'N', 'N'],
  ['B', 'X', 'N', 'N', 'N', 'N', 'N', 'N']], 'B')