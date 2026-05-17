#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <string>

// =============================================================
// Token reconocidos por el lexer
// =============================================================
enum class TokenType {

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


// =============================================================
// Mapeo entre tokens del lexer y tokens del parser
// El parser usa estos nombres para consultar la tabla LL(1)
// =============================================================
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


// =============================================================
//  Convierte un TokenType a su representación en string
//  Útil para imprimir tablas y reportes
// =============================================================
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