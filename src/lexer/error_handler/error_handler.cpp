#include "error_handler.h"
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * @file error_handler.cpp
 * @brief Implementación de los métodos para el manejo de errores léxicos.
 */

// =============================================================
//  LexicalError — Implementación
// =============================================================

/**
 * @brief Convierte el error a una línea legible para el reporte.
 * * @details 
 * Formato de salida:
 * [ERROR] Línea X, Col Y  →  'lexema'  (TIPO_ERROR): mensaje
 * * Ejemplo:
 * [ERROR] Línea 4, Col 16  →  '3x'  (MALFORMED_NUMBER): Número con sufijo inválido '3x'. ¿Quiso escribir '3'?
 * * @return std::string String formateado con la información del error.
 */
std::string LexicalError::toString() const {
    std::ostringstream oss;
    oss << "[ERROR] "
        << "Línea " << std::setw(2) << line
        << ", Col "  << std::setw(2) << column
        << "  →  '" << lexeme << "'"
        << " : " << message;
    return oss.str();
}


// =============================================================
//  ErrorHandler — Constructor
// =============================================================

/**
 * @brief Constructor por defecto. Inicializa la lista de errores vacía.
 */
ErrorHandler::ErrorHandler() {
    errors_.clear();
}


// =============================================================
//  ErrorHandler — Métodos públicos
// =============================================================

/**
 * @brief Registra un error generando automáticamente el mensaje.
 * * @details Llama internamente a buildMessage() para construir un mensaje descriptivo 
 * según el tipo de error y el lexema.
 * * @param type   Clasificación del error (ErrorType).
 * @param lexeme Lexema que causó el error.
 * @param line   Línea donde fue detectado (desde 1).
 * @param column Columna donde inicia el lexema (desde 1).
 */
void ErrorHandler::addError(ErrorType type,
                             const std::string& lexeme,
                             int line,
                             int column)
{
    std::string msg = buildMessage(type, lexeme);
    errors_.emplace_back(type, lexeme, msg, line, column);
}

/**
 * @brief Registra un error con un mensaje personalizado.
 * * @details Útil cuando el Lexer tiene información de contexto adicional
 * que no puede inferirse solo del tipo y el lexema.
 * * @param type    Clasificación del error.
 * @param lexeme  Lexema que causó el error.
 * @param message Mensaje descriptivo personalizado.
 * @param line    Línea donde fue detectado.
 * @param column  Columna donde inicia el lexema.
 */
void ErrorHandler::addError(ErrorType type,
                             const std::string& lexeme,
                             const std::string& message,
                             int line,
                             int column)
{
    errors_.emplace_back(type, lexeme, message, line, column);
}

/**
 * @brief Indica si se registró al menos un error.
 * * @details Uso típico en main():
 * @code
 * if (errorHandler.hasErrors()) {
 * errorHandler.printReport(std::cerr);
 * }
 * @endcode
 * * @return true si hay errores, false si el análisis fue limpio.
 */
bool ErrorHandler::hasErrors() const {
    return !errors_.empty();
}

/**
 * @brief Retorna el número total de errores registrados.
 * * @return int Conteo de errores acumulados.
 */
int ErrorHandler::getErrorCount() const {
    return static_cast<int>(errors_.size());
}

/**
 * @brief Retorna la lista completa de errores (solo lectura).
 * * @details Permite a módulos externos (tests, reportes) iterar sobre los errores sin modificarlos.
 * * @return const std::vector<LexicalError>& Referencia constante al vector de errores.
 */
const std::vector<LexicalError>& ErrorHandler::getErrors() const {
    return errors_;
}

/**
 * @brief Filtra y retorna solo los errores de una línea específica.
 * * @details Útil para mostrar errores en contexto, por ejemplo junto al código fuente de esa línea.
 * * @param line Número de línea a filtrar (desde 1).
 * @return std::vector<LexicalError> Vector con los errores de esa línea.
 */
std::vector<LexicalError> ErrorHandler::getErrorsByLine(int line) const {
    std::vector<LexicalError> result;
    for (const auto& error : errors_) {
        if (error.line == line) {
            result.push_back(error);
        }
    }
    return result;
}

/**
 * @brief Elimina todos los errores registrados.
 * * @details Permite reutilizar el mismo ErrorHandler para múltiples análisis sin crear una nueva instancia.
 */
void ErrorHandler::clear() {
    errors_.clear();
}

/**
 * @brief Imprime el reporte completo de errores a un stream.
 * * @details 
 * Formato del reporte:
 * ┌─────────────────────────────────────┐
 * │     REPORTE DE ERRORES LÉXICOS      │
 * │  Se encontraron N error(es)         │
 * ├─────────────────────────────────────┤
 * │ [ERROR] Línea X, Col Y → 'lex': msg │
 * │ ...                                 │
 * ├─────────────────────────────────────┤
 * │  Análisis completado con N error(es)│
 * └─────────────────────────────────────┘
 * Si no hay errores imprime un mensaje de éxito.
 * * @param out Stream de salida (ej. std::cout, std::cerr, archivo).
 */
void ErrorHandler::printReport(std::ostream& out) const {
    const std::string separator(55, '-');

    out << "\n" << separator << "\n";
    out << "     REPORTE DE ERRORES LÉXICOS\n";
    out << separator << "\n";

    if (errors_.empty()) {
        out << "  ✓ Sin errores léxicos. Análisis exitoso.\n";
        out << separator << "\n";
        return;
    }

    out << "  Se encontraron " << errors_.size() << " error(es):\n";
    out << separator << "\n";

    // Imprime cada error con su información completa
    for (const auto& error : errors_) {
        out << "  " << error.toString() << "\n";
    }

    out << separator << "\n";
    printSummary(out);
    out << separator << "\n";
}

/**
 * @brief Imprime solo el resumen del análisis (sin detalle por error).
 * * @details Muestra el total de errores o un mensaje de éxito si no hay errores.
 * * @param out Stream de salida.
 */
void ErrorHandler::printSummary(std::ostream& out) const {
    if (errors_.empty()) {
        out << "  ✓ Análisis completado sin errores.\n";
    } else {
        out << "  ✗ Análisis completado con "
            << errors_.size() << " error(es) léxico(s).\n";
    }
}


// =============================================================
//  ErrorHandler — Métodos privados
// =============================================================

/**
 * @brief Construye el mensaje descriptivo según el tipo de error.
 * * @details Cada tipo de error tiene un mensaje claro y orientado al programador, 
 * indicando qué salió mal y posibles causas.
 * * @param type   Tipo de error a describir.
 * @param lexeme Lexema que causó el error (incluido en el mensaje).
 * @return std::string String con el mensaje descriptivo.
 */
std::string ErrorHandler::buildMessage(ErrorType type,
                                        const std::string& lexeme) const
{
    std::ostringstream oss;

    switch (type) {
        case ErrorType::INVALID_CHARACTER:
            oss << "Carácter inválido '" << lexeme
                << "'. No pertenece al alfabeto del lenguaje.";
            break;

        case ErrorType::MALFORMED_NUMBER:
            oss << "Número con formato inválido '" << lexeme
                << "'. Los números no pueden contener letras. "
                << "¿Quiso escribir un identificador o un número separado?";
            break;

        case ErrorType::INVALID_IDENTIFIER:
            oss << "Identificador inválido '" << lexeme
                << "'. Los identificadores deben iniciar con letra o '_', "
                << "no con un dígito.";
            break;

        case ErrorType::INCOMPLETE_FLOAT:
            oss << "Número real incompleto '" << lexeme
                << "'. Se esperaban dígitos antes y después del punto decimal.";
            break;

        case ErrorType::UNEXPECTED_TOKEN:
            oss << "Token inesperado '" << lexeme
                << "'. El símbolo no es válido en este contexto.";
            break;

        case ErrorType::GENERIC_ERROR:
        default:
            oss << "Error léxico en '" << lexeme
                << "'. Lexema no reconocido por el analizador.";
            break;
    }

    return oss.str();
}

/**
 * @brief Convierte un ErrorType a su nombre en string.
 * * @details Usado al imprimir el tipo de error en el reporte detallado.
 * * @param type Tipo de error.
 * @return std::string Nombre del tipo como string.
 */
std::string ErrorHandler::errorTypeToString(ErrorType type) const {
    switch (type) {
        case ErrorType::INVALID_CHARACTER:  return "INVALID_CHARACTER";
        case ErrorType::MALFORMED_NUMBER:   return "MALFORMED_NUMBER";
        case ErrorType::INVALID_IDENTIFIER: return "INVALID_IDENTIFIER";
        case ErrorType::INCOMPLETE_FLOAT:   return "INCOMPLETE_FLOAT";
        case ErrorType::UNEXPECTED_TOKEN:   return "UNEXPECTED_TOKEN";
        case ErrorType::GENERIC_ERROR:      return "GENERIC_ERROR";
        default:                            return "UNKNOWN_ERROR";
    }
}