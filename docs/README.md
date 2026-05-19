# LL1-Parser — Analizador Léxico y Sintáctico LL(1)

Proyecto de compiladores que implementa las etapas de **análisis léxico**, **análisis sintáctico LL(1)** y **análisis semántico básico** para un lenguaje imperativo tipo Pascal simplificado. Genera una tabla LL(1), valida código fuente mediante un autómata de pila no recursivo, construye un AST y alimenta una tabla de símbolos persistente.

---

## Tabla de contenido

1. [Requisitos](#1-requisitos)
2. [Estructura del proyecto](#2-estructura-del-proyecto)
3. [Compilación](#3-compilación)
   - [Con CMake (recomendado)](#31-con-cmake-recomendado)
   - [Con g++ directo](#32-con-g-directo)
4. [Ejecución](#4-ejecución)
   - [Flags disponibles](#41-flags-disponibles)
   - [Ejemplos de uso](#42-ejemplos-de-uso)
5. [El lenguaje soportado](#5-el-lenguaje-soportado)
   - [Gramática](#51-gramática)
   - [Tokens reconocidos](#52-tokens-reconocidos)
   - [Programas de ejemplo](#53-programas-de-ejemplo)
6. [Salida del programa](#6-salida-del-programa)
   - [Tabla de análisis léxico](#61-tabla-de-análisis-léxico)
   - [Conjuntos FIRST y FOLLOW](#62-conjuntos-first-y-follow)
   - [Tabla LL(1)](#63-tabla-ll1)
   - [Árbol Sintáctico (AST)](#64-árbol-sintáctico-ast)
   - [Tabla de símbolos](#65-tabla-de-símbolos)
   - [Traza de análisis](#66-traza-de-análisis)
7. [Manejo de errores](#7-manejo-de-errores)
8. [Arquitectura interna](#8-arquitectura-interna)
9. [Personalizar la gramática](#9-personalizar-la-gramática)

---

## 1. Requisitos

| Herramienta | Versión mínima | Notas |
|-------------|---------------|-------|
| **g++**     | 9.0           | Soporte C++17 obligatorio |
| **CMake**   | 3.15          | Solo para el método CMake |
| **clang++** | 10.0          | Alternativa a g++ |

No se requieren bibliotecas externas. El proyecto es 100 % estándar C++17.

---

## 2. Estructura del proyecto

```text
src/
├── main.cpp                         ← Punto de entrada, CLI, orquestador
│
├── grammar/                         ── Módulo de gramática
│   ├── grammar.txt                  ← Gramática LL(1) editable
│   ├── core/
│   │   └── grammar.h / grammar.cpp  ← Estructuras Grammar, Symbol, Production
│   ├── analysis/
│   │   ├── first_set.h/cpp          ← Cálculo de conjuntos FIRST
│   │   ├── follow_set.h/cpp         ← Cálculo de conjuntos FOLLOW
│   │   ├── ll1_table.h/cpp          ← Construcción de la tabla LL(1)
│   │   └── validator.h/cpp          ← Validación: recursión izq., NT indefinidos
│   └── io/
│       └── grammar_parser.h/cpp     ← Lector del archivo grammar.txt
│
├── lexer/                           ── Módulo léxico
│   ├── lexer/
│   │   └── lexer.h / lexer.cpp      ← Autómata de estados finitos
│   ├── token/
│   │   ├── token.h                  ← Struct Token {tipo, lexema, línea, columna}
│   │   └── token_type.h             ← Enum TokenType + tokenToParserSymbol()
│   ├── symbol_table/
│   │   └── symbol_table.h/cpp       ← Tabla de símbolos del lexer
│   ├── error_handler/
│   │   └── error_handler.h/cpp      ← Recolector de errores léxicos
│   └── utils/
│       └── utils.h / utils.cpp      ← Clasificación de caracteres, keywords
│
├── parser/                          ── Módulo sintáctico
│   ├── parser.h / parser.cpp        ← Motor de pila LL(1) no recursivo
│   ├── symbol_manager.h/cpp         ← Tabla de símbolos del parser (con scopes)
│   ├── ast/
│   │   ├── ast_node.h/cpp           ← Nodos del AST, impresión en árbol
│   │   ├── ast_builder.cpp          ← Constructor del AST
│   │   └── ast_visitor.cpp          ← Patrón visitor sobre el AST
│   ├── semantic/
│   │   └── semantic_analyzer.h/cpp  ← Dos pasadas: declaraciones + usos
│   └── trace/
│       ├── trace_manager.h/cpp      ← Logger de pasos de la pila
│       └── trace_formatter.cpp      ← Formatos MINIMAL/NORMAL/VERBOSE/DEBUG
│
└── test/                            ── Programas de prueba
    ├── programa.pas                 ← Programa válido
    ├── programa_error.pas           ← Variable usada sin declarar
    └── programa_duplicado.pas       ← Variable declarada dos veces
```

---

## 3. Compilación

### 3.1 Con CMake (recomendado)

```bash
# Desde la raíz del proyecto (donde está src/)
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# El binario queda en:
./build/bin/ll1-parser
```

En Windows con MinGW:

```bash
cmake -S src -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

En Windows con Visual Studio:

```bash
cmake -S src -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### 3.2 Con g++ directo

Desde dentro de `src/`:

```bash
g++ -std=c++17 -O2 -I. \
  grammar/analysis/first_set.cpp \
  grammar/analysis/follow_set.cpp \
  grammar/analysis/ll1_table.cpp \
  grammar/analysis/validator.cpp \
  grammar/core/grammar.cpp \
  grammar/io/grammar_parser.cpp \
  lexer/error_handler/error_handler.cpp \
  lexer/lexer/lexer.cpp \
  lexer/symbol_table/symbol_table.cpp \
  lexer/token/token.cpp \
  lexer/utils/utils.cpp \
  parser/ast/ast_builder.cpp \
  parser/ast/ast_node.cpp \
  parser/ast/ast_visitor.cpp \
  parser/parser.cpp \
  parser/semantic/semantic_analyzer.cpp \
  parser/symbol_manager.cpp \
  parser/trace/trace_formatter.cpp \
  parser/trace/trace_manager.cpp \
  main.cpp \
  -o ll1_parser
```

---

## 4. Ejecución

```bash
./ll1-parser --input <archivo.pas> [opciones]
```

### 4.1 Flags disponibles

| Flag | Argumento | Descripción |
|------|-----------|-------------|
| `--input` | `<archivo>` | **Obligatorio.** Ruta al programa fuente a analizar |
| `--grammar` | `<archivo>` | Ruta a la gramática. Por defecto busca `grammar/grammar.txt` automáticamente |
| `--sets` | — | Muestra los conjuntos FIRST y FOLLOW calculados |
| `--table` | — | Muestra la tabla LL(1) completa (en bloques de 6 columnas) |
| `--trace` | — | Activa la traza de pila y la guarda en `traza_analisis.txt` |
| `--trace-level` | `0`–`4` | Nivel de detalle de la traza (ver tabla abajo) |
| `--help`, `-h` | — | Muestra la ayuda y termina |

**Niveles de traza:**

| Nivel | Nombre | Qué muestra |
|-------|--------|-------------|
| `0` | NONE | Sin traza |
| `1` | MINIMAL | Solo errores y resultado final |
| `2` | NORMAL | Cada paso, MATCH y EXPAND *(predeterminado con `--trace`)* |
| `3` | VERBOSE | NORMAL + estado de pila en cada paso |
| `4` | DEBUG | VERBOSE con separadores visuales entre cada paso |

### 4.2 Ejemplos de uso

**Análisis básico:**

```bash
./build/bin/ll1-parser --input test/programa.pas
```

**Mostrar FIRST/FOLLOW y tabla LL(1):**

```bash
./build/bin/ll1-parser --input test/programa.pas --sets --table
```

**Generar traza completa:**

```bash
./build/bin/ll1-parser --input test/programa.pas --trace
# Genera traza_analisis.txt en el directorio actual
```

**Traza en modo DEBUG con gramática personalizada:**

```bash
./build/bin/ll1-parser --input test/programa.pas \
             --grammar mi_gramatica.txt \
             --trace --trace-level 4
```

**Ver ayuda:**

```bash
./build/bin/ll1-parser --help
```

---

## 5. El lenguaje soportado

### 5.1 Gramática

La gramática está en `grammar/grammar.txt` y puede modificarse sin recompilar. El formato usa `->` y `|` para alternativas; `EPS` representa la producción vacía (ε).

```text
P  -> PROGRAM ID SEMICOLON D B

D  -> VAR L | EPS

L  -> ID COLON T SEMICOLON L2
L2 -> L | EPS

T  -> INT | FLOAT

B  -> BEGIN S END

S  -> ID ASSIGN E SEMICOLON S2
S2 -> S | EPS

E  -> F E2
E2 -> PLUS F E2 | MINUS F E2 | EPS

F  -> ID | NUM | FNUM
```

**No-terminales y su rol:**

| NT | Rol |
|----|-----|
| `P` | Programa completo |
| `D` | Sección de declaraciones (`VAR`) — opcional |
| `L` / `L2` | Una variable declarada / continuación de lista |
| `T` | Tipo de dato (`INT` o `FLOAT`) |
| `B` | Bloque `BEGIN ... END` |
| `S` / `S2` | Una sentencia de asignación / continuación |
| `E` / `E2` | Expresión aritmética y su continuación |
| `F` | Factor: identificador, entero o real |

### 5.2 Tokens reconocidos

| Categoría | Tokens | Ejemplos en código |
|-----------|--------|-------------------|
| Palabras reservadas | `PROGRAM` `VAR` `BEGIN` `END` `INT` `FLOAT` | `PROGRAM`, `begin`, `INT` |
| Identificadores | `ID` | `x`, `miVariable`, `suma_total` |
| Literales enteros | `NUM` | `0`, `10`, `255` |
| Literales reales | `FNUM` | `3.14`, `2.0`, `0.5` |
| Operadores | `PLUS` `MINUS` `MULT` `ASSIGN` `OP_GREATER` `OP_LESS` | `+` `-` `*` `=` `>` `<` |
| Delimitadores | `SEMICOLON` `COLON` `LPAREN` `RPAREN` | `;` `:` `(` `)` |

> El lexer es **insensible a mayúsculas/minúsculas** en las palabras reservadas: `PROGRAM`, `program` y `Program` son equivalentes.

### 5.3 Programas de ejemplo

**Programa válido** (`test/programa.pas`):

```pascal
PROGRAM miPrograma;
VAR
  x: INT;
  y: FLOAT;
BEGIN
  x = 10;
  y = x + 5;
END
```

**Programa con error sintáctico** — falta `;` después de `INT` y `+` sin operando:

```pascal
PROGRAM errorPrograma;
VAR
  x: INT
BEGIN
  x = 5 + ;
END
```

Salida esperada:

```text
✗ Errores sintácticos (2):
  - Error sintáctico (línea 4, col 1): se esperaba SEMICOLON, se recibió BEGIN('BEGIN')
  - Error sintáctico (línea 5, col 9): token inesperado SEMICOLON(';') al expandir no-terminal F
```

**Programa con error semántico** — variable usada sin declarar:

```pascal
PROGRAM miPrograma;
VAR
  x: INT;
BEGIN
  y = 10;
END
```

Salida esperada:

```text
✗ Errores semánticos:
  - Error semántico (línea 5): variable 'y' usada sin declarar.
```

**Programa con declaración duplicada**:

```pascal
PROGRAM miPrograma;
VAR
  x: INT;
  x: FLOAT;
BEGIN
  x = 10;
END
```

---

## 6. Salida del programa

El programa imprime **10 secciones numeradas** en stdout. Cada sección puede activarse o suprimirse con flags.

### 6.1 Tabla de análisis léxico

Siempre visible. Muestra cada token con su posición, símbolo del parser, categoría y lexema. Los errores léxicos se marcan con `!`:

```text
  ----------------------------------------------------------
  #    Línea  Col   Símbolo       Categoría       Lexema
  ----------------------------------------------------------
  0    1      1     PROGRAM       Palabra clave   'PROGRAM'
  1    1      9     ID            Identificador   'miPrograma'
  2    1      19    SEMICOLON     Delimitador     ';'
  ...
! 7    3      5     UNKNOWN       ERROR LEXICO    '@'
  ----------------------------------------------------------
```

### 6.2 Conjuntos FIRST y FOLLOW

Se activa con `--sets`. Muestra una tabla lado a lado:

```text
    NT   FIRST                        FOLLOW
    -------------------------------------------------------
    P    { PROGRAM }                  { $ }
    D    { EPS, VAR }                 { BEGIN }
    E    { FNUM, ID, NUM }            { SEMICOLON }
    E2   { EPS, MINUS, PLUS }        { SEMICOLON }
    ...
```

### 6.3 Tabla LL(1)

Se activa con `--table`. Se imprime en **bloques de 6 terminales** para evitar desbordamiento horizontal:

```text
    -----------------------------------------------------------------------
    NT   BEGIN        END     FLOAT   FNUM    ID                     INT
    -----------------------------------------------------------------------
    P    -            -       -       -       -                      -
    D    EPS          -       -       -       -                      -
    L    -            -       -       -       ID COLON T SEMICOLON L2 -
    ...
    -----------------------------------------------------------------------

    -----------------------------------------------------------------------
    NT   MINUS       NUM     PLUS       PROGRAM                  SEMICOLON
    -----------------------------------------------------------------------
    E2   MINUS F E2  -       PLUS F E2  -                        EPS
    F    -           NUM     -          -                        -
    ...
```

### 6.4 Árbol Sintáctico (AST)

Siempre visible si el análisis tiene éxito. Se imprime como árbol con colores ANSI:

```text
└── PROGRAMA
    ├── id:miPrograma (1:9)
    ├── DECLARACIONES
    │   └── DECLARACION
    │       ├── id:x (3:1)
    │       ├── TIPO
    │       │   └── int
    │       └── DECLARACION
    │           ├── id:y (4:1)
    │           └── TIPO
    │               └── float
    └── BLOQUE
        └── SENTENCIA
            ├── id:x (6:1)
            ├── =
            ├── EXPRESION
            │   └── FACTOR
            │       └── num:10 (6:5)
            └── SENTENCIA
                ├── id:y (7:1)
                ├── =
                └── EXPRESION
                    ├── FACTOR
                    │   └── id:x (7:5)
                    └── EXPR_CONT
                        ├── +
                        └── FACTOR
                            └── num:5 (7:9)
```

**Tipos de nodo en el AST:**

| Nodo | Descripción |
|------|-------------|
| `PROGRAMA` | Raíz del árbol |
| `DECLARACIONES` | Bloque `VAR` completo |
| `DECLARACION` | Una variable declarada (nombre + tipo) |
| `TIPO` | Tipo de dato (`int` o `float`) |
| `BLOQUE` | Bloque `BEGIN ... END` |
| `SENTENCIA` | Asignación `ID = E` |
| `EXPRESION` | Expresión aritmética `F E2` |
| `EXPR_CONT` | Continuación con `+` o `-` |
| `FACTOR` | Hoja: identificador o número |

### 6.5 Tabla de símbolos

Siempre visible. Estructura conforme al PDF de especificación:

```text
    Variable        Tipo    Línea  Valor inicial  Usada
    ---------------------------------------------------
    x               int     3      -              sí
    y               float   4      -              sí
    ---------------------------------------------------
    Total: 2 símbolo(s)
```

### 6.6 Traza de análisis

Se activa con `--trace`. Se escribe en `traza_analisis.txt` en el directorio de ejecución. Muestra el estado de la pila en cada paso (sin marcadores internos):

```text
   0 | [INIT] Inicializacion del parser
         Pila: $ P
         Tokens de entrada: 25
   1 | [STEP] $ P vs PROGRAM
   0 | [EXPAND] Expandir no-terminal: P -> PROGRAM ID SEMICOLON D B
   2 | [STEP] $ B D SEMICOLON ID PROGRAM vs PROGRAM
   2 | [MATCH] Match - Terminal coincide: PROGRAM - Token: 'PROGRAM'
   3 | [STEP] $ B D SEMICOLON ID vs ID
   3 | [MATCH] Match - Terminal coincide: ID - Token: 'miPrograma'
   ...
  42 | [OK] Analisis exitoso
```

Formato de cada línea:

- `[STEP]` — Estado actual: `pila vs token_actual`
- `[EXPAND]` — Expansión de no-terminal con la producción aplicada
- `[MATCH]` — Terminal consumido exitosamente
- `[ERROR]` — Error sintáctico con línea y token inesperado
- `[OK]` — Análisis completado sin errores

---

## 7. Manejo de errores

El analizador detecta y reporta tres tipos de errores:

### Errores léxicos

Caracteres no reconocidos por el autómata del lexer. Se marcan con `!` en la tabla léxica y el análisis continúa (recuperación automática).

```text
! 5    2      8     UNKNOWN       ERROR LEXICO    '@'
```

### Errores sintácticos

El parser informa la línea, columna, token recibido y su lexema:

```text
Error sintáctico (línea 4, col 1): se esperaba SEMICOLON, se recibió BEGIN('BEGIN')
Error sintáctico (línea 5, col 9): token inesperado SEMICOLON(';') al expandir no-terminal F
```

### Errores semánticos

La segunda pasada del analizador semántico detecta:

- Variable **declarada dos veces** en el mismo bloque
- Variable **usada sin declarar** en el bloque `BEGIN/END`
- Variable **declarada pero nunca usada** (advertencia, no error)

```text
Error semántico (línea 5): variable 'y' usada sin declarar.
Advertencia: variable 'x' declarada en línea 3 nunca se usa.
```

---

## 8. Arquitectura interna

El flujo de procesamiento sigue el pipeline clásico de un compilador:

```text
Código fuente (.pas)
        │
        ▼
┌───────────────┐
│     Lexer     │  → Lista de tokens [ {tipo, lexema, línea, col} ]
│  (AFD)        │  → Tabla de símbolos léxica
└───────┬───────┘
        │
        ▼
┌───────────────────────────────────┐
│     Módulo de gramática           │
│  GrammarParser → Grammar          │
│  FirstSetCalculator               │
│  FollowSetCalculator              │
│  LL1TableGenerator                │
└───────────────┬───────────────────┘
                │  Tabla LL(1) [NT][Terminal] → Producción
                ▼
┌───────────────────────────────────┐
│     Parser LL(1) — pila           │
│  Motor no recursivo:              │
│  ┌──────────────────────────┐     │
│  │ Pila: [ $ | P | ... ]    │     │
│  │ Token actual: PROGRAM    │     │
│  └──────────────────────────┘     │
│  Acciones: MATCH / EXPAND / ERROR │
│  Construye el AST en paralelo     │
└───────────────┬───────────────────┘
                │  AST + lista de tokens
                ▼
┌───────────────────────────────────┐
│     Analizador semántico          │
│  1ª pasada: recolectar decl.      │
│  2ª pasada: validar usos          │
│  → Tabla de símbolos semántica    │
└───────────────────────────────────┘
```

### Algoritmo del motor de pila (parser.cpp)

```text
Inicializar:  push($)  push(símbolo_inicial)
token_actual = primer token de la lista

Mientras pila no vacía:
  top = pila.top()

  Si top == "$":
    Si token_actual == "$" → ÉXITO
    Si no                  → ERROR: entrada no terminada

  Si top es Terminal:
    Si top == token_actual → POP + avanzar token (MATCH)
    Si no                  → ERROR: se esperaba X, se recibió Y

  Si top es No-Terminal:
    Consultar Tabla[top][token_actual]
    Si hay producción → POP + apilar producción en reversa (EXPAND)
    Si celda vacía    → ERROR: token inesperado al expandir NT
```

---

## 9. Personalizar la gramática

El archivo `grammar/grammar.txt` puede editarse libremente **sin recompilar**. Reglas del formato:

```text
# Esto es un comentario (línea que empieza con #)

# Formato de una regla:
NT -> simbolo1 simbolo2 ... | alternativa1 | EPS

# EPS representa la producción vacía (ε)
# Los no-terminales se detectan automáticamente (todo símbolo que aparece como LHS)
# Los terminales deben coincidir con los strings que retorna tokenToParserSymbol()
```

**Ejemplo: agregar multiplicación**

En `grammar.txt`, reemplazar:

```tetx
E2 -> PLUS F E2 | MINUS F E2 | EPS
```

por:

```tetx
E2 -> PLUS T E2 | MINUS T E2 | EPS
T  -> F T2
T2 -> MULT F T2 | EPS
```

Los terminales que el lexer entrega se definen en `lexer/token/token_type.h` dentro de la función `tokenToParserSymbol()`. Si se agregan nuevos tokens al lexer, se deben registrar ahí para que el parser los reconozca.

---
