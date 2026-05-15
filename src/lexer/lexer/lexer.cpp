/**
 * @file lexer.cpp
 * @brief Implementación del analizador léxico.
 *
 * @details
 * Implementa el autómata de estados finitos del Lexer.
 * Cada método privado representa un estado especializado
 * del autómata que construye un lexema y lo clasifica.
 *
 * Tabla de transiciones del autómata:
 * @code
 * ┌──────────────────┬────────────────────────────┐
 * │ Carácter actual  │ Acción                     │
 * ├──────────────────┼────────────────────────────┤
 * │ espacio/\t/\n/\r │ skipWhitespace()           │
 * │ letra o '_'      │ scanIdentifierOrKeyword()  │
 * │ dígito           │ scanNumber()               │
 * │ + - * / = > <    │ scanOperator()             │
 * │ ; ( ) { }        │ scanDelimiter()            │
 * │ cualquier otro   │ scanUnknown()              │
 * └──────────────────┴────────────────────────────┘
 * @endcode
 *
 * @author  Equipo Lexer
 * @version 1.0
 */

#include "lexer.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// =============================================================
//  Constructor
// =============================================================

/**
 * @brief Inicializa el Lexer con el código fuente dado.
 *
 * @details Establece el estado inicial del autómata:
 *  - pos_    = 0 → apunta al primer carácter
 *  - line_   = 1 → primera línea
 *  - column_ = 1 → primera columna
 *
 * @param source  Código fuente a analizar.
 */
Lexer::Lexer(const std::string& source)
    : source_(source)
    , pos_(0)
    , line_(1)
    , column_(1)
{}


// =============================================================
//  Análisis principal
// =============================================================

/**
 * @brief Ejecuta el análisis léxico completo sobre source_.
 *
 * @details
 * Bucle principal del autómata: itera sobre cada carácter
 * del código fuente y despacha al escáner correspondiente
 * según el tipo de carácter actual.
 *
 * Después del bucle agrega un token END_OF_FILE y retorna
 * el LexerResult con tokens, tabla de símbolos y errores.
 *
 * @return LexerResult con los tres productos del análisis.
 */
LexerResult Lexer::analyze() {

    // Limpiar resultados anteriores si se reutiliza el lexer
    tokens_.clear();
    symbolTable_.clear();
    errorHandler_.clear();

    // ─── Bucle principal del autómata ────────────────────────
    while (!isAtEnd()) {

        char c = currentChar();

        // 1. Ignorar whitespace (espacios, tabs, saltos de línea)
        if (Utils::isWhitespace(c)) {
            skipWhitespace();
            continue;
        }

        // 2. Identificador o palabra reservada
        //    Regla: letra(letra|dígito|'_')*
        if (Utils::isLetter(c)) {
            scanIdentifierOrKeyword();
            continue;
        }

        // 3. Número entero o real
        //    Regla: dígito+ ('.' dígito+)?
        if (Utils::isDigit(c)) {
            scanNumber();
            continue;
        }

        // 4. Operador: + - * / = > <
        if (Utils::isOperator(c)) {
            scanOperator();
            continue;
        }

        // 5. Delimitador: ; ( ) { }
        if (Utils::isDelimiter(c)) {
            scanDelimiter();
            continue;
        }

        // 6. Carácter no reconocido → error léxico
        scanUnknown();
    }

    // ─── Token de fin de archivo ─────────────────────────────
    addToken(TokenType::END_OF_FILE, "EOF", line_, column_);

    // ─── Empaquetar y retornar resultado ─────────────────────
    LexerResult result;
    result.tokens      = tokens_;
    result.symbolTable = symbolTable_;
    result.errorHandler = errorHandler_;
    return result;
}


// =============================================================
//  Acceso a resultados
// =============================================================

/**
 * @brief Retorna la lista de tokens generados.
 * @return Referencia constante al vector de tokens.
 */
const std::vector<Token>& Lexer::getTokens() const {
    return tokens_;
}

/**
 * @brief Retorna la tabla de símbolos construida.
 * @return Referencia constante a la SymbolTable.
 */
const SymbolTable& Lexer::getSymbolTable() const {
    return symbolTable_;
}

/**
 * @brief Retorna el manejador de errores con los errores detectados.
 * @return Referencia constante al ErrorHandler.
 */
const ErrorHandler& Lexer::getErrorHandler() const {
    return errorHandler_;
}


// =============================================================
//  Utilidades de impresión
// =============================================================

/**
 * @brief Imprime todos los tokens al stream dado.
 *
 * @details
 * Formato de cada fila:
 * @code
 *   [línea:col]   TIPO_TOKEN        'lexema'
 * @endcode
 *
 * Ejemplo de salida:
 * @code
 *   ──────────────────────────────────────────────────
 *          LISTA DE TOKENS
 *   ──────────────────────────────────────────────────
 *    #    Pos          Tipo              Lexema
 *   ──────────────────────────────────────────────────
 *    1   [1:1]        KW_INT            'int'
 *    2   [1:5]        IDENT             'suma'
 *    3   [1:10]       OP_ASSIGN         '='
 *   ──────────────────────────────────────────────────
 * @endcode
 *
 * @param out  Stream de salida.
 */
void Lexer::printTokens(std::ostream& out) const {
    const std::string separator(54, '-');

    out << "\n" << separator << "\n";
    out << "          LISTA DE TOKENS\n";
    out << separator << "\n";

    out << "  "
        << std::left  << std::setw(4)  << "#"
        << std::left  << std::setw(10) << "Pos"
        << std::left  << std::setw(20) << "Tipo"
        << std::left  << std::setw(16) << "Lexema"
        << "\n";
    out << separator << "\n";

    int idx = 1;
    for (const auto& token : tokens_) {
        // Construir posición [línea:columna]
        std::ostringstream pos;
        pos << "[" << token.line << ":" << token.column << "]";

        out << "  "
            << std::left  << std::setw(4)  << idx++
            << std::left  << std::setw(10) << pos.str()
            << std::left  << std::setw(20) << tokenTypeToString(token.type)
            << "'" << token.lexeme << "'"
            << "\n";
    }

    out << separator << "\n";
    out << "  Total: " << tokens_.size() << " token(s)\n";
    out << separator << "\n";
}


// =============================================================
//  Navegación del código fuente
// =============================================================

/**
 * @brief Retorna el carácter en la posición actual.
 *
 * @details Retorna '\0' como centinela cuando pos_ >= source_.size(),
 *          evitando accesos fuera de rango.
 *
 * @return Carácter actual o '\0'.
 */
char Lexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return source_[pos_];
}

/**
 * @brief Mira el siguiente carácter sin avanzar.
 *
 * @details Implementa lookahead de 1 carácter. Usado principalmente
 *          en scanNumber() para distinguir entre entero y float,
 *          y para detectar números mal formados como "3x".
 *
 * @return Carácter siguiente o '\0' si está en el último carácter.
 */
char Lexer::peekChar() const {
    if (pos_ + 1 >= static_cast<int>(source_.size())) return '\0';
    return source_[pos_ + 1];
}

/**
 * @brief Avanza la posición y actualiza línea/columna.
 *
 * @details
 * Reglas de actualización:
 *  - Si currentChar() == '\n': line_++, column_ = 1
 *  - Cualquier otro carácter : column_++
 *
 * Siempre incrementa pos_ en 1.
 */
void Lexer::advance() {
    if (!isAtEnd()) {
        if (source_[pos_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        pos_++;
    }
}

/**
 * @brief Indica si se llegó al final del código fuente.
 *
 * @return true  si pos_ >= source_.size()
 * @return false si aún hay caracteres.
 */
bool Lexer::isAtEnd() const {
    return pos_ >= static_cast<int>(source_.size());
}

/**
 * @brief Consume todos los caracteres de espacio en blanco.
 *
 * @details Avanza la posición mientras el carácter actual sea
 *          espacio (' '), tabulador ('\t'), retorno de carro ('\r')
 *          o nueva línea ('\n'). advance() actualiza line_ y column_.
 */
void Lexer::skipWhitespace() {
    while (!isAtEnd() && Utils::isWhitespace(currentChar())) {
        advance();
    }
}


// =============================================================
//  Escáneres especializados
// =============================================================

/**
 * @brief Escanea un identificador o palabra reservada.
 *
 * @details
 * Aplica la regla gramatical:
 * @code
 *   identificador ::= letra ( letra | dígito | '_' )*
 * @endcode
 *
 * Pasos:
 *  1. Registrar columna de inicio (para el token).
 *  2. Acumular caracteres mientras sean alfanuméricos o '_'.
 *  3. Consultar si el lexema es palabra reservada:
 *     - Sí → clasificar con el TokenType de la keyword.
 *     - No → clasificar como IDENT y agregar a symbolTable_.
 *  4. Agregar el token a tokens_.
 *
 * @note Los identificadores se distinguen de las keywords
 *       mediante la función Utils::isKeyword() que compara
 *       contra las 5 palabras reservadas del lenguaje.
 */
void Lexer::scanIdentifierOrKeyword() {
    int startColumn = column_;
    std::string lexeme;

    // Acumular mientras sea letra, dígito o '_'
    while (!isAtEnd() && Utils::isAlphaNumeric(currentChar())) {
        lexeme += currentChar();
        advance();
    }

    // Determinar si es keyword o identificador
    if (Utils::isKeyword(lexeme)) {
        TokenType kwType = Utils::getKeywordType(lexeme);
        addToken(kwType, lexeme, line_, startColumn);
    } else {
        addToken(TokenType::IDENT, lexeme, line_, startColumn);

        // Registrar en tabla de símbolos (sin duplicados)
        symbolTable_.add(lexeme, TokenType::IDENT, line_, startColumn);
    }
}

/**
 * @brief Escanea un número entero o real positivo.
 *
 * @details
 * Aplica las reglas gramaticales:
 * @code
 *   entero ::= dígito+
 *   real   ::= dígito+ '.' dígito+
 * @endcode
 *
 * Árbol de decisión:
 * @code
 *   Acumular dígitos (parte entera)
 *       │
 *       ├── ¿currentChar() == '.'?
 *       │       │
 *       │       ├── ¿peekChar() es dígito?
 *       │       │       └── Acumular '.' + dígitos → LIT_FLOAT ✓
 *       │       │
 *       │       └── ¿peekChar() NO es dígito?
 *       │               └── INCOMPLETE_FLOAT (error) ✗
 *       │
 *       └── ¿currentChar() es letra?
 *               ├── Sí  → MALFORMED_NUMBER (error) ✗  ej: 3x
 *               └── No  → LIT_INT ✓
 * @endcode
 */
void Lexer::scanNumber() {
    int startColumn = column_;
    std::string lexeme;

    // ── Acumular parte entera ─────────────────────────────────
    while (!isAtEnd() && Utils::isDigit(currentChar())) {
        lexeme += currentChar();
        advance();
    }

    // ── ¿Es número real? (tiene punto decimal) ───────────────
    if (!isAtEnd() && Utils::isDot(currentChar())) {

        char next = peekChar();

        // Caso válido: dígito después del punto → LIT_FLOAT
        if (Utils::isDigit(next)) {
            lexeme += currentChar(); // agregar '.'
            advance();

            // Acumular parte decimal
            while (!isAtEnd() && Utils::isDigit(currentChar())) {
                lexeme += currentChar();
                advance();
            }

            // Verificar que no siga una letra después del float
            if (!isAtEnd() && Utils::isLetter(currentChar())) {
                // Acumular el resto para mostrar el lexema completo
                while (!isAtEnd() && Utils::isAlphaNumeric(currentChar())) {
                    lexeme += currentChar();
                    advance();
                }
                errorHandler_.addError(
                    ErrorType::MALFORMED_NUMBER, lexeme, line_, startColumn
                );
                addToken(TokenType::UNKNOWN, lexeme, line_, startColumn);
                return;
            }

            addToken(TokenType::LIT_FLOAT, lexeme, line_, startColumn);
            return;
        }

        // Caso inválido: punto sin dígitos decimales → INCOMPLETE_FLOAT
        lexeme += currentChar(); // agregar '.'
        advance();
        errorHandler_.addError(
            ErrorType::INCOMPLETE_FLOAT, lexeme, line_, startColumn
        );
        addToken(TokenType::UNKNOWN, lexeme, line_, startColumn);
        return;
    }

    // ── ¿Número mal formado? (letra pegada al número) ────────
    //    Ej: 3x → MALFORMED_NUMBER
    if (!isAtEnd() && Utils::isLetter(currentChar())) {
        // Acumular el resto para mostrar el lexema completo en el error
        while (!isAtEnd() && Utils::isAlphaNumeric(currentChar())) {
            lexeme += currentChar();
            advance();
        }
        errorHandler_.addError(
            ErrorType::MALFORMED_NUMBER, lexeme, line_, startColumn
        );
        addToken(TokenType::UNKNOWN, lexeme, line_, startColumn);
        return;
    }

    // ── Número entero válido ──────────────────────────────────
    addToken(TokenType::LIT_INT, lexeme, line_, startColumn);
}

/**
 * @brief Escanea un operador de un solo carácter.
 *
 * @details
 * Operadores reconocidos: + - * / = > <
 *
 * Consulta Utils::getOperatorType(c) para obtener el TokenType
 * correcto, agrega el token y avanza la posición.
 */
void Lexer::scanOperator() {
    int startColumn = column_;
    char c = currentChar();
    std::string lexeme(1, c);

    TokenType type = Utils::getOperatorType(c);
    advance();

    addToken(type, lexeme, line_, startColumn);
}

/**
 * @brief Escanea un delimitador de un solo carácter.
 *
 * @details
 * Delimitadores reconocidos: ; ( ) { }
 *
 * Consulta Utils::getDelimiterType(c) para obtener el TokenType
 * correcto, agrega el token y avanza la posición.
 */
void Lexer::scanDelimiter() {
    int startColumn = column_;
    char c = currentChar();
    std::string lexeme(1, c);

    TokenType type = Utils::getDelimiterType(c);
    advance();

    addToken(type, lexeme, line_, startColumn);
}

/**
 * @brief Registra un carácter no reconocido como error léxico.
 *
 * @details
 * Cuando ningún escáner puede manejar el carácter actual,
 * este método lo captura, genera un token UNKNOWN y delega
 * el error al errorHandler_ con tipo INVALID_CHARACTER.
 *
 * El análisis continúa después del error (no se detiene).
 */
void Lexer::scanUnknown() {
    int startColumn = column_;
    char c = currentChar();
    std::string lexeme(1, c);
    advance();

    errorHandler_.addError(
        ErrorType::INVALID_CHARACTER, lexeme, line_, startColumn
    );
    addToken(TokenType::UNKNOWN, lexeme, line_, startColumn);
}


// =============================================================
//  Helpers internos
// =============================================================

/**
 * @brief Agrega un token a la lista interna de tokens.
 *
 * @details Método centralizado para la creación de tokens.
 *          Construye el Token con los 4 campos y lo añade
 *          al vector tokens_ usando emplace_back para eficiencia.
 *
 * @param type    Tipo del token (TokenType).
 * @param lexeme  Lexema del token.
 * @param line    Línea donde inicia el token.
 * @param column  Columna donde inicia el token.
 */
void Lexer::addToken(TokenType type,
                      const std::string& lexeme,
                      int line,
                      int column)
{
    tokens_.emplace_back(type, lexeme, line, column);
}