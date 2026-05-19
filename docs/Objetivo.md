# CP Lexer y Parser

**Objetivo:** Integrar las etapas del análisis léxico y sintáctico por medio del Lexer y el programa First/Follow para construir un parser funcional.

---

## Arquitectura

El proyecto se dividirá en tres módulos:

1. **El Lexer** — Ya generado; entregará una lista de tokens.
2. **El módulo de gramática (First/Follow)** — Genera una tabla LL(1).
3. **Módulo sintáctico** — Utiliza la tabla LL(1) y una pila para validar la lista de tokens.

---

## Desarrollo

1. El trabajo podrá realizarse en equipos de hasta **4 integrantes**.
2. Este proyecto será considerado como la **segunda evaluación**.
3. Contarán con **dos semanas** para el desarrollo del mismo.

---

## Consideraciones Técnicas Obligatorias

### 1. Conexión Token – Parser

El parser **no puede** usar cadenas de forma directa (como `"+"` o `"id"`), sino con la estructura de datos generada en el lexer.

- **Entrada del parser:** Una lista de objetos tipo `token { tipo, valor, línea }`
- **Match:** El proceso debe comparar el tipo del token con las columnas de la tabla LL(1).

### 2. Adaptación de la Gramática

Se debe actualizar el formato de la gramática para que los terminales coincidan con las etiquetas del lexer.

| Antes | Ahora |
| ------- | ------- |
| `E -> T + E` | `E -> T SUM E` *(donde `SUM` es el nombre del token que entrega el lexer)* |

### 3. Implementación del Motor de la Pila

Se sugiere que el parser funcione como un **algoritmo de pila no recursivo**:

1. Apilar el símbolo de fin de cadena (`$`) y el símbolo inicial de la gramática.
2. Mientras la pila no esté vacía:
   - **a.** Si el tope es un **Terminal**: comparar con el token actual del Lexer. Si coinciden, hacer `pop` y avanzar al siguiente token.
   - **b.** Si el tope es un **No Terminal**: consultar la Tabla LL(1) usando el tope y el token actual. Si hay una regla, hacer `pop` y apilar la producción a la inversa.
   - **c.** Si la celda está **vacía**: disparar error sintáctico.

### 4. Manejo de Errores de Contexto

El programa debe informar:

1. La **línea** donde ocurrió el error (por medio del lexer).
2. El **token inesperado**.

---

## Preparación para la Tabla de Símbolos

Para que el parser se complemente con las etapas siguientes, deberá alimentar la **Tabla de Símbolos persistente**.

1. **Requisito:** Cada que el lexer encuentre un `ID` (identificador) deberá guardarlo en un diccionario.
2. **Estructura mínima:**

| Nombre | Tipo | Línea | Valor inicial |
|--------|------|-------|---------------|

3. **Validación básica:** Si el parser detecta una declaración de variable, debe verificar que el identificador no exista previamente en la tabla (evitando duplicados).

---

## Ejemplo de Validación

**Código de entrada:** `int x; x=5;`

1. **Lexer:** Genera tokens `[INT, ID(x), SEMICOLON, ID(x), ASSIGN, NUM(5), SEMICOLON]`.
2. **Parser:** Consulta la tabla LL(1). Si la gramática dice que después de `INT` debe venir un `ID`, el parser avanza.
3. **Resultado:**
   - Si la cadena es válida → `"Análisis sintáctico exitoso"`.
   - Si falta un `;` → `"Error en línea 1: se esperaba SEMICOLON, se recibió ID(x)"`.

---

## Gramática de Uso

### Terminales (Tokens que el Lexer debe reconocer)

```text
PROGRAM, VAR, INT, FLOAT, ID, SEMICOLON, COLON, ASSIGN, NUM, PLUS, MULT, LPAREN, RPAREN
```

### Reglas de Producción

| # | Regla |
|---|-------|
| a | `P  → PROGRAM ID SEMICOLON D B` |
| b | `D  → VAR L \| EPS` |
| c | `L  → ID COLON T SEMICOLON L'` |
| d | `L' → L \| EPS` |
| e | `T  → INT \| FLOAT` |
| f | `B  → BEGIN S END` |
| g | `S  → ID ASSIGN E SEMICOLON S'` |
| h | `S' → S \| EPS` |
| i | `E  → F E'` |
| j | `E' → PLUS F E' \| EPS` |
| k | `F  → ID \| NUM` |

---

## Código de Uso

**Programa válido:**

```text
PROGRAM miPrograma;
VAR
  x: INT;
  y: FLOAT;
BEGIN
  x = 10;
  y = x + 5;
END
```

**Programa con errores:**

```text
PROGRAM errorPrograma;
VAR
  x: INT
BEGIN
  x = 5 + ;
END
```

---

## Entregables

1. El **código fuente** en `.txt`
2. El **video** correspondiente sobre el funcionamiento del mismo.
3. Un **log** generado por el programa llamado `traza_analisis.txt` que mostrará los pasos de la pila sugerida.
