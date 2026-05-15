#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include <ostream>

/**
 * @file error_handler.h
 * @brief Módulo responsable de registrar, clasificar y reportar todos los errores léxicos.
 * * @details
 * - El Lexer delega TODOS los errores a este módulo.
 * - ErrorHandler NO lanza excepciones; acumula errores para reportarlos todos al final del análisis.
 * - Esto permite continuar el análisis aunque haya errores, lo que es más útil para el programador.
 * * @par Uso típico:
 * @code
 * ErrorHandler eh;
 * eh.addError(ErrorType::INVALID_CHARACTER, "3x", 4, 16);
 * eh.printReport(std::cout);
 * @endcode
 */

/**
 * @enum ErrorType
 * @brief Clasifica el tipo de error léxico detectado.
 */
enum class ErrorType {
    INVALID_CHARACTER,  ///< Carácter que no pertenece al alfabeto del lenguaje (Ej: @, #, $, %, &, ~).
    MALFORMED_NUMBER,   ///< Número con formato incorrecto (Ej: 3x).
    INVALID_IDENTIFIER, ///< Identificador que inicia con dígito (Ej: 2suma, 9var).
    INCOMPLETE_FLOAT,   ///< Número real con punto pero sin parte decimal o entera (Ej: 3. o .5).
    UNEXPECTED_TOKEN,   ///< Carácter reconocido pero en contexto inválido (Ej: punto aislado '.').
    GENERIC_ERROR       ///< Error genérico no clasificado en las categorías anteriores.
};

/**
 * @struct LexicalError
 * @brief Representa un único error léxico con toda su información.
 */
struct LexicalError {
    ErrorType type;       ///< Clasificación del error.
    std::string lexeme;   ///< Lexema que causó el error.
    std::string message;  ///< Mensaje descriptivo para el usuario.
    int line;             ///< Línea donde ocurrió el error (desde 1).
    int column;           ///< Columna donde inicia el error (desde 1).

    /**
     * @brief Constructor de la estructura LexicalError.
     * * @param type Clasificación del error.
     * @param lexeme Lexema que causó el error.
     * @param message Mensaje descriptivo para el usuario.
     * @param line Línea donde ocurrió el error.
     * @param column Columna donde inicia el error.
     */
    LexicalError(ErrorType type,
                 const std::string& lexeme,
                 const std::string& message,
                 int line,
                 int column)
        : type(type)
        , lexeme(lexeme)
        , message(message)
        , line(line)
        , column(column)
    {}

    /**
     * @brief Convierte el error a un string legible.
     * @return std::string Cadena con el formato: [ERROR] Línea X, Col Y -> 'lexema' : mensaje
     */
    std::string toString() const;
};

/**
 * @class ErrorHandler
 * @brief Acumula y reporta errores léxicos durante el análisis.
 */
class ErrorHandler {
public:
    /**
     * @brief Constructor por defecto.
     */
    ErrorHandler();

    /**
     * @brief Destructor por defecto.
     */
    ~ErrorHandler() = default;

    /**
     * @brief Registra un nuevo error léxico. Genera automáticamente el mensaje descriptivo según el tipo.
     * * @param type Clasificación del error (ErrorType).
     * @param lexeme El lexema que causó el error.
     * @param line Línea donde fue detectado.
     * @param column Columna donde inicia el lexema.
     */
    void addError(ErrorType type,
                  const std::string& lexeme,
                  int line,
                  int column);

    /**
     * @brief Permite registrar un error con un mensaje específico y personalizado.
     * * @param type Clasificación del error.
     * @param lexeme El lexema que causó el error.
     * @param message Mensaje personalizado para el usuario.
     * @param line Línea donde fue detectado.
     * @param column Columna donde inicia el lexema.
     */
    void addError(ErrorType type,
                  const std::string& lexeme,
                  const std::string& message,
                  int line,
                  int column);

    /**
     * @brief Verifica si existen errores registrados.
     * @return true si se registró al menos un error, false en caso contrario.
     */
    bool hasErrors() const;

    /**
     * @brief Obtiene la cantidad total de errores.
     * @return int Número de errores registrados.
     */
    int getErrorCount() const;

    /**
     * @brief Obtiene la lista completa de errores registrados.
     * @return const std::vector<LexicalError>& Referencia constante a la lista de errores.
     */
    const std::vector<LexicalError>& getErrors() const;

    /**
     * @brief Filtra y retorna los errores que ocurrieron en una línea específica.
     * * @param line Número de línea a filtrar.
     * @return std::vector<LexicalError> Lista de errores encontrados en esa línea.
     */
    std::vector<LexicalError> getErrorsByLine(int line) const;

    /**
     * @brief Elimina todos los errores registrados.
     * * @details Útil para reutilizar el manejador de errores en múltiples análisis sin tener que instanciar uno nuevo.
     */
    void clear();

    /**
     * @brief Imprime el reporte completo de errores.
     * * @details Muestra un encabezado, detalla cada error individualmente y finaliza con un resumen.
     * * @param out Stream de salida (ej: std::cout, std::cerr).
     */
    void printReport(std::ostream& out) const;

    /**
     * @brief Imprime únicamente el resumen total de errores.
     * * @param out Stream de salida.
     */
    void printSummary(std::ostream& out) const;

private:
    std::vector<LexicalError> errors_; ///< Lista interna de errores acumulados.

    /**
     * @brief Genera el mensaje descriptivo según el tipo de error.
     * * @param type Tipo de error.
     * @param lexeme Lexema que causó el error.
     * @return std::string Mensaje formateado para el usuario.
     */
    std::string buildMessage(ErrorType type,
                             const std::string& lexeme) const;

    /**
     * @brief Convierte un valor de ErrorType a su representación en string.
     * * @param type Tipo de error.
     * @return std::string Nombre del tipo de error.
     */
    std::string errorTypeToString(ErrorType type) const;
};

#endif // ERROR_HANDLER_H