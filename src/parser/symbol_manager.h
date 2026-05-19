/**
 * @file symbol_manager.h
 * @brief Gestor de tabla de símbolos del parser
 * @description
 * Este módulo proporciona gestión de símbolos durante el parsing.
 * Mantiene un registro de variables declaradas, sus tipos y ubicaciones,
 * y permite validar que las variables se usen correctamente.
 * 
 * Características:
 * - Soporte para múltiples ámbitos (scopes) - preparado para expansión
 * - Detección de declaraciones duplicadas
 * - Validación de uso de variables (declaradas antes de usar)
 * - Seguimiento de inicialización de variables
 * 
 * @note Este módulo está preparado para扩展 a funciones y procedimientos
 *       donde cada función tendría su propio ámbito.
 * 
 * @author Equipo Parser
 * @version 1.0
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

// Forward declaration
namespace lexer {
    struct Token;
}

/**
 * @struct ParserSymbolEntry
 * @brief Representa una entrada en la tabla de símbolos del parser
 * @description
 * Almacena la información de una variable declarada:
 * - Nombre del identificador
 * - Tipo de datos (int, float, etc.)
 * - Línea y columna de la declaración
 * - Estado de inicialización
 */
struct ParserSymbolEntry {
    /** @brief Nombre de la variable */
    std::string name;
    
    /** @brief Tipo de datos (int, float, etc.) */
    std::string type;
    
    /** @brief Línea donde fue declarada */
    int line;
    
    /** @brief Columna donde fue declarada */
    int column;
    
    /** @brief Indica si la variable ha sido inicializada */
    bool isInitialized;

    /** @brief Constructor por defecto */
    ParserSymbolEntry() : name(""), type(""), line(0), column(0), isInitialized(false) {}

    /** @brief Constructor con parámetros */
    ParserSymbolEntry(const std::string& n, const std::string& t, int l, int c)
        : name(n), type(t), line(l), column(c), isInitialized(false) {}
};

/**
 * @class SymbolManager
 * @brief Gestor de tabla de símbolos para el parser
 * @description
 * Mantiene un registro de todos los identificadores declarados durante el parsing.
 * Proporciona métodos para:
 * - Declarar nuevas variables (con validación de duplicados)
 * - Consultar si una variable está declarada
 * - Obtener información de una variable
 * - Rastrear el uso de variables
 * - Gestión de ámbitos (preparado para expansión)
 * 
 * El gestor mantiene una pila de ámbitos (scopes) que permite扩展
 * a futuro para soportar funciones y procedimientos anidados.
 */
class SymbolManager {
public:
    /** @brief Constructor por defecto */
    SymbolManager();

    // ============================================================
    // Gestión de declaraciones
    // ============================================================
    
    /**
     * @brief Declara una nueva variable
     * @param name Nombre del identificador
     * @param type Tipo de datos
     * @param line Línea de declaración
     * @param column Columna de declaración
     * @return true si se declaró exitosamente, false si ya existe (duplicado)
     * 
     * @note Si la variable ya existe, no la redeclara y retorna false
     */
    bool declare(const std::string& name, const std::string& type, int line, int column);
    
    /**
     * @brief Verifica si una variable está declarada
     * @param name Nombre a buscar
     * @return true si existe en el ámbito actual
     */
    bool isDeclared(const std::string& name) const;
    
    /**
     * @brief Busca y retorna información de una variable
     * @param name Nombre a buscar
     * @return Puntero a la entrada o nullptr si no existe
     */
    const ParserSymbolEntry* lookup(const std::string& name) const;

    // ============================================================
    // Rastreo de uso
    // ============================================================
    
    /**
     * @brief Marca una variable como inicializada
     * @param name Nombre de la variable
     */
    void markInitialized(const std::string& name);
    
    /**
     * @brief Verifica si una variable está inicializada
     * @param name Nombre a verificar
     * @return true si está inicializada
     */
    bool isInitialized(const std::string& name) const;

    // ============================================================
    // Validación
    // ============================================================
    
    /**
     * @brief Verifica si una variable ya fue declarada
     * @param name Nombre a verificar
     * @return true si es una declaración duplicada
     */
    bool hasDuplicateDeclaration(const std::string& name) const;
    
    /**
     * @brief Obtiene la lista de errores acumulados
     * @return Vector de mensajes de error
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief Obtiene la lista de advertencias acumuladas
     * @return Vector de mensajes de advertencia
     */
    std::vector<std::string> getWarnings() const;

    // ============================================================
    // Salida
    // ============================================================
    
    /**
     * @brief Imprime la tabla de símbolos a un stream
     * @param out Stream de salida (cout, archivo, etc.)
     */
    void printTable(std::ostream& out) const;
    
    /**
     * @brief Genera representación string de la tabla
     * @return String con la tabla formateada
     */
    std::string toString() const;

    // ============================================================
    // Gestión de ámbitos (para extensión futura)
    // ============================================================
    
    /**
     * @brief Apila un nuevo ámbito
     * @note Preparado para funciones/procedimientos anidados
     */
    void pushScope();
    
    /**
     * @brief Desapila el ámbito actual
     * @note Al salir de un bloque, se eliminan las variables locales
     */
    void popScope();

    // ============================================================
    // Información
    // ============================================================
    
    /**
     * @brief Retorna el número de símbolos en la tabla
     * @return Cantidad de variables declaradas
     */
    int getSymbolCount() const;

private:
    /**
     * @brief Estructura que representa un ámbito
     * @details Cada ámbito contiene su propio mapa de símbolos
     */
    struct Scope {
        std::map<std::string, ParserSymbolEntry> symbols;
    };

    /** @brief Pila de ámbitos (scopes) */
    std::vector<Scope> scopes_;
    
    /** @brief Lista de errores encontrados */
    std::vector<std::string> errors_;
    
    /** @brief Lista de advertencias */
    std::vector<std::string> warnings_;

    /**
     * @brief Obtiene referencia al ámbito actual
     * @return Referencia al scope en el tope de la pila
     */
    Scope& currentScope();
    
    /**
     * @brief Obtiene referencia constante al ámbito actual
     */
    const Scope& currentScope() const;
};

/**
 * @struct ParseResult
 * @brief Estructura legacy para resultados de parsing
 * @deprecated Usar LL1Parser::Result en su lugar
 * 
 * @note Este struct se mantiene por compatibilidad pero
 *       el código nuevo debería usar LL1Parser::Result
 */
struct ParseResult {
    bool success;                              ///< Análisis exitoso
    std::shared_ptr<class ASTNode> ast;      ///< Árbol generado
    std::vector<std::string> errors;         ///< Lista de errores
    std::vector<std::string> warnings;       ///< Lista de advertencias
    int errorCount;                           ///< Conteo de errores
    int warningCount;                          ///< Conteo de advertencias

    /** @brief Constructor por defecto */
    ParseResult()
        : success(false), errorCount(0), warningCount(0) {}
};