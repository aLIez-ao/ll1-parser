/**
 * @file token.h
 * @brief Definición de la estructura Token y sus métodos de utilidad
 * @description
 * Representa la unidad mínima reconocida por el lexer. Cada vez que el lexer
 * identifica un lexema válido o inválido, construye y almacena un Token
 * con toda su información contextual para ser consumido posteriormente por el parser.
 * 
 * El token es la estructura fundamental que conecta el análisis léxico con
 * el análisis sintáctico. Contiene:
 * - El tipo de token (clasificación léxica)
 * - El lexema (texto original del código fuente)
 * - La posición (línea y columna) donde aparece en el código fuente
 * 
 * Esta estructura también proporciona métodos de utilidad para:
 * - Verificar la categoría del token (keyword, literal, operador, etc.)
 * - Convertir al símbolo que el parser usa para la tabla LL(1)
 * - Generar representaciones en string para debugging y reportes
 * 
 * @author Equipo Lexer
 * @version 1.0
 * @see TokenType, tokenToParserSymbol()
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>
#include "token_type.h"

/**
 * @struct Token
 * @brief Estructura que almacena la información completa de un componente léxico
 * @description
 * Cada token representa una unidad atómica del código fuente que el lexer
 * ha identificado y clasificado. La estructura mantiene:
 * - type: la clasificación del token (enum TokenType)
 * - lexeme: el texto exacto como aparece en el código fuente
 * - line: número de línea (comienza en 1)
 * - column: número de columna (comienza en 1)
 * 
 * El lexer genera una secuencia de tokens que el parser consume para realizar
 * el análisis sintáctico. Cada token incluye información de posición que es
 * crucial para reportar errores con precisión.
 */
struct Token {
    /**
     * @brief Clasificación del token según TokenType
     * @details
     * Indica la categoría léxica del token: palabra reservada, identificador,
     * literal, operador, delimitador, o token especial (EOF/UNKNOWN).
     * El parser usa esta información para decidir las acciones de análisis.
     */
    TokenType type;
    
    /**
     * @brief Texto original del token tal como aparece en el código fuente
     * @details
     * Contiene el lexema exacto. Por ejemplo, si el token es un identificador
     * 'x', lexeme será "x". Si es un número '42', lexeme será "42".
     * Este valor es útil para debugging y para el análisis semántico.
     */
    std::string lexeme;
    
    /**
     * @brief Número de línea donde inicia el token (1-indexed)
     * @details
     * La primera línea del archivo es la línea 1. Se usa para reportar
     * errores léxicos, sintácticos y semánticos con la ubicación correcta.
     */
    int line;
    
    /**
     * @brief Número de columna donde inicia el token (1-indexed)
     * @details
     * La primera columna de cada línea es la columna 1. Combinada con line,
     * proporciona la posición exacta del token en el código fuente.
     */
    int column;

    /**
     * @brief Constructor completo para inicializar un token identificado.
     * @param type Clasificación del token.
     * @param lexeme Texto original extraído del código fuente.
     * @param line Línea donde fue detectado.
     * @param column Columna exacta donde inicia el lexema.
     */
    Token(TokenType type,
          const std::string& lexeme,
          int line,
          int column)
        : type(type)
        , lexeme(lexeme)
        , line(line)
        , column(column) {}

    /**
     * @brief Constructor por defecto.
     * @details Inicializa un token marcándolo como fin de archivo (EOF)
     * con valores en cero y cadena vacía por seguridad.
     */
    Token()
        : type(TokenType::END_OF_FILE)
        , lexeme("")
        , line(0)
        , column(0) {}

    /**
     * @brief Verifica si el token es una palabra reservada.
     * @return true si pertenece a una palabra clave del lenguaje.
     */
    bool isKeyword() const {
        return type == TokenType::KW_PROGRAM  ||
               type == TokenType::KW_VAR       ||
               type == TokenType::KW_INT      ||
               type == TokenType::KW_FLOAT    ||
               type == TokenType::KW_BEGIN    ||
               type == TokenType::KW_END      ||
               type == TokenType::KW_IF       ||
               type == TokenType::KW_ELSE     ||
               type == TokenType::KW_WHILE;
    }

    /**
     * @brief Verifica si el token es un literal numérico.
     * @return true si es una constante entera o de punto flotante.
     */
    bool isLiteral() const {
        return type == TokenType::LIT_INT ||
               type == TokenType::LIT_FLOAT;
    }

    /**
     * @brief Verifica si el token representa un operador.
     * @return true si es un operador aritmético, relacional o de asignación.
     */
    bool isOperator() const {
        return type == TokenType::OP_PLUS      ||
               type == TokenType::OP_MINUS     ||
               type == TokenType::OP_MULTIPLY  ||
               type == TokenType::OP_DIVIDE    ||
               type == TokenType::OP_ASSIGN    ||
               type == TokenType::OP_GREATER   ||
               type == TokenType::OP_LESS;
    }

    /**
     * @brief Verifica si el token es un signo delimitador.
     * @return true si es un paréntesis, llave, punto y coma o dos puntos.
     */
    bool isDelimiter() const {
        return type == TokenType::DELIM_SEMICOLON ||
               type == TokenType::DELIM_COLON    ||
               type == TokenType::DELIM_LPAREN   ||
               type == TokenType::DELIM_RPAREN   ||
               type == TokenType::DELIM_LBRACE   ||
               type == TokenType::DELIM_RBRACE;
    }

    /**
     * @brief Indica si el token representa un error léxico detectado.
     * @return true si el tipo está clasificado como UNKNOWN.
     */
    bool isError() const {
        return type == TokenType::UNKNOWN;
    }

    /**
     * @brief Indica si el token marca el final del archivo o flujo de entrada.
     * @return true si el tipo es END_OF_FILE.
     */
    bool isEOF() const {
        return type == TokenType::END_OF_FILE;
    }

    /**
     * @brief Retorna el símbolo del parser correspondiente a este token.
     * @return String con el nombre del terminal en la gramática LL(1).
     */
    std::string toParserSymbol() const {
        return tokenToParserSymbol(type);
    }

    /**
     * @brief Genera una representación en formato de texto legible del token.
     * @details Ideal para imprimir la lista de tokens generada por el lexer.
     * Formato: [línea:columna]    TIPO    'lexema'
     * @return std::string Cadena de caracteres formateada.
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << line << ":" << column << "]"
            << "\t" << tokenTypeToString(type)
            << "\t\t'" << lexeme << "'";
        return oss.str();
    }
};

#endif // TOKEN_H