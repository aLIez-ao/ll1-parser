#include "utils.h"
#include <algorithm>
#include <cctype>

/**
 * @file utils.cpp
 * @brief Implementación de funciones auxiliares para el analizador léxico.
 * * @details Este módulo proporciona funciones puras (sin estado) para 
 * clasificar caracteres, manipular cadenas de texto y realizar búsquedas 
 * rápidas de palabras reservadas, operadores y delimitadores.
 */

namespace Utils {

    // =============================================================
    //  Clasificación de caracteres
    // =============================================================

    /**
     * @brief Verifica si un carácter es una letra o un guion bajo.
     * * @details Se incluye el guion bajo ('_') porque es válido como 
     * carácter inicial o intermedio en los identificadores del lenguaje.
     * * @param c Carácter a evaluar.
     * @return true si es letra (a-z, A-Z) o guion bajo, false en caso contrario.
     */
    bool isLetter(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
    }

    /**
     * @brief Verifica si un carácter es un dígito numérico.
     * * @param c Carácter a evaluar.
     * @return true si es un dígito (0-9), false en caso contrario.
     */
    bool isDigit(char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    }

    /**
     * @brief Verifica si un carácter es alfanumérico (letra, dígito o guion bajo).
     * * @details Útil para validar el cuerpo de un identificador después de 
     * haber validado que su primer carácter es una letra.
     * * @param c Carácter a evaluar.
     * @return true si es alfanumérico, false en caso contrario.
     */
    bool isAlphaNumeric(char c) {
        return isLetter(c) || isDigit(c);
    }

    /**
     * @brief Verifica si un carácter es un espacio en blanco.
     * * @param c Carácter a evaluar.
     * @return true si es espacio, tabulador, retorno de carro o nueva línea.
     */
    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    /**
     * @brief Verifica si un carácter corresponde a un operador matemático o relacional.
     * * @param c Carácter a evaluar.
     * @return true si es +, -, *, /, =, >, <.
     */
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' ||
               c == '/' || c == '=' || c == '>' || c == '<';
    }

    /**
     * @brief Verifica si un carácter es un delimitador estructural.
     * * @param c Carácter a evaluar.
     * @return true si es ;, (, ), {, }.
     */
    bool isDelimiter(char c) {
        return c == ';' || c == '(' || c == ')' ||
               c == '{' || c == '}';
    }

    /**
     * @brief Verifica si un carácter es un punto decimal.
     * * @param c Carácter a evaluar.
     * @return true si es un punto (.), false en caso contrario.
     */
    bool isDot(char c) {
        return c == '.';
    }


    // =============================================================
    //  Manipulación de strings
    // =============================================================

    /**
     * @brief Convierte una cadena de texto a minúsculas.
     * * @details Útil para hacer que las palabras reservadas sean insensibles 
     * a mayúsculas/minúsculas (case-insensitive) si el lenguaje lo requiere.
     * * @param str Cadena original.
     * @return std::string Nueva cadena en minúsculas.
     */
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    /**
     * @brief Convierte una cadena de texto a mayúsculas.
     * * @param str Cadena original.
     * @return std::string Nueva cadena en mayúsculas.
     */
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    /**
     * @brief Elimina los espacios en blanco al inicio y al final de una cadena.
     * * @param str Cadena original.
     * @return std::string Cadena sin espacios en los extremos.
     */
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";

        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    /**
     * @brief Verifica si una cadena está vacía o compuesta solo por espacios.
     * * @param str Cadena a evaluar.
     * @return true si está en blanco, false si contiene caracteres visibles.
     */
    bool isBlank(const std::string& str) {
        return trim(str).empty();
    }


    // =============================================================
    //  Lookup de palabras reservadas
    // =============================================================

    /**
     * @brief Determina si un lexema corresponde a una palabra reservada del lenguaje.
     * * @param lexeme El texto a evaluar.
     * @return true si es una palabra reservada válida.
     */
    bool isKeyword(const std::string& lexeme) {
        std::string lower = toLower(lexeme);
        return lower == "program" ||
               lower == "var"    ||
               lower == "int"    ||
               lower == "float"  ||
               lower == "begin"  ||
               lower == "end"    ||
               lower == "if"     ||
               lower == "else"   ||
               lower == "while";
    }

    /**
     * @brief Obtiene el tipo de token específico para una palabra reservada.
     * * @param lexeme El texto de la palabra reservada.
     * @return TokenType El token correspondiente (ej. KW_INT), o UNKNOWN si no coincide.
     */
    TokenType getKeywordType(const std::string& lexeme) {
        std::string lower = toLower(lexeme);

        if (lower == "program") return TokenType::KW_PROGRAM;
        if (lower == "var")     return TokenType::KW_VAR;
        if (lower == "int")     return TokenType::KW_INT;
        if (lower == "float")   return TokenType::KW_FLOAT;
        if (lower == "begin")   return TokenType::KW_BEGIN;
        if (lower == "end")     return TokenType::KW_END;
        if (lower == "if")      return TokenType::KW_IF;
        if (lower == "else")    return TokenType::KW_ELSE;
        if (lower == "while")   return TokenType::KW_WHILE;

        return TokenType::UNKNOWN;
    }


    // =============================================================
    //  Lookup de operadores y delimitadores
    // =============================================================

    /**
     * @brief Obtiene el tipo de token correspondiente a un operador de un solo carácter.
     * * @param c Carácter del operador.
     * @return TokenType El token correspondiente (ej. OP_PLUS), o UNKNOWN si no es operador.
     */
    TokenType getOperatorType(char c) {
        switch (c) {
            case '+': return TokenType::OP_PLUS;
            case '-': return TokenType::OP_MINUS;
            case '*': return TokenType::OP_MULTIPLY;
            case '/': return TokenType::OP_DIVIDE;
            case '=': return TokenType::OP_ASSIGN;
            case '>': return TokenType::OP_GREATER;
            case '<': return TokenType::OP_LESS;
            default:  return TokenType::UNKNOWN;
        }
    }

    /**
     * @brief Obtiene el tipo de token correspondiente a un delimitador.
     * * @param c Carácter del delimitador.
     * @return TokenType El token correspondiente (ej. DELIM_SEMICOLON), o UNKNOWN.
     */
    TokenType getDelimiterType(char c) {
        switch (c) {
            case ';': return TokenType::DELIM_SEMICOLON;
            case ':': return TokenType::DELIM_COLON;
            case '(': return TokenType::DELIM_LPAREN;
            case ')': return TokenType::DELIM_RPAREN;
            case '{': return TokenType::DELIM_LBRACE;
            case '}': return TokenType::DELIM_RBRACE;
            default:  return TokenType::UNKNOWN;
        }
    }


    // =============================================================
    //  Validación de lexemas numéricos
    // =============================================================

    /**
     * @brief Valida si una cadena completa tiene el formato correcto de un entero.
     * * @param str Cadena a evaluar.
     * @return true si todos los caracteres son dígitos.
     */
    bool isValidInteger(const std::string& str) {
        if (str.empty()) return false;
        for (char c : str) {
            if (!isDigit(c)) return false;
        }
        return true;
    }

    /**
     * @brief Valida si una cadena completa tiene el formato correcto de un número de punto flotante.
     * * @details Un formato válido debe contener exactamente un punto decimal, 
     * rodeado de al menos un dígito a cada lado (ej. 3.14, 2.0).
     * * @param str Cadena a evaluar.
     * @return true si cumple con la estructura de un float.
     */
    bool isValidFloat(const std::string& str) {
        if (str.empty()) return false;

        size_t dotPos = str.find('.');

        if (dotPos == std::string::npos) return false;
        if (str.find('.', dotPos + 1) != std::string::npos) return false;
        if (dotPos == 0) return false;
        if (dotPos == str.size() - 1) return false;

        for (size_t i = 0; i < str.size(); i++) {
            if (i == dotPos) continue;
            if (!isDigit(str[i])) return false;
        }

        return true;
    }

} // namespace Utils