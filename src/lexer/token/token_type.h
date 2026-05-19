/**
 * @file token_type.h
 * @brief Definición de tipos de token y funciones de conversión
 * @description
 * Este archivo contiene la enumeración TokenType que clasifica todos los
 * tokens reconocibles por el lexer, así como funciones de conversión
 * para mapear entre el tipo interno del lexer y los símbolos que el
 * parser utiliza para consultar la tabla LL(1).
 * 
 * La estructura de tokens se divide en las siguientes categorías:
 * - Palabras reservadas (keywords): program, var, int, float, begin, end, if, else, while
 * - Literales: números enteros y de punto flotante
 * - Identificadores: nombres de variables definidos por el usuario
 * - Operadores: símbolos aritméticos y de asignación
 * - Delimitadores: signos de puntuación del lenguaje
 * - Especiales: fin de archivo y tokens desconocidos
 * 
 * @author Equipo Lexer
 * @version 1.0
 */

#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <string>

/**
 * @enum TokenType
 * @brief Enumeración que define todos los tipos de tokens reconocibles por el lexer
 * @description
 * Cada valor representa una categoría léxica del lenguaje. El lexer usa estos
 * tipos para clasificar los lexemas encontrados durante el análisis.
 * 
 * Categorías de tokens:
 * - Palabras reservadas (KW_*): palabras clave del lenguaje con significado predefinido
 * - Literales (LIT_*): valores constantes como números enteros y reales
 * - Identificadores (IDENT): nombres definidos por el usuario para variables
 * - Operadores (OP_*): símbolos que representan operaciones aritméticas o de comparación
 * - Delimitadores (DELIM_*): signos de puntuación que estructuran el código
 * - Especiales: marcadores especiales como fin de archivo o errores
 */
enum class TokenType {

    // ---------------------------------------------------------
    // Palabras reservadas del lenguaje (Keywords)
    // ---------------------------------------------------------
    KW_PROGRAM,     ///< Palabra clave 'program' - encabezado de programa
    KW_VAR,        ///< Palabra clave 'var' - inicio de sección de declaraciones
    KW_INT,        ///< Palabra clave 'int' - tipo de dato entero
    KW_FLOAT,      ///< Palabra clave 'float' - tipo de dato real
    KW_BEGIN,      ///< Palabra clave 'begin' - inicio de bloque de código
    KW_END,        ///< Palabra clave 'end' - fin de bloque de código
    KW_IF,         ///< Palabra clave 'if' - estructura condicional (reservado, no usado)
    KW_ELSE,       ///< Palabra clave 'else' - parte falsa de condición (reservado, no usado)
    KW_WHILE,      ///< Palabra clave 'while' - ciclo while (reservado, no usado)

    // ---------------------------------------------------------
    // Literales numéricos
    // ---------------------------------------------------------
    LIT_INT,        ///< Literal entero: secuencia de dígitos (ej: 10, 42, 0)
    LIT_FLOAT,      ///< Literal de punto flotante: dígitos con punto decimal (ej: 3.14, 2.0)

    // ---------------------------------------------------------
    // Identificadores
    // ---------------------------------------------------------
    IDENT,          ///< Identificador: nombre definido por el usuario (ej: suma, promedio, resultado)

    // ---------------------------------------------------------
    // Operadores aritméticos y de comparación
    // ---------------------------------------------------------
    OP_PLUS,        ///< Operador suma: '+'
    OP_MINUS,       ///< Operador resta: '-'
    OP_MULTIPLY,    ///< Operador multiplicación: '*'
    OP_DIVIDE,      ///< Operador división: '/' (reservado, no implementado en gramática)
    OP_ASSIGN,      ///< Operador asignación: '='
    OP_GREATER,     ///< Operador mayor que: '>'
    OP_LESS,        ///< Operador menor que: '<' (reservado, no usado en gramática)

    // ---------------------------------------------------------
    // Delimitadores
    // ---------------------------------------------------------
    DELIM_SEMICOLON,    ///< Punto y coma: ';'
    DELIM_COLON,        ///< Dos puntos: ':'
    DELIM_LPAREN,       ///< Paréntesis izquierdo: '('
    DELIM_RPAREN,       ///< Paréntesis derecho: ')'
    DELIM_LBRACE,       ///< Llave izquierda: '{' (reservada, no usada)
    DELIM_RBRACE,       ///< Llave derecha: '}' (reservada, no usada)

    // ---------------------------------------------------------
    // Especiales
    // ---------------------------------------------------------
    END_OF_FILE,    ///< Marcador de fin de archivo: '$'
    UNKNOWN         ///< Token no reconocido o error léxico
};

    // ---------------------------------------------------------
    // Palabras reservadas del lenguaje
    // ---------------------------------------------------------
    KW_PROGRAM,     // program
    KW_VAR,         // var
    KW_INT,         // int
    KW_FLOAT,       // float
    KW_BEGIN,       // begin
    KW_END,         // end
    KW_IF,          // if
    KW_ELSE,        // else
    KW_WHILE,       // while

    // ---------------------------------------------------------
    // Literales
    // ---------------------------------------------------------
    LIT_INT,        // ej: 10, 42, 0
    LIT_FLOAT,      // ej: 3.14, 2.0

    // ---------------------------------------------------------
    // Identificadores
    // ---------------------------------------------------------
    IDENT,          // ej: suma, promedio, resultado

    // ---------------------------------------------------------
    // Operadores
    // ---------------------------------------------------------
    OP_PLUS,        // +
    OP_MINUS,       // -
    OP_MULTIPLY,    // *
    OP_DIVIDE,      // /
    OP_ASSIGN,      // =
    OP_GREATER,     // >
    OP_LESS,        // <

    // ---------------------------------------------------------
    // Delimitadores
    // ---------------------------------------------------------
    DELIM_SEMICOLON,    // ;
    DELIM_COLON,        // :
    DELIM_LPAREN,       // (
    DELIM_RPAREN,       // )
    DELIM_LBRACE,       // {
    DELIM_RBRACE,       // }

    // ---------------------------------------------------------
    // Especiales
    // ---------------------------------------------------------
    END_OF_FILE,    // fin de la cadena de entrada
    UNKNOWN         // token no reconocido / error léxico
};


/**
 * @brief Convierte un TokenType al símbolo que el parser usa para consultar la tabla LL(1)
 * @description
 * Esta función establece la conexión entre el lexer y el parser. El parser no trabaja
 * con las etiquetas originales del lexer (como "+" o "IDENT"), sino con símbolos
 * normalizados que coinciden con los nombres de terminal en la gramática LL(1).
 * 
 * Mapeo de tokens:
 * - KW_PROGRAM    -> "PROGRAM"   (palabra clave 'program')
 * - KW_VAR        -> "VAR"       (palabra clave 'var')
 * - KW_INT        -> "INT"       (palabra clave 'int')
 * - KW_FLOAT      -> "FLOAT"     (palabra clave 'float')
 * - KW_BEGIN      -> "BEGIN"     (palabra clave 'begin')
 * - KW_END        -> "END"       (palabra clave 'end')
 * - IDENT         -> "ID"        (identificador de variable)
 * - LIT_INT       -> "NUM"       (número entero)
 * - LIT_FLOAT     -> "FNUM"      (número de punto flotante)
 * - OP_PLUS       -> "PLUS"      (operador suma)
 * - OP_MINUS      -> "MINUS"     (operador resta)
 * - OP_MULTIPLY   -> "MULT"      (operador multiplicación)
 * - OP_ASSIGN     -> "ASSIGN"    (operador asignación)
 * - END_OF_FILE   -> "$"         (marcador de fin de entrada)
 * 
 * @param type Tipo de token del lexer
 * @return std::string Símbolo que el parser usa para consultar la tabla LL(1)
 * 
 * @note Esta función es crítica para la conexión Lexer-Parser. Si se agregan
 *       nuevos tokens al lexer, deben agregarse aquí para que el parser los reconozca.
 */
inline std::string tokenToParserSymbol(TokenType type) {
    switch (type) {
        case TokenType::KW_PROGRAM:       return "PROGRAM";
        case TokenType::KW_VAR:           return "VAR";
        case TokenType::KW_INT:           return "INT";
        case TokenType::KW_FLOAT:         return "FLOAT";
        case TokenType::KW_BEGIN:         return "BEGIN";
        case TokenType::KW_END:           return "END";
        case TokenType::IDENT:            return "ID";
        case TokenType::LIT_INT:          return "NUM";
        case TokenType::LIT_FLOAT:        return "FNUM";
        case TokenType::OP_PLUS:          return "PLUS";
        case TokenType::OP_MINUS:         return "MINUS";
        case TokenType::OP_MULTIPLY:      return "MULT";
        case TokenType::OP_ASSIGN:        return "ASSIGN";
        case TokenType::DELIM_SEMICOLON:  return "SEMICOLON";
        case TokenType::DELIM_COLON:      return "COLON";
        case TokenType::DELIM_LPAREN:     return "LPAREN";
        case TokenType::DELIM_RPAREN:     return "RPAREN";
        case TokenType::END_OF_FILE:      return "$";
        default:                          return "UNKNOWN";
    }
}


/**
 * @brief Convierte un TokenType a su representación en string legible
 * @description
 * Función de utilidad para imprimir tablas, reportes y depuración. Convierte
 * el tipo de token enum a una cadena descriptiva que indica la categoría
 * del token en un formato entendible por humanos.
 * 
 * @param type Tipo de token a convertir
 * @return std::string Representación legible del tipo de token
 * 
 * @note Útil para:
 *       - Imprimir la tabla de análisis léxico
 *       - Mostrar errores léxicos en reportes
 *       - Depuración del lexer
 *       - Logging del proceso de análisis
 */
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Palabras reservadas
        case TokenType::KW_PROGRAM:       return "KW_PROGRAM";
        case TokenType::KW_VAR:           return "KW_VAR";
        case TokenType::KW_INT:           return "KW_INT";
        case TokenType::KW_FLOAT:         return "KW_FLOAT";
        case TokenType::KW_BEGIN:         return "KW_BEGIN";
        case TokenType::KW_END:           return "KW_END";
        case TokenType::KW_IF:            return "KW_IF";
        case TokenType::KW_ELSE:          return "KW_ELSE";
        case TokenType::KW_WHILE:         return "KW_WHILE";

        // Literales
        case TokenType::LIT_INT:          return "LIT_INT";
        case TokenType::LIT_FLOAT:        return "LIT_FLOAT";

        // Identificadores
        case TokenType::IDENT:            return "IDENT";

        // Operadores
        case TokenType::OP_PLUS:          return "OP_PLUS";
        case TokenType::OP_MINUS:         return "OP_MINUS";
        case TokenType::OP_MULTIPLY:      return "OP_MULTIPLY";
        case TokenType::OP_DIVIDE:        return "OP_DIVIDE";
        case TokenType::OP_ASSIGN:        return "OP_ASSIGN";
        case TokenType::OP_GREATER:       return "OP_GREATER";
        case TokenType::OP_LESS:          return "OP_LESS";

        // Delimitadores
        case TokenType::DELIM_SEMICOLON:  return "DELIM_SEMICOLON";
        case TokenType::DELIM_COLON:      return "DELIM_COLON";
        case TokenType::DELIM_LPAREN:     return "DELIM_LPAREN";
        case TokenType::DELIM_RPAREN:     return "DELIM_RPAREN";
        case TokenType::DELIM_LBRACE:     return "DELIM_LBRACE";
        case TokenType::DELIM_RBRACE:     return "DELIM_RBRACE";

        // Especiales
        case TokenType::END_OF_FILE:      return "EOF";
        case TokenType::UNKNOWN:          return "UNKNOWN";

        default:                          return "UNDEFINED";
    }
}

#endif // TOKEN_TYPE_H