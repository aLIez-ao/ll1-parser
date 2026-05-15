#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "../token/token_type.h"
#include "../token/token.h"
#include "../symbol_table/symbol_table.h"
#include "../error_handler/error_handler.h"
#include "../utils/utils.h"

/**
 * @file lexer.h
 * @brief Declaración de la clase Lexer (Analizador Léxico).
 *
 * @details
 * El Lexer es el módulo central del analizador léxico.
 * Recibe una cadena con código fuente y la recorre carácter
 * por carácter construyendo lexemas, clasificándolos en tokens,
 * detectando errores léxicos y poblando la tabla de símbolos.
 *
 * Flujo general de análisis:
 * @code
 *   Código fuente (string)
 *         │
 *         ▼
 *    ┌──────────┐
 *    │  Lexer   │ ──► Lista de Tokens
 *    │ analyze()│ ──► Tabla de Símbolos
 *    │          │ ──► Reporte de Errores
 *    └──────────┘
 * @endcode
 *
 * Reconoce los siguientes elementos del lenguaje:
 *  - Palabras reservadas : int, float, if, else, while
 *  - Identificadores     : letra(letra|dígito)*
 *  - Enteros positivos   : dígito+
 *  - Reales positivos    : dígito+.dígito+
 *  - Operadores          : + - * / = > <
 *  - Delimitadores       : ; ( ) { }
 *
 * @author  Equipo Lexer
 * @version 1.0
 */


/**
 * @struct LexerResult
 * @brief Resultado completo devuelto por el análisis léxico.
 *
 * @details
 * Agrupa los tres productos del análisis en una sola estructura
 * para facilitar el retorno y el acceso desde main().
 */
struct LexerResult {

    /**
     * @brief Lista ordenada de todos los tokens reconocidos.
     * @details Incluye tokens válidos e UNKNOWN (errores léxicos).
     *          El orden refleja la aparición en el código fuente.
     */
    std::vector<Token> tokens;

    /**
     * @brief Tabla de símbolos con los identificadores únicos.
     * @details Solo contiene entradas de tipo IDENT, sin duplicados.
     */
    SymbolTable symbolTable;

    /**
     * @brief Manejador de errores con todos los errores detectados.
     * @details Permite consultar, filtrar e imprimir errores léxicos.
     */
    ErrorHandler errorHandler;
};


/**
 * @class Lexer
 * @brief Analizador léxico que transforma código fuente en tokens.
 *
 * @details
 * Implementa un autómata de estados finitos determinista (AFD)
 * que recorre el código fuente carácter por carácter y toma
 * decisiones basadas en el carácter actual y el contexto acumulado.
 *
 * Estados internos del autómata:
 * @code
 *   INICIO
 *     ├── letra/_ ──────────► IDENT_OR_KEYWORD
 *     ├── dígito ───────────► INTEGER
 *     │                         └── '.' ──► FLOAT
 *     ├── operador ─────────► OPERATOR (token inmediato)
 *     ├── delimitador ──────► DELIMITER (token inmediato)
 *     ├── espacio/\n/\t ────► (ignorar, avanzar)
 *     └── otro ─────────────► ERROR LÉXICO
 * @endcode
 *
 * Uso básico:
 * @code
 *   std::string source = "int suma = 10;";
 *   Lexer lexer(source);
 *   LexerResult result = lexer.analyze();
 *
 *   // Imprimir tokens
 *   for (const Token& t : result.tokens) {
 *       std::cout << t.toString() << "\n";
 *   }
 *
 *   // Imprimir tabla de símbolos
 *   result.symbolTable.print(std::cout);
 *
 *   // Imprimir errores
 *   result.errorHandler.printReport(std::cout);
 * @endcode
 */
class Lexer {
public:

    // =========================================================
    //  Constructor / Destructor
    // =========================================================

    /**
     * @brief Constructor principal del Lexer.
     *
     * @details
     * Recibe el código fuente como string e inicializa
     * los campos internos a sus valores iniciales:
     *  - pos_    = 0  (inicio del string)
     *  - line_   = 1  (primera línea)
     *  - column_ = 1  (primera columna)
     *
     * @param source  String con el código fuente a analizar.
     *                Puede contener múltiples líneas.
     */
    explicit Lexer(const std::string& source);

    /**
     * @brief Destructor por defecto.
     */
    ~Lexer() = default;

    // =========================================================
    //  Análisis principal
    // =========================================================

    /**
     * @brief Ejecuta el análisis léxico completo del código fuente.
     *
     * @details
     * Recorre el código fuente de izquierda a derecha construyendo
     * tokens hasta llegar al final de la cadena.
     *
     * Por cada carácter aplica el siguiente algoritmo:
     * @code
     *   1. Ignorar whitespace (actualizar línea/columna)
     *   2. Si letra o '_'  → scanIdentifierOrKeyword()
     *   3. Si dígito       → scanNumber()
     *   4. Si operador     → scanOperator()
     *   5. Si delimitador  → scanDelimiter()
     *   6. Otro            → registrar error léxico (UNKNOWN)
     * @endcode
     *
     * Al finalizar agrega automáticamente un token END_OF_FILE.
     *
     * @return LexerResult con tokens, tabla de símbolos y errores.
     */
    LexerResult analyze();

    // =========================================================
    //  Acceso a resultados parciales
    // =========================================================

    /**
     * @brief Retorna la lista de tokens generados hasta el momento.
     *
     * @details Solo tiene sentido llamarlo después de analyze().
     *
     * @return Referencia constante al vector de tokens.
     */
    const std::vector<Token>& getTokens() const;

    /**
     * @brief Retorna la tabla de símbolos construida.
     *
     * @details Solo tiene sentido llamarlo después de analyze().
     *
     * @return Referencia constante a la SymbolTable.
     */
    const SymbolTable& getSymbolTable() const;

    /**
     * @brief Retorna el manejador de errores con los errores detectados.
     *
     * @details Solo tiene sentido llamarlo después de analyze().
     *
     * @return Referencia constante al ErrorHandler.
     */
    const ErrorHandler& getErrorHandler() const;

    // =========================================================
    //  Utilidades de impresión
    // =========================================================

    /**
     * @brief Imprime la lista de tokens al stream dado.
     *
     * @details
     * Formato por token:
     * @code
     *  [línea:col]   TIPO_TOKEN      'lexema'
     * @endcode
     *
     * @param out  Stream de salida (std::cout, archivo, etc.)
     */
    void printTokens(std::ostream& out) const;

private:

    // =========================================================
    //  Estado interno del autómata
    // =========================================================

    /** @brief Código fuente completo a analizar. */
    std::string source_;

    /** @brief Posición actual en source_ (índice del carácter). */
    int pos_;

    /** @brief Línea actual del análisis (inicia en 1). */
    int line_;

    /** @brief Columna actual del análisis (inicia en 1). */
    int column_;

    // =========================================================
    //  Productos del análisis
    // =========================================================

    /** @brief Lista acumulada de tokens reconocidos. */
    std::vector<Token> tokens_;

    /** @brief Tabla de símbolos con identificadores únicos. */
    SymbolTable symbolTable_;

    /** @brief Manejador de errores léxicos detectados. */
    ErrorHandler errorHandler_;

    // =========================================================
    //  Navegación del código fuente
    // =========================================================

    /**
     * @brief Retorna el carácter en la posición actual.
     *
     * @details Si pos_ >= source_.size() retorna '\0' como
     *          centinela de fin de cadena.
     *
     * @return Carácter actual o '\0' si se llegó al final.
     */
    char currentChar() const;

    /**
     * @brief Mira el siguiente carácter sin avanzar la posición.
     *
     * @details Útil para decisiones de lookahead de 1 carácter,
     *          por ejemplo distinguir entero de float.
     *
     * @return Carácter siguiente o '\0' si está al final.
     */
    char peekChar() const;

    /**
     * @brief Avanza la posición al siguiente carácter.
     *
     * @details Actualiza automáticamente line_ y column_:
     *  - Si el carácter actual es '\n': incrementa line_, resetea column_ a 1.
     *  - Cualquier otro carácter: incrementa column_.
     */
    void advance();

    /**
     * @brief Indica si se llegó al final del código fuente.
     *
     * @return true  si pos_ >= source_.size()
     * @return false si aún hay caracteres por analizar.
     */
    bool isAtEnd() const;

    /**
     * @brief Consume todos los caracteres de espacio en blanco.
     *
     * @details Avanza mientras el carácter actual sea espacio,
     *          tabulador, retorno de carro o nueva línea.
     *          Actualiza line_ y column_ a través de advance().
     */
    void skipWhitespace();

    // =========================================================
    //  Escáneres especializados
    // =========================================================

    /**
     * @brief Escanea un identificador o palabra reservada.
     *
     * @details
     * Aplica la regla:   letra ( letra | dígito | '_' )*
     *
     * Algoritmo:
     * @code
     *   1. Registrar columna de inicio
     *   2. Acumular caracteres mientras sean alfanuméricos o '_'
     *   3. Consultar Utils::isKeyword(lexema)
     *      ├── true  → token = getKeywordType(lexema)
     *      └── false → token = TokenType::IDENT
     *   4. Si es IDENT → agregar a symbolTable_
     *   5. Agregar token a tokens_
     * @endcode
     */
    void scanIdentifierOrKeyword();

    /**
     * @brief Escanea un número entero o real positivo.
     *
     * @details
     * Aplica las reglas:
     *  - Entero: dígito+
     *  - Real  : dígito+ '.' dígito+
     *
     * Algoritmo:
     * @code
     *   1. Acumular dígitos → parte entera
     *   2. ¿Siguiente char es '.'?
     *      ├── Sí: avanzar, acumular dígitos → parte decimal
     *      │    ├── ¿hay dígitos decimales? → LIT_FLOAT
     *      │    └── No hay dígitos tras '.' → INCOMPLETE_FLOAT (error)
     *      └── No: verificar que no siga letra
     *           ├── ¿sigue letra? → MALFORMED_NUMBER (error) ej: 3x
     *           └── No → LIT_INT
     * @endcode
     */
    void scanNumber();

    /**
     * @brief Escanea un operador de un solo carácter.
     *
     * @details
     * Operadores reconocidos: + - * / = > <
     * Consulta Utils::getOperatorType(c) y agrega el token.
     *
     * @note Si el carácter no es un operador válido se registra
     *       como UNKNOWN aunque esto no debería ocurrir dado
     *       que el dispatcher en analyze() ya verifica.
     */
    void scanOperator();

    /**
     * @brief Escanea un delimitador de un solo carácter.
     *
     * @details
     * Delimitadores reconocidos: ; ( ) { }
     * Consulta Utils::getDelimiterType(c) y agrega el token.
     */
    void scanDelimiter();

    /**
     * @brief Registra un carácter desconocido como error léxico.
     *
     * @details
     * Cuando analyze() encuentra un carácter que no corresponde
     * a ninguna categoría válida, llama a este método.
     *
     * Crea un token UNKNOWN y delega el error al errorHandler_
     * con tipo INVALID_CHARACTER.
     */
    void scanUnknown();

    // =========================================================
    //  Helpers internos
    // =========================================================

    /**
     * @brief Agrega un token a la lista de tokens.
     *
     * @details Método centralizado para insertar tokens,
     *          evitando repetir la construcción en cada escáner.
     *
     * @param type    Tipo del token.
     * @param lexeme  Lexema del token.
     * @param line    Línea donde inicia el token.
     * @param column  Columna donde inicia el token.
     */
    void addToken(TokenType type,
                  const std::string& lexeme,
                  int line,
                  int column);
};

#endif // LEXER_H