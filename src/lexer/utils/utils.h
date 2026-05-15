#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "../token/token_type.h"

/**
 * @file utils.h
 * @brief Declaración de funciones auxiliares para el analizador léxico.
 * @details Módulo de utilidades que proporciona funciones puras (sin estado) para 
 * la clasificación de caracteres, manipulación de cadenas de texto y búsqueda 
 * de palabras reservadas. Al ser funciones puras, no modifican el estado del lexer.
 */
namespace Utils {

    // ---------------------------------------------------------
    //  Clasificación de caracteres
    // ---------------------------------------------------------

    /**
     * @brief Verifica si un carácter es una letra o un guion bajo.
     * @param c Carácter a evaluar.
     * @return true si es letra (a-z, A-Z) o guion bajo (_).
     */
    bool isLetter(char c);

    /**
     * @brief Verifica si un carácter es un dígito numérico.
     * @param c Carácter a evaluar.
     * @return true si es dígito (0-9).
     */
    bool isDigit(char c);

    /**
     * @brief Verifica si un carácter es alfanumérico.
     * @details Útil para validar el cuerpo de los identificadores.
     * @param c Carácter a evaluar.
     * @return true si es letra, dígito o guion bajo.
     */
    bool isAlphaNumeric(char c);

    /**
     * @brief Verifica si un carácter es un espacio en blanco.
     * @param c Carácter a evaluar.
     * @return true si es espacio, tabulador, retorno de carro (CR) o nueva línea.
     */
    bool isWhitespace(char c);

    /**
     * @brief Verifica si un carácter corresponde a un operador reconocido.
     * @param c Carácter a evaluar.
     * @return true si es +, -, *, /, =, > o <.
     */
    bool isOperator(char c);

    /**
     * @brief Verifica si un carácter es un delimitador estructural.
     * @param c Carácter a evaluar.
     * @return true si es ;, (, ), { o }.
     */
    bool isDelimiter(char c);

    /**
     * @brief Verifica si un carácter es el punto decimal.
     * @param c Carácter a evaluar.
     * @return true si es el carácter '.'.
     */
    bool isDot(char c);

    // ---------------------------------------------------------
    //  Manipulación de strings
    // ---------------------------------------------------------

    /**
     * @brief Convierte una cadena de texto a minúsculas.
     * @details Facilita las comparaciones insensibles a mayúsculas/minúsculas.
     * @param str Cadena original.
     * @return std::string Nueva cadena en minúsculas.
     */
    std::string toLower(const std::string& str);

    /**
     * @brief Convierte una cadena de texto a mayúsculas.
     * @param str Cadena original.
     * @return std::string Nueva cadena en mayúsculas.
     */
    std::string toUpper(const std::string& str);

    /**
     * @brief Elimina los espacios en blanco al inicio y al final de una cadena.
     * @param str Cadena original.
     * @return std::string Cadena sin espacios en los extremos.
     */
    std::string trim(const std::string& str);

    /**
     * @brief Verifica si una cadena está vacía o compuesta solo por espacios.
     * @param str Cadena a evaluar.
     * @return true si el string está en blanco.
     */
    bool isBlank(const std::string& str);

    // ---------------------------------------------------------
    //  Lookup de palabras reservadas
    // ---------------------------------------------------------

    /**
     * @brief Determina si un lexema corresponde a una palabra reservada.
     * @param lexeme El texto a evaluar.
     * @return true si es una palabra reservada válida del lenguaje.
     */
    bool isKeyword(const std::string& lexeme);

    /**
     * @brief Obtiene el tipo de token para un lexema que es palabra reservada.
     * @param lexeme El texto de la palabra reservada.
     * @return TokenType El token correspondiente, o UNKNOWN si no es palabra reservada.
     */
    TokenType getKeywordType(const std::string& lexeme);

    // ---------------------------------------------------------
    //  Lookup de operadores y delimitadores (char → TokenType)
    // ---------------------------------------------------------

    /**
     * @brief Obtiene el tipo de token correspondiente a un carácter operador.
     * @param c Carácter del operador.
     * @return TokenType El token correspondiente, o UNKNOWN si no es operador.
     */
    TokenType getOperatorType(char c);

    /**
     * @brief Obtiene el tipo de token correspondiente a un carácter delimitador.
     * @param c Carácter del delimitador.
     * @return TokenType El token correspondiente, o UNKNOWN si no es delimitador.
     */
    TokenType getDelimiterType(char c);

    // ---------------------------------------------------------
    //  Validación de lexemas numéricos
    // ---------------------------------------------------------

    /**
     * @brief Valida si una cadena tiene el formato de un entero positivo.
     * @param str Cadena a evaluar.
     * @return true si el string contiene únicamente dígitos.
     */
    bool isValidInteger(const std::string& str);

    /**
     * @brief Valida si una cadena tiene el formato de un real positivo válido.
     * @param str Cadena a evaluar.
     * @return true si el string cumple con el formato dígitos.dígitos.
     */
    bool isValidFloat(const std::string& str);

} // namespace Utils

#endif // UTILS_H