# Análisis Completo del Proyecto LL(1) Parser

## Contenido

1. [Resumen Ejecutivo](#1-resumen-ejecutivo)
2. [Arquitectura General del Sistema](#2-arquitectura-general-del-sistema)
3. [Análisis del Módulo Léxico (Lexer)](#3-análisis-del-módulo-léxico-lexer)
4. [Análisis del Módulo de Gramática](#4-análisis-del-módulo-de-gramática)
5. [Análisis del Parser LL(1)](#5-análisis-del-parser-ll1)
6. [Análisis del Módulo Semántico](#6-análisis-del-módulo-semántico)
7. [Análisis del AST (Árbol Sintáctico Abstracto)](#7-análisis-del-ast-árbol-sintáctico-abstracto)
8. [Sistema de Trazas y Depuración](#8-sistema-de-trazas-y-depuración)
9. [Verificación de Correctitud de Salidas](#9-verificación-de-correctitud-de-salidas)
10. [Casos de Prueba y Resultados](#10-casos-de-prueba-y-resultados)
11. [Características que Faltan o Pueden Mejorar](#11-características-que-faltan-o-pueden-mejorar)
12. [Calificación y Evaluación](#12-calificación-y-evaluación)
13. [Estimación del Nivel del Programador](#13-estimación-del-nivel-del-programador)
14. [Conclusiones y Recomendaciones](#14-conclusiones-y-recomendaciones)

---

## 1. Resumen Ejecutivo

El proyecto **LL(1) Parser** es un compilador académico implementado en **C++17** que implementa las fases fundamentales del análisis de compiladores: análisis léxico, análisis sintáctico mediante técnica LL(1), análisis semántico básico y construcción del Árbol Sintáctico Abstracto (AST).

El sistema procesa un lenguaje imperativo simplificado de estilo Pascal, generando como salida:
- Lista de tokens con información de posición
- Tabla de símbolos
- Conjuntos FIRST y FOLLOW
- Tabla de análisis LL(1)
- Resultado del análisis sintáctico
- Análisis de errores semánticos
- Árbol sintáctico abstracto
- Traza opcional del proceso de parsing

**Veredicto final**: El proyecto cumple completamente con sus objetivos académicos y funciona correctamente en todas sus fases.

---

## 2. Arquitectura General del Sistema

### 2.1 Estructura de Directorios

```
src/
├── main.cpp                    ← Punto de entrada, CLI y orquestador
│
├── grammar/                    ← Módulo de gramática
│   ├── grammar.txt             ← Gramática LL(1) editable
│   ├── core/
│   │   ├── grammar.h / .cpp    ← Estructuras Grammar, Symbol, Production
│   ├── analysis/
│   │   ├── first_set.h / .cpp  ← Cálculo de conjuntos FIRST
│   │   ├── follow_set.h / .cpp ← Cálculo de conjuntos FOLLOW
│   │   ├── ll1_table.h / .cpp  ← Construcción de la tabla LL(1)
│   │   └── validator.h / .cpp  ← Validación de gramática
│   └── io/
│       └── grammar_parser.h / .cpp ← Lector del archivo grammar.txt
│
├── lexer/                      ← Módulo léxico
│   ├── lexer/
│   │   └── lexer.h / .cpp      ← Autómata de estados finitos
│   ├── token/
│   │   ├── token.h             ← Estructura Token
│   │   └── token_type.h        ← Enum TokenType + mapeo
│   ├── symbol_table/
│   │   └── symbol_table.h / .cpp ← Tabla de símbolos del lexer
│   ├── error_handler/
│   │   └── error_handler.h / .cpp ← Recolector de errores léxicos
│   └── utils/
│       └── utils.h / .cpp      ← Clasificación de caracteres
│
├── parser/                     ← Módulo sintáctico
│   ├── parser.h / .cpp         ← Motor de pila LL(1) no recursivo
│   ├── symbol_manager.h / .cpp ← Tabla de símbolos del parser
│   ├── ast/
│   │   ├── ast_node.h / .cpp   ← Nodos del AST
│   │   ├── ast_builder.cpp     ← Constructor del AST
│   │   └── ast_visitor.cpp     ← Patrón visitor
│   ├── semantic/
│   │   └── semantic_analyzer.h / .cpp ← Análisis semántico
│   └── trace/
│       ├── trace_manager.h / .cpp ← Logger de pasos
│       └── trace_formatter.cpp ← Formatos de traza
│
└── test/                       ← Programas de prueba
    ├── programa.pas            ← Programa válido
    ├── programa_error.pas      ← Variable sin declarar
    └── programa_duplicado.pas  ← Variable duplicada
```

### 2.2 Flujo de Procesamiento

```
Código fuente (.pas)
        │
        ▼
┌───────────────────────┐
│       Lexer           │  → Lista de tokens [{tipo, lexema, línea, col}]
│    (AFD - C++17)      │  → Tabla de símbolos léxica
│                       │  → Errores léxicos
└───────────┬───────────┘
            │
            ▼
┌───────────────────────────────────┐
│     Módulo de Gramática            │
│  GrammarParser → Grammar           │
│  FirstSetCalculator                │
│  FollowSetCalculator               │
│  LL1TableGenerator                 │
│  Validator                         │
└───────────────┬───────────────────┘
                │  Tabla LL(1) [NT][Terminal] → Producción
                ▼
┌───────────────────────────────────┐
│     Parser LL(1) — Pila            │
│  Motor no recursivo:               │
│  ┌──────────────────────────┐      │
│  │ Pila: [ $ | P | ... ]    │      │
│  │ Token actual: PROGRAM    │      │
│  └──────────────────────────┘      │
│  Acciones: MATCH / EXPAND / ERROR   │
│  Construye AST en paralelo         │
└───────────────┬─────────────────────┘
                │  AST + lista de tokens
                ▼
┌───────────────────────────────────┐
│     Analizador Semántico           │
│  1ª pasada: recolectar declaraciones│
│  2ª passada: validar usos          │
│  → Tabla de símbolos semántica    │
└───────────────────────────────────┘
```

### 2.3 Compilación y Ejecución

**Compilación (CMake):**
```bash
cd src/
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
# Ejecutable: ./build/bin/ll1-parser
```

**Compilación (g++ directo):**
```bash
cd src/
g++ -std=c++17 -O2 -I. \
  grammar/analysis/*.cpp \
  grammar/core/*.cpp \
  grammar/io/*.cpp \
  lexer/error_handler/*.cpp \
  lexer/lexer/*.cpp \
  lexer/symbol_table/*.cpp \
  lexer/token/*.cpp \
  lexer/utils/*.cpp \
  parser/ast/*.cpp \
  parser/parser.cpp \
  parser/semantic/*.cpp \
  parser/symbol_manager.cpp \
  parser/trace/*.cpp \
  main.cpp -o ll1_parser
```

**Ejecución:**
```bash
./build/bin/ll1-parser --input <archivo> [opciones]

Opciones:
  --input <archivo>    Archivo fuente a analizar (OBLIGATORIO)
  --grammar <archivo>  Archivo de gramática (por defecto: grammar.txt)
  --sets               Mostrar conjuntos FIRST y FOLLOW
  --table              Mostrar tabla LL(1)
  --trace              Guardar traza en traza_analisis.txt
  --trace-level <0-4>  Nivel de detalle de la traza
  --help, -h           Mostrar ayuda
```

---

## 3. Análisis del Módulo Léxico (Lexer)

### 3.1 Descripción General

El módulo léxico está implementado como un **Autómata de Estados Finitos Determinista (AFD)** que recorre el código fuente carácter por carácter, identificando tokens según patrones predefinidos.

**Ubicación**: `src/lexer/`

**Componentes principales**:
- `lexer.h` / `lexer.cpp`: Clase principal del analizador léxico
- `token.h` / `token.cpp`: Estructura que representa un token
- `token_type.h`: Enumeración de tipos de token y mapeo a símbolos del parser
- `symbol_table.h` / `symbol_table.cpp`: Tabla de identificadores únicos
- `error_handler.h` / `error_handler.cpp`: Manejo de errores léxicos
- `utils.h` / `utils.cpp`: Funciones auxiliares de clasificación

### 3.2 Tokens Reconocidos

El lexer reconoce los siguientes tipos de tokens:

| Categoría | Tokens | Ejemplo |
|-----------|--------|---------|
| Palabras reservadas | `PROGRAM`, `VAR`, `INT`, `FLOAT`, `BEGIN`, `END`, `IF`, `ELSE`, `WHILE` | `program`, `var`, `int` |
| Identificadores | `IDENT` | `miVariable`, `x`, `suma_total` |
| Literales enteros | `LIT_INT` | `0`, `10`, `255` |
| Literales reales | `LIT_FLOAT` | `3.14`, `2.0`, `0.5` |
| Operadores | `OP_PLUS`, `OP_MINUS`, `OP_MULTIPLY`, `OP_DIVIDE`, `OP_ASSIGN`, `OP_GREATER`, `OP_LESS` | `+`, `-`, `*`, `/`, `=`, `>`, `<` |
| Delimitadores | `DELIM_SEMICOLON`, `DELIM_COLON`, `DELIM_LPAREN`, `DELIM_RPAREN`, `DELIM_LBRACE`, `DELIM_RBRACE` | `;`, `:`, `(`, `)`, `{`, `}` |
| Fin de archivo | `END_OF_FILE` | EOF |
| Desconocido | `UNKNOWN` | Caracteres no reconocidos |

**Mapeo a símbolos del parser** (en `token_type.h`):

```cpp
tokenToParserSymbol(TokenType):
  KW_PROGRAM    → "PROGRAM"
  KW_VAR        → "VAR"
  KW_INT        → "INT"
  KW_FLOAT      → "FLOAT"
  KW_BEGIN      → "BEGIN"
  KW_END        → "END"
  IDENT         → "ID"
  LIT_INT       → "NUM"
  LIT_FLOAT     → "FNUM"
  OP_PLUS       → "PLUS"
  OP_MINUS      → "MINUS"
  OP_MULTIPLY   → "MULT"
  OP_ASSIGN     → "ASSIGN"
  DELIM_SEMICOLON → "SEMICOLON"
  DELIM_COLON   → "COLON"
  DELIM_LPAREN  → "LPAREN"
  DELIM_RPAREN  → "RPAREN"
  END_OF_FILE   → "$"
```

### 3.3 Estructura de la Clase Token

```cpp
struct Token {
    TokenType type;       // Clasificación del token
    std::string lexeme;   // Texto original del código fuente
    int line;             // Línea donde fue encontrado (inicia en 1)
    int column;           // Columna donde inicia el lexema (inicia en 1)
};
```

### 3.4 Autómata de Estados

El lexer implementa un autómata de estados finitos determinista con el siguiente comportamiento:

| Estado del carácter | Acción |
|---------------------|--------|
| Espacio/tab/nueva línea | Ignorar, avanzar |
| Letra o `_` | `scanIdentifierOrKeyword()` |
| Dígito | `scanNumber()` |
| Operador (`+`, `-`, `*`, `/`, `=`, `>`, `<`) | `scanOperator()` |
| Delimitador (`;`, `(`, `)`, `{`, `}`) | `scanDelimiter()` |
| Otro | Error léxico → `scanUnknown()` |

**Detalle de los escáneres**:

1. **scanIdentifierOrKeyword()**: Acumula caracteres alfanuméricos y `_`. Si el lexema coincide con una palabra reservada, clasifica como keyword; caso contrario, como IDENT.

2. **scanNumber()**: Acumula dígitos. Si encuentra `.`, verifica que haya dígitos después (para FLOAT). Detecta números mal formados como `3x`.

3. **scanOperator()**: Clasifica operadores de un solo carácter.

4. **scanDelimiter()**: Clasifica delimitadores de un solo carácter.

5. **scanUnknown()**: Registra error léxico y crea token UNKNOWN.

### 3.5 Tabla de Símbolos del Léxico

El lexer mantiene una tabla de símbolos con los identificadores únicos encontrados:

```
┌─────────────┬───────────┬────────┐
│   Lexema    │   Token   │ Línea  │
├─────────────┼───────────┼────────┤
│ miPrograma  │ IDENT     │   1    │
│ x           │ IDENT     │   3    │
│ y           │ IDENT     │   4    │
└─────────────┴───────────┴────────┘
```

**Características**:
- No permite duplicados (solo guarda la primera aparición)
- Registra línea y columna de primera aparición

### 3.6 Manejo de Errores Léxicos

**Tipos de errores léxicos** (en `error_handler.h`):

| Tipo | Descripción | Ejemplo |
|------|-------------|---------|
| `INVALID_CHARACTER` | Carácter no válido | `@`, `#`, `$` |
| `MALFORMED_NUMBER` | Número mal formedo | `3x`, `12abc` |
| `INVALID_IDENTIFIER` | Identificador comenzando con dígito | `2suma` |
| `INCOMPLETE_FLOAT` | Float sin parte decimal | `3.` |
| `UNEXPECTED_TOKEN` | Token en contexto inválido | `.` aislados |
| `GENERIC_ERROR` | Error genérico | - |

El lexer no se detiene ante errores; continúa el análisis y marca los tokens inválidos como UNKNOWN.

### 3.7 Verificación

**Salida para `test/programa.pas`**:

```
  Tokens generados : 25
  Identificadores  : 3

  ----------------------------------------------------------
  #    Línea Col   Símbolo      Categoría      Lexema
  ----------------------------------------------------------
  0    1      1     PROGRAM       Palabra clave   'program'
  1    1      9     ID            Identificador   'miPrograma'
  2    1      19    SEMICOLON     Delimitador     ';'
  3    2      1     VAR           Palabra clave   'var'
  4    3      3     ID            Identificador   'x'
  5    3      4     COLON         Delimitador     ':'
  6    3      6     INT           Palabra clave   'int'
  7    3      9     SEMICOLON     Delimitador     ';'
  8    4      3     ID            Identificador   'y'
  9    4      4     COLON         Delimitador     ':'
 10   4      6     FLOAT         Palabra clave   'float'
 11   4      11    SEMICOLON     Delimitador     ';'
 12   5      1     BEGIN         Palabra clave   'begin'
 13    6      3     ID            Identificador   'x'
 14    6      5     ASSIGN        Operador        '='
 15    6      7     NUM           Literal         '10'
 16    6      9     SEMICOLON     Delimitador     ';'
 17    7      3     ID            Identificador   'y'
 18    7      5     ASSIGN        Operador        '='
 19    7      7     ID            Identificador   'x'
 20    7      9     PLUS          Operador        '+'
 21    7      11    NUM           Literal         '5'
 22    7      12    SEMICOLON     Delimitador     ';'
 23    8      1     END           Palabra clave   'end'
 24    8      4     $             Fin de archivo  'EOF'
  ----------------------------------------------------------
```

**Veredicto**: El módulo léxico funciona correctamente y genera la salida esperada.

---

## 4. Análisis del Módulo de Gramática

### 4.1 Descripción General

El módulo de gramática se encarga de:
1. Leer y parsear la gramática desde un archivo
2. Calcular los conjuntos FIRST
3. Calcular los conjuntos FOLLOW
4. Construir la tabla de análisis LL(1)
5. Validar que la gramática sea LL(1)

**Ubicación**: `src/grammar/`

### 4.2 Formato de la Gramática

La gramática se define en `grammar.txt` con el siguiente formato:

```
# Comentarios empiezan con #

# No-terminal -> producciones ( | = alternativa, EPS = epsilon)
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

### 4.3 Gramática Soportada

**No-terminales y su significado**:

| Símbolo | Descripción |
|---------|-------------|
| P | Programa completo |
| D | Sección de declaraciones (VAR ... o EPS) |
| L | Lista de declaraciones de variables |
| L2 | Continuación de lista de variables |
| T | Tipo de dato (INT o FLOAT) |
| B | Bloque BEGIN ... END |
| S | Sentencia de asignación |
| S2 | Continuación de sentencias |
| E | Expresión aritmética |
| E2 | Continuación de expresión (+ o -) |
| F | Factor (identificador, número, número real) |

### 4.4 Estructuras de Datos

```cpp
struct Symbol {
    std::string name;
    bool isNT;   // true = No-Terminal, false = Terminal
};

using Production = std::vector<Symbol>;
using RuleMap = std::map<std::string, std::vector<Production>>;

struct Grammar {
    std::string startSymbol;
    std::vector<std::string> ntOrder;     // Orden de inserción
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    RuleMap rules;
};
```

### 4.5 Cálculo de Conjuntos FIRST

**Algoritmo**: Iteración de punto fijo (fixed-point iteration)

**Definición**:
- Si X es terminal: FIRST(X) = { X }
- Si X es epsilon (ε): FIRST(X) = { ε }
- Si X es no-terminal: FIRST(X) = union de FIRST de cada producción

**Implementación** (`first_set.cpp`):

```cpp
FirstFollowMap FirstSetCalculator::compute(const Grammar& g) {
    FirstFollowMap firstSets;
    // Inicializar conjuntos vacíos para cada NT
    for (const auto& nt : g.nonTerminals) firstSets[nt] = {};

    // Iterar hasta que no haya cambios
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& [nt, prods] : g.rules) {
            for (const auto& prod : prods) {
                StringSet toAdd = firstOfSequence(prod, firstSets);
                // Agregar y detectar cambio
            }
        }
    }
    return firstSets;
}
```

**FIRST de una secuencia** (para producciones con múltiples símbolos):

```cpp
StringSet firstOfSequence(const std::vector<Symbol>& seq, ...) {
    StringSet result;
    bool allCanBeEpsilon = true;

    for (const auto& sym : seq) {
        if (sym.isEpsilon()) { result.insert(EPSILON); break; }
        if (!sym.isNT) { result.insert(sym.name); allCanBeEpsilon = false; break; }
        // No-terminal: agregar FIRST - {ε}, verificar si puede derivar ε
    }

    if (allCanBeEpsilon) result.insert(EPSILON);
    return result;
}
```

### 4.6 Cálculo de Conjuntos FOLLOW

**Algoritmo**: Iteración de punto fijo

**Reglas**:
1. FOLLOW(S) = { $ } (S = símbolo inicial)
2. Si A → αBβ, agregar FIRST(β) - {ε} a FOLLOW(B)
3. Si A → αB o (A → αBβ y ε ∈ FIRST(β)), agregar FOLLOW(A) a FOLLOW(B)

**Implementación** (`follow_set.cpp`):

```cpp
FirstFollowMap FollowSetCalculator::compute(const Grammar& g, const FirstFollowMap& firstSets) {
    FirstFollowMap followSets;

    // Inicializar conjuntos vacíos
    for (const auto& nt : g.nonTerminals) followSets[nt] = {};

    // Regla 1: símbolo inicial tiene $
    followSets[g.startSymbol].insert(END_MARKER);

    // Iterar hasta estabilizar
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& [lhs, prods] : g.rules) {
            for (const auto& prod : prods) {
                for (size_t i = 0; i < prod.size(); ++i) {
                    const Symbol& sym = prod[i];
                    if (!sym.isNT || sym.isEpsilon()) continue;

                    std::vector<Symbol> beta(prod.begin() + i + 1, prod.end());

                    if (beta.empty()) {
                        // Regla 4: FOLLOW(A) ⊆ FOLLOW(B)
                        for (const auto& s : followSets[lhs])
                            followSets[sym.name].insert(s);
                    } else {
                        // Regla 2: FIRST(β) - {ε}
                        StringSet fb = FirstSetCalculator::firstOfSequence(beta, firstSets);
                        for (const auto& s : fb) if (s != EPSILON) followSets[sym.name].insert(s);

                        // Regla 3: si ε ∈ FIRST(β), agregar FOLLOW(A)
                        if (fb.count(EPSILON)) {
                            for (const auto& s : followSets[lhs])
                                followSets[sym.name].insert(s);
                        }
                    }
                }
            }
        }
    }
    return followSets;
}
```

### 4.7 Resultados FIRST y FOLLOW

| NT | FIRST | FOLLOW |
|----|-------|--------|
| P | { PROGRAM } | { $ } |
| D | { EPS, VAR } | { BEGIN } |
| L | { ID } | { BEGIN } |
| L2 | { EPS, ID } | { BEGIN } |
| T | { FLOAT, INT } | { SEMICOLON } |
| B | { BEGIN } | { $ } |
| S | { ID } | { END } |
| S2 | { EPS, ID } | { END } |
| E | { FNUM, ID, NUM } | { SEMICOLON } |
| E2 | { EPS, MINUS, PLUS } | { SEMICOLON } |
| F | { FNUM, ID, NUM } | { MINUS, PLUS, SEMICOLON } |

### 4.8 Construcción de la Tabla LL(1)

**Algoritmo**:
Para cada producción A → α:
1. Para cada terminal a ∈ FIRST(α) (a ≠ ε): agregar α a M[A, a]
2. Si ε ∈ FIRST(α): para cada b ∈ FOLLOW(A): agregar α a M[A, b]

**Verificación de LL(1)**:
- Si M[A, a] tiene más de una producción → **Conflicto** → No es LL(1)

**Implementación** (`ll1_table.cpp`):

```cpp
LL1TableGenerator::Result LL1TableGenerator::build(const Grammar& grammar, ...) {
    Result result;

    for (const auto& [nonTerminal, productions] : grammar.rules) {
        for (const auto& production : productions) {
            StringSet firstOfProd = FirstSetCalculator::firstOfSequence(production, firstSets);

            // FIRST - {ε}
            for (const auto& terminal : firstOfProd) {
                if (terminal == EPSILON) continue;
                addProductionsForSymbol(nonTerminal, production, {terminal}, ...);
            }

            // FOLLOW si ε ∈ FIRST
            if (firstOfProd.count(EPSILON) > 0) {
                auto it = followSets.find(nonTerminal);
                if (it != followSets.end())
                    addProductionsForSymbol(nonTerminal, production, it->second, ...);
            }
        }
    }

    // Verificar conflictos
    if (!result.errors.empty()) result.isValid = false;
    return result;
}
```

### 4.9 Tabla LL(1) Resultado

**21 entradas válidas** (ejemplo de estructura):

```
NT   BEGIN        END     FLOAT   FNUM    ID                        INT
P    -            -       -       -       -                         -
D    EPS          -       -       -       -                         -
L    -            -       -       -       ID COLON T SEMICOLON L2   -
L2   EPS          -       -       -       L                         -
T    -            -       FLOAT   -       -                         INT
B    BEGIN S END  -       -       -       -                         -
S    -            -       -       -       ID ASSIGN E SEMICOLON S2  -
S2   -            EPS     -       -       S                         -
E    -            -       -       F E2    F E2                      -
E2   -            -       -       -       -                         -
F    -            -       -       FNUM    ID                        -
```

### 4.10 Validación de la Gramática

El validator verifica:
1. **Recursión izquierda directa**: A → A ...
2. **No-terminales indefinidos**: Símbolos en RHS sin producción

**Salida para la gramática actual**:
```
✓ Gramática bien formada (sin recursión izquierda ni NT indefinidos)
```

---

## 5. Análisis del Parser LL(1)

### 5.1 Descripción General

El parser implementa un **motor de pila no recursivo** que utiliza la tabla LL(1) para realizar el análisis sintáctico.

**Ubicación**: `src/parser/parser.h` / `parser.cpp`

### 5.2 Algoritmo

```
1. Inicializar: push($), push(símbolo_inicial)
2. token_actual = primer token
3. Mientras pila no vacía:
    a. Si tope == "$":
         - Si token == "$": ÉXITO
         - Si no: ERROR
    b. Si tope es Terminal:
         - Si tope == token: POP + avanzartoken (MATCH)
         - Si no: ERROR
    c. Si tope es No-Terminal:
         - Consultar Tabla[tope][token]
         - Si hay producción: POP + apilar producción en orden inverso (EXPAND)
         - Si celda vacía: ERROR
```

### 5.3 Estructura de la Clase Parser

```cpp
class LL1Parser {
private:
    std::stack<std::string> stack_;           // Pila de análisis
    std::vector<Token> tokens_;               // Tokens de entrada
    size_t currentTokenIndex_ = 0;             // Índice del token actual
    LL1Table ll1Table_;                       // Tabla LL(1)
    std::string startSymbol_;                 // Símbolo inicial
    std::set<std::string> nonTerminals_;      // Conjunto de no-terminales

    ASTBuilder astBuilder_;                   // Constructor del AST
    TraceManager traceManager_;               // Gestor de trazas
    SymbolManager symbolManager_;             // Tabla de símbolos

public:
    Result parse(const std::vector<Token>& tokens,
                 const LL1Table& ll1Table,
                 const std::string& startSymbol,
                 const std::vector<std::string>& nonTerminals);
};
```

### 5.4 Manejo de Errores Sintácticos

El parser reporta:
- Línea y columna del error
- Token esperado
- Token recibido
- No-terminal que se estaba expandiendo

**Ejemplo de error**:
```
Error sintáctico (línea 4, col 1): se esperaba SEMICOLON, se recibió BEGIN('BEGIN')
Error sintáctico (línea 5, col 9): token inesperado SEMICOLON(';') al expandir no-terminal F
```

### 5.5 Resultado del Parser

```cpp
struct Result {
    bool success;                              // true = análisis exitoso
    std::shared_ptr<ASTNode> ast;              // Árbol sintáctico
    std::vector<std::string> errors;          // Lista de errores
    int errorCount;                            // Conteo de errores
};
```

---

## 6. Análisis del Módulo Semántico

### 6.1 Descripción General

El analizador semántico realiza validación de contexto después de que el análisis sintáctico ha procesado exitosamente el código.

**Ubicación**: `src/parser/semantic/semantic_analyzer.h` / `semantic_analyzer.cpp`

### 6.2 Verificaciones Implementadas

El analyzer realiza dos pasadas sobre la lista de tokens:

**Primera pasada (recolectar declaraciones)**:
- Recorrer tokens en la sección VAR (entre `VAR` y `BEGIN`)
- Registrar cada identificación junto con su tipo (INT/FLOAT)
- Detectar declaraciones duplicadas

**Segunda pasada (validar usos)**:
- Recorrer tokens en el bloque (entre `BEGIN` y `END`)
- Verificar que cada identificador usado haya sido declarado
- Marcar variables como "usadas"

### 6.3 Estructuras de Datos

```cpp
struct SemanticSymbol {
    std::string name;     // Nombre de la variable
    std::string type;    // "int" o "float"
    int declLine;        // Línea de declaración
    int declCol;         // Columna de declaración
    bool used = false;   // Si fue usada en el código
};

struct SemanticResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, SemanticSymbol> symbols;
};
```

### 6.4 Errores Detectados

| Error | Descripción | Ejemplo |
|-------|-------------|---------|
| Variable no declarada | Uso de variable sin declaración | `y = 10;` (sin `y` en VAR) |
| Variable duplicada | Declaración dos veces | `x: int; x: float;` |
| Advertencia | Variable declarada pero no usada | `x: int;` (nunca usada) |

### 6.5 Resultado para Casos de Prueba

**programa_error.pas** (variable sin declarar):
```
✗ Errores semánticos (1):
  - Error semántico (línea 5): variable 'y' usada sin declarar.
⚠ Advertencias (1):
  - Advertencia: variable 'x' declarada en línea 3 nunca se usa.
```

**programa_duplicado.pas** (declaración duplicada):
```
✗ Errores semánticos (1):
  - Error semántico (línea 4): variable 'x' ya fue declarada en línea 3.
```

**programa.pas** (válido):
```
✓ Sin errores semánticos.
```

---

## 7. Análisis del AST (Árbol Sintáctico Abstracto)

### 7.1 Descripción General

El AST representa la estructura jerárquica del programa.parseado sin incluir información redundante (como delimitadores).

**Ubicación**: `src/parser/ast/ast_node.h` / `ast_node.cpp`

### 7.2 Tipos de Nodos

```cpp
enum class ASTNodeType {
    // Estructura del programa
    PROGRAM,
    DECLARATIONS,
    DECLARATION,
    VARIABLE_LIST,
    TYPE_SPECIFIER,
    BLOCK,

    // Sentencias
    STATEMENT_LIST,
    STATEMENT,
    ASSIGN_STATEMENT,

    // Expresiones
    EXPRESSION,
    EXPRESSION_PRIME,
    FACTOR,

    // Terminales
    IDENTIFIER,
    NUMBER,
    PLUS, MINUS, MULT, ASSIGN, SEMICOLON, COLON,
    LPAREN, RPAREN,
    PROGRAM_KW, VAR_KW, BEGIN_KW, END_KW, INT_KW, FLOAT_KW,

    // Especial
    EPSILON,
    UNKNOWN
};
```

### 7.3 Estructura de un Nodo

```cpp
class ASTNode {
public:
    ASTNodeType type;                              // Tipo de nodo
    std::string lexeme;                            // Lexema (para hojas)
    int line;                                      // Línea
    int column;                                    // Columna
    std::vector<std::shared_ptr<ASTNode>> children;
};
```

### 7.4 Construcción del AST

El parser construye el AST **en paralelo** durante el análisis:

1. Cuando se expande un no-terminal, se crea un nodo соответствующий
2. El nodo se apila en un `astStack_` separado
3. Los terminales se agregan como hijos del nodo actual
4. Un marcador `$$AST_POP$$` señala cuándo cerrar un nodo

### 7.5 Verificación: AST Generado vs Código Fuente

**Código fuente** (`test/programa.pas`):
```pascal
program miPrograma;
var
  x: int;
  y: float;
begin
  x = 10;
  y = x + 5;
end
```

**AST generado**:
```
└── PROGRAMA
    ├── id:miPrograma (1:9)
    ├── DECLARACIONES
    │   └── DECLARACION
    │       ├── id:x (3:3)
    │       ├── TIPO
    │       │   └── int
    │       └── DECLARACION
    │           ├── id:y (4:3)
    │           └── TIPO
    │               └── float
    └── BLOQUE
        └── SENTENCIA
            ├── id:x (6:3)
            ├── =
            ├── EXPRESION
            │   └── FACTOR
            │       └── num:10 (6:7)
            └── SENTENCIA
                ├── id:y (7:3)
                ├── =
                └── EXPRESION
                    ├── FACTOR
                    │   └── id:x (7:7)
                    └── EXPR_CONT
                        ├── +
                        └── FACTOR
                            └── num:5 (7:11)
```

**Verificación**:
- ✓ Programa: `miPrograma` en línea 1, columna 9
- ✓ Declaración x: int en línea 3, columna 3
- ✓ Declaración y: float en línea 4, columna 3
- ✓ Asignación x = 10 en línea 6
- ✓ Asignación y = x + 5 en línea 7

**CONCLUSIÓN**: El AST se genera correctamente y refleja fielmente la estructura del programa.

---

## 8. Sistema de Trazas y Depuración

### 8.1 Descripción General

El sistema de trazas permite visualizar el proceso interno del parser.

**Ubicación**: `src/parser/trace/trace_manager.h` / `trace_formatter.cpp`

### 8.2 Niveles de Traza

| Nivel | Nombre | Descripción |
|-------|--------|-------------|
| 0 | NONE | Sin traza |
| 1 | MINIMAL | Solo errores y resultado final |
| 2 | NORMAL | Cada paso, MATCH y EXPAND (predeterminado) |
| 3 | VERBOSE | NORMAL + estado de pila en cada paso |
| 4 | DEBUG | VERBOSE con separadores visuales |

### 8.3 Acciones Registradas

| Acción | Descripción |
|--------|-------------|
| INITIALIZATION | Inicialización del parser |
| STEP | Estado actual: pila vs token |
| MATCH | Terminal consumido exitosamente |
| EXPAND | Expansión de no-terminal |
| ERROR | Error sintáctico detectado |
| SUCCESS | Análisis completado |

### 8.4 Archivo de Traza

La traza se guarda en `traza_analisis.txt` (siempre que se use `--trace`):

**Ejemplo de contenido (nivel 2)**:
```
══════════════════════════════════════════════════════════
   0 | [INIT] Inicializacion del parser
         Pila: $ P
         Tokens de entrada: 25
══════════════════════════════════════════════════════════
   1 | [STEP] $ P vs PROGRAM
         Pila: $ P
══════════════════════════════════════════════════════════
   0 | [EXPAND] Expandir no-terminal: P -> PROGRAM ID SEMICOLON D B
══════════════════════════════════════════════════════════
   2 | [STEP] $ B D SEMICOLON ID PROGRAM vs PROGRAM
         Pila: $ B D SEMICOLON ID PROGRAM
══════════════════════════════════════════════════════════
   2 | [MATCH] Match - Terminal coincide: PROGRAM - Token: 'program'
══════════════════════════════════════════════════════════
   3 | [STEP] $ B D SEMICOLON ID vs ID
         Pila: $ B D SEMICOLON ID
...
  46 | [OK] Analisis exitoso
══════════════════════════════════════════════════════════
```

### 8.5 Comparación de Tamaños

| Nivel | Líneas en traza |
|-------|-----------------|
| 0 (NONE) | ~95 |
| 1 (MINIMAL) | ~95 |
| 2 (NORMAL) | **95** |
| 3 (VERBOSE) | **141** |
| 4 (DEBUG) | **327** |

---

## 9. Verificación de Correctitud de Salidas

### 9.1 Análisis Léxico

| Aspecto | Esperado | Obtenido | Estado |
|---------|----------|----------|--------|
| Tokens totales | 25 | 25 | ✓ |
| Identificadores únicos | 3 | 3 | ✓ |
| Posiciones correctas | Sí | Sí | ✓ |
| Clasificación correcta | Sí | Sí | ✓ |

### 9.2 Tabla de Símbolos

| Variable | Tipo | Línea | Esperado | Obtenido |
|----------|------|-------|----------|----------|
| x | int | 3 | x, int, 3 | ✓ |
| y | float | 4 | y, float, 4 | ✓ |

### 9.3 Gramática y Tabla LL(1)

| Aspecto | Estado |
|---------|--------|
| FIRST calculados correctamente | ✓ |
| FOLLOW calculados correctamente | ✓ |
| Tabla LL(1) sin conflictos | ✓ |
| Entradas en tabla | 21 |

### 9.4 Análisis Semántico

| Caso | Error esperado | Detectado |
|------|----------------|-----------|
| Variable sin declarar | Sí | ✓ |
| Variable duplicada | Sí | ✓ |
| Variable no usada | Advertencia | ✓ |

### 9.5 AST

| Aspecto | Verificación |
|---------|---------------|
| Estructura jerárquica | ✓ |
| Nodos correctos | ✓ |
| Posiciones (línea, columna) | ✓ |
| Coincidencia con código fuente | ✓ |

---

## 10. Casos de Prueba y Resultados

### 10.1 Programa Válido (`programa.pas`)

**Código**:
```pascal
program miPrograma;
var
  x: int;
  y: float;
begin
  x = 10;
  y = x + 5;
end
```

**Resultado**:
- ✓ Análisis sintáctico exitoso
- ✓ Sin errores semánticos
- ✓ 2 símbolos declarados (x, y)
- ✓ Estado: ÉXITO
- ✓ Errores: 0

### 10.2 Programa con Error Semántico (`programa_error.pas`)

**Código**:
```pascal
program miPrograma;
var
  x: int;
begin
  y = 10;
end
```

**Resultado**:
- ✓ Análisis sintáctico exitoso
- ✗ Error semántico: variable 'y' usada sin declarar (línea 5)
- ⚠ Advertencia: variable 'x' declarada en línea 3 nunca se usa
- ✗ Estado: FALLIDO

### 10.3 Programa con Declaración Duplicada (`programa_duplicado.pas`)

**Código**:
```pascal
program miPrograma;
var
  x: int;
  x: float;
begin
  x = 10;
end
```

**Resultado**:
- ✓ Análisis sintáctico exitoso
- ✗ Error semántico: variable 'x' ya fue declarada en línea 3 (línea 4)
- ✗ Estado: FALLIDO

---

## 11. Características que Faltan o Pueden Mejorar

### 11.1 Funcionalidades del Lenguaje

| Característica | Estado | Notas |
|----------------|--------|-------|
| Declaraciones de variables | ✅ Implementado | Solo int y float |
| Asignaciones | ✅ Implementado | Con expresiones aritméticas |
| Operadores + y - | ✅ Implementado | Binarios |
| Operador * | ⚠️ En lexer, no en gramática | No usable |
| Comparadores (> <) | ⚠️ En lexer, no en gramática | No usable |
| Sentencias IF | ❌ No implementado | Palabras reservadas existen |
| Ciclos WHILE | ❌ No implementado | Palabras reservadas existen |
| Funciones/Procedimientos | ❌ No implementado | - |
| Arrays | ❌ No implementado | - |
| Registros/Structs | ❌ No implementado | - |
| Llamadas a funciones | ❌ No implementado | - |

### 11.2 Mejoras en el Compilador

| Característica | Estado | Notas |
|----------------|--------|-------|
| Verificación de tipos | ❌ No implementado | No valida compatibilidad int/float |
| Ámbito (scopes) | ⚠️ Parcial | SymbolManager tiene soporte, pero no se usa activamente |
| Recuperación de errores | ❌ No implementado | Solo reporta, no recupera |
| Comentarios | ❌ No implementado | Serían tratados como error |
| Tratamiento de errores léxicos | ✅ Implementado | Continúa análisis |
| Análisis semántico basado en AST | ❌ Funciona sobre tokens | Podría usar el AST |

### 11.3 Mejoras en Código

| Característica | Estado | Notas |
|----------------|--------|-------|
| Tests unitarios | ❌ No existen | - |
| Documentación interna | ⚠️ Parcial | README extenso, pero poco en código |
| Manejo de memoria | ✅ Correcto | Uso de smart pointers |
| Manejo de excepciones | ⚠️ Mínimo | No usa excepciones |

---

## 12. Calificación y Evaluación

### 12.1 Tabla de Calificación

| Criterio | Peso | Nota (0-10) |
|----------|------|-------------|
| Pipeline completo (Lexer → Parser → Semántico → AST) | 20% | 9.0 |
| Correctitud de salidas | 20% | 9.0 |
| Documentación | 15% | 9.0 |
| Manejo de errores | 15% | 7.0 |
| Extensibilidad | 10% | 7.0 |
| Calidad del código | 10% | 8.0 |
| Testing | 10% | 5.0 |

**Nota final: 8.0 / 10**

### 12.2 Evaluación por Criterios

| Criterio | Evaluación |
|----------|------------|
| ¿Compila correctamente? | ✅ Sí, con CMake y g++ |
| ¿Ejecuta sin errores? | ✅ Sí |
| ¿Detecta errores léxicos? | ✅ Sí |
| ¿Detecta errores sintácticos? | ✅ Sí |
| ¿Detecta errores semánticos? | ✅ Sí |
| ¿Genera AST correcto? | ✅ Sí |
| ¿Cumple requisitos del curso? | ✅ Sí |
| ¿Listo para producción? | ❌ No (proyecto académico) |

---

## 13. Estimación del Nivel del Programador

### 13.1 Fortalezas Demostradas

El código evidencia un nivel **intermedio-avanzado** en las siguientes áreas:

1. **Algoritmos de compiladores**: Uso correcto de FIRST, FOLLOW, tabla LL(1), parser de pila
2. **Programación orientada a objetos**: Clases bien diseñadas con responsabilidades claras
3. **C++ moderno**: Uso de smart pointers, containers STL, templates, move semantics
4. **Patrones de diseño**: Visitor para AST, gestión de errores, separación de módulos
5. **Documentación**: README exhaustivo (~620 líneas)

### 13.2 Áreas de Mejora

1. **Testing**: No hay tests unitarios automatizados
2. **Análisis semántico**: Trabaja sobre tokens en lugar de AST
3. **Extensibilidad**: La gramática es editable pero limitada
4. **Manejo de errores**: Solo reporta, no recupera

### 13.3 Conclusión

**Nivel estimado: Estudiante de compiladores (ciclo intermedio-avanzado)**

El proyecto cumple completamente con los objetivos académicos de un curso de compiladores, demostrando comprensión de los conceptos fundamentales y capacidad de implementación en C++ moderno.

---

## 14. Conclusiones y Recomendaciones

### 14.1 Conclusiones

1. **El proyecto funciona correctamente**: Todas las fases (léxico, sintáctico, semántico) producen salidas correctas y verificadas.

2. **Cumple los objetivos académicos**: Según `docs/Objetivo.md`, el proyecto integra las etapas del análisis léxico y sintáctico, construye un parser funcional con tabla LL(1), alimenta la tabla de símbolos, detecta errores de contexto y genera traza.

3. **AST se genera correctamente**: El árbol sintáctico refleja fielmente la estructura del código fuente con posiciones correctas.

4. **Es un proyecto académico sólido**: No está listo para producción, pero cumple perfectamente su propósito educativo.

### 14.2 Recomendaciones

**Para mejorar el proyecto**:
1. Agregar tests unitarios con Google Test o Catch2
2. Implementar verificación de tipos (int vs float)
3. Completar la gramática con soporte para IF, WHILE
4. Hacer que el análisis semántico trabaje sobre el AST
5. Agregar soporte para scopes (funciones, bloques anidados)

**Para usar el proyecto**:
1. Para ver FIRST/FOLLOW: `./ll1-parser --input test/programa.pas --sets`
2. Para ver tabla LL(1): `./ll1-parser --input test/programa.pas --table`
3. Para ver traza: `./ll1-parser --input test/programa.pas --trace --trace-level 4`
4. Para ver todo: `./ll1-parser --input test/programa.pas --sets --table --trace`

---

*Documento generado como análisis objetivo del proyecto LL(1) Parser*
*Fecha: Mayo 2026*