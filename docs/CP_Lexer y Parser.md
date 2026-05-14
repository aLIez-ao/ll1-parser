Objetivo: Integrar las etapas del análisis léxico y sintáctico por medio del Lexer y el programa
First/Follow para construir un parser funcional
Arquitectura. El proyecto se dividirá en dos:
1. El lexer. Ya generado y que entregará una lista de tokens
2. El módulo de gramática, (first/follow) que genera una tabla LL1
3. Módulo sintáctico. Que utilizará la tabla LL1 y una pila para validar la lista de
tokens.
Desarrollo:
1. El trabajo podrá realizar en equipos de hasta 4 integrantes.
2. Este proyecto será considerado como su segunda evaluación.
3. Contarán con dos semanas para el desarrollo del mismo.
Consideraciones técnicas obligatorias.
1. Conexión Token - Parser
El parser no puede usar cadenas de forma directa como ( “+” o “id” ) sino con la
estructura de datos generada en el lexer.
● Entrada del parser. Una lista de objetos tipo: token {tipo, valor, lìnea}
● Match. El proceso debe comparar el tipo del token con las columnas de la tabla
LL1
2. Adaptación de la gramática. Se debe actualizar el formato de la gramática para que los
terminales coincidan con las etiquetas del lexer.
○ Antes: E-> T + E
○ Ahora: E -> T SUM E (donde SUM es el nombre del token que entrega el lexer)
3. Implementación del motor de la pila.
Se sugiere que el parser funcione como un algoritmo de pila no recursivo de la siguiente
forma:
1. Apilar el símbolo de fin de cadena ($) y el símbolo inicial de la gramática
2. Mientras la pila no este vacia:
a. Si el tope es un Terminal: Comparar con el token actual del Lexer. Si
coinciden, hacer pop y avanzar al siguiente token.
b. Si el tope es un No Terminal: Consultar la Tabla LL(1) usando el tope y
el token actual. Si hay una regla, hacer pop y apilar la producción a la
inversa.
c. Si la celda está vacía: Disparar error sintáctico.
4. Manejo de errores de contexto
El programa debe informar:
1. La línea donde ocurrió el error. (por medio del lexer)
2. El token inespaerado
Preparación para la tabla de símbolos.
Para que el parser se complemente con las etapas siguientes este deberá alimentar la Tabla de
Símbolos persistente.
1. Requisito. Cada que el lexer encuentre ID (identificador) deberá guardarlo en un
diccionario.
2. Estructura mínima. | Nombre | Tipo | Línea | Valor inicial |
3. Validación básica. Si el parser detecta una declaración de variable, debe verificar que
identificador no exista previamente en la tabla. (evitando duplicados)
Ejemplo de validación
Código de entrada: int x; x=5;
1. Lexer: Genera tokens [INT, ID(x), SEMICOLON, ID(x), ASSIGN,
NUM(5), SEMICOLON].
2. Parser: Consulta la tabla LL(1). Si la gramática dice que después de INT debe
venir un ID, el parser avanza.
3. Resultado: Si la cadena es válida, el programa imprime "Análisis sintáctico
exitoso". Si falta un ;, debe decir: "Error en línea 1: se esperaba
SEMICOLON, se recibió ID(x)".
Gramática de uso.
1. Terminales (Tokens que su Lexer debe reconocer):
PROGRAM, VAR, INT, FLOAT, ID, SEMICOLON, COLON, ASSIGN, NUM, PLUS, MULT,
LPAREN, RPAREN
2. Reglas de Producción:
a. P→ PROGRAM ID SEMICOLON D B
b. D → VAR L | EPS
c. L → ID COLON T SEMICOLON L’
d. L’ → L | EPS
e. T → INT | FLOAT
f. B → BEGIN S END
g. S → ID ASSIGN E SEMICOLON S’
h. S’ →S | EPS
i. E → F E’
j. E’ → PLUS F E’ | EPS
k. F → ID | NUM
Código de uso.
PROGRAM miPrograma;
VAR
x: INT;
y: FLOAT;
BEGIN
x = 10;
y = x + 5;
END
PROGRAM errorPrograma;
VAR
x: INT
BEGIN
x = 5 + ;
END
Entregables.
1. El código fuente en .txt
2. El video correspondiente sobre el funcionamiento del mismo.
3. Un log generado por el programa llamado traza_analisis.txt que mostrará los pasos de
la pila sugerida.