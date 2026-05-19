/**
 * @file parser.h
 * @brief Definición del parser LL(1) de pila no recursivo
 * @description
 * Este módulo implementa el corazón del analizador sintáctico: el parser LL(1).
 * Utiliza un algoritmo de pila no recursivo que consulta la tabla LL(1)
 * para determinar qué acción tomar en cada paso del análisis.
 * 
 * Algoritmo del parser:
 * 1. Inicializar pila con $ (marcador de fondo) y símbolo inicial
 * 2. Obtener el primer token de la entrada
 * 3. Mientras la pila no esté vacía:
 *    a. Si el tope es $: verificar si token es $ → ÉXITO
 *    b. Si el tope es un terminal:
 *       - Si coincide con token: hacer pop y avanzar al siguiente token
 *       - Si no coincide: reportar error
 *    c. Si el tope es un no-terminal:
 *       - Consultar tabla[tope][token]
 *       - Si hay producción: hacer pop y apilar producción invertida
 *       - Si celda vacía: reportar error
 * 
 * Además del análisis sintáctico, este módulo:
 * - Construye el Árbol Sintáctico Abstracto (AST) en paralelo
 * - Registra cada paso en la traza para debugging
 * - Mantiene una tabla de símbolos del parser
 * 
 * @author Equipo Parser
 * @version 1.0
 * @see LL1Table, ASTNode, TraceManager
 */

#pragma once

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <set>

#include "../lexer/token/token.h"
#include "../grammar/analysis/ll1_table.h"
#include "ast/ast_node.h"
#include "trace/trace_manager.h"
#include "symbol_manager.h"

/**
 * @brief Marcador especial para construcción del AST
 * @details
 * Este string se apila en la pila de análisis para señalar cuándo
 * se debe cerrar un nodo del AST. Cuando el parser encuentra este
 * marcador, hace pop del nodo actual del astStack_.
 * 
 * El AST se construye en paralelo con el parsing:
 * - Al expandir un no-terminal: se crea un nuevo nodo AST
 * - Al hacer match de un terminal: se crea un nodo terminal
 * - Al encontrar este marcador: se cierra el nodo actual
 */
static const std::string AST_POP_MARKER = "$$AST_POP$$";

/**
 * @class LL1Parser
 * @brief Parser LL(1) basado en pila no recursiva
 * @description
 * Esta clase implementa el algoritmo de parsing descendente recursivo
 * usando una pila explícita en lugar de recursión.
 * 
 * El parser recibe:
 * - Lista de tokens del lexer
 * - Tabla LL(1) precalculada
 * - Símbolo inicial de la gramática
 * - Lista de no-terminales
 * 
 * El parser produce:
 * - AST (árbol sintáctico abstracto) si buildAST_ = true
 * - Lista de errores sintácticos
 * - Traza de ejecución (opcional)
 * - Tabla de símbolos del parser
 */
class LL1Parser {
public:
    /**
     * @brief Constructor por defecto
     * @details Inicializa el parser con valores predeterminados
     */
    LL1Parser();
    
    /**
     * @brief Destructor por defecto
     */
    ~LL1Parser();

    /**
     * @brief Establece el nivel de detalle de la traza
     * @param level Nivel de traceo (NONE, MINIMAL, NORMAL, VERBOSE, DEBUG)
     */
    void setTraceLevel(TraceLevel level);
    
    /**
     * @brief Define el archivo donde se escribirá la traza
     * @param filename Ruta del archivo de traza
     */
    void setTraceFile(const std::string& filename);
    
    /**
     * @brief Habilita o deshabilita la construcción del AST
     * @param enable true = construir AST, false = solo analizar
     */
    void enableASTBuilding(bool enable);

    /**
     * @struct Result
     * @brief Estructura que contiene el resultado del análisis sintáctico
     */
    struct Result {
        /**
         * @brief true si el análisis fue exitoso (sin errores sintácticos)
         */
        bool success = false;
        
        /**
         * @brief Árbol sintáctico abstracto generado
         * @note Solo válido si buildAST_ = true y success = true
         */
        std::shared_ptr<ASTNode> ast;
        
        /**
         * @brief Lista de mensajes de error sintáctico
         */
        std::vector<std::string> errors;
        
        /**
         * @brief Conteo de errores encontrados
         */
        int errorCount = 0;
    };

    /**
     * @brief Ejecuta el análisis sintáctico LL(1)
     * @param tokens Lista de tokens generados por el lexer
     * @param ll1Table Tabla LL(1) precalculada
     * @param startSymbol Símbolo inicial de la gramática (generalmente "P")
     * @param nonTerminales Lista de todos los no-terminales de la gramática
     * @return Result Estructura con el resultado del análisis
     * 
     * @note Este método es thread-safe si cada hilo tiene su propia instancia de LL1Parser
     */
    Result parse(const std::vector<Token>& tokens,
                 const LL1Table& ll1Table,
                 const std::string& startSymbol,
                 const std::vector<std::string>& nonTerminals);

    /**
     * @brief Obtiene el AST generado (si existe)
     * @return shared_ptr al nodo raíz del AST
     */
    std::shared_ptr<ASTNode> getAST() const;
    
    /**
     * @brief Obtiene el gestor de trazas
     * @return Referencia constante al TraceManager
     */
    const TraceManager& getTraceManager() const;
    
    /**
     * @brief Obtiene el gestor de símbolos
     * @return Referencia constante al SymbolManager
     */
    const SymbolManager& getSymbolManager() const;

private:
    /**
     * @brief Pila de análisis sintáctico
     * @details
     * Almacena símbolos (terminales y no-terminales) durante el parsing.
     * El símbolo $ marca el fondo de la pila.
     */
    std::stack<std::string> stack_;
    
    /**
     * @brief Copia local de los tokens de entrada
     */
    std::vector<Token> tokens_;
    
    /**
     * @brief Índice del token actual en la lista de tokens
     */
    size_t currentTokenIndex_ = 0;
    
    /**
     * @brief Copia de la tabla LL(1) para consultas durante el parsing
     */
    LL1Table ll1Table_;
    
    /**
     * @brief Copia del símbolo inicial de la gramática
     */
    std::string startSymbol_;
    
    /**
     * @brief Conjunto de no-terminales (para rápido membership test)
     */
    std::set<std::string> nonTerminals_;
    
    /**
     * @brief Puntero al resultado actual (para reportar errores durante parsing)
     */
    Result* currentResult_ = nullptr;
    
    /**
     * @brief Pila para construcción del AST
     * @details
     * Mantiene nodos AST mientras se procesan las producciones.
     * El nodo en el tope es el "actual" al que se le agregan hijos.
     */
    std::stack<std::shared_ptr<ASTNode>> astStack_;

    /**
     * @brief Constructor del AST
     */
    ASTBuilder astBuilder_;
    
    /**
     * @brief Gestor de trazas
     */
    TraceManager traceManager_;
    
    /**
     * @brief Gestor de símbolos del parser
     */
    SymbolManager symbolManager_;
    
    /**
     * @brief Bandera para habilitar/deshabilitar construcción del AST
     */
    bool buildAST_ = true;

    /**
     * @brief Inicializa el parser para un nuevo análisis
     * @param tokens Lista de tokens
     * @param table Tabla LL(1)
     * @param start Símbolo inicial
     * @param nonTerminals Lista de no-terminales
     * 
     * @note Reinicia las estructuras internas: limpia las pilas,
     *       resetea el índice de token, etc.
     */
    void init(const std::vector<Token>& tokens, const LL1Table& table, const std::string& start, const std::vector<std::string>& nonTerminals);
    
    /**
     * @brief Verifica si un símbolo es un terminal
     * @param sym Símbolo a verificar
     * @return true si es terminal (no está en el conjunto de no-terminales)
     */
    bool isTerminal(const std::string& sym) const;
    
    /**
     * @brief Verifica si un símbolo es un no-terminal
     * @param sym Símbolo a verificar
     * @return true si es no-terminal
     */
    bool isNonTerminal(const std::string& sym) const;
    
    /**
     * @brief Obtiene el símbolo del parser correspondiente al token actual
     * @return String con el símbolo (o "$" si es EOF)
     */
    std::string currentSymbol() const;
    
    /**
     * @brief Verifica si hay más tokens por procesar
     * @return true si hay tokens disponibles
     */
    bool hasMoreTokens() const;
    
    /**
     * @brief Avanza al siguiente token en la entrada
     */
    void nextToken();
    
    /**
     * @brief Realiza la acción de MATCH para un terminal
     * @param term Terminal esperado que debe coincidir con el token actual
     * 
     * @note Si coincide: hace pop de la pila y avanza al siguiente token
     *       Si no coincide: reporta error pero continúa
     */
    void matchTerminal(const std::string& term);
    
    /**
     * @brief Expande un no-terminal usando la tabla LL(1)
     * @param nt No-terminal a expandir
     * 
     * @note Busca en la tabla[nt][tokenActual] la producción a aplicar
     *       Hace pop del no-terminal y apila la producción en orden inverso
     *       También crea los nodos correspondientes del AST
     */
    void expandNonTerminal(const std::string& nt);
    
    /**
     * @brief Reporta un error sintáctico
     * @param expected Símbolo que se esperaba
     * @param got Símbolo que se recibió
     * 
     * @note Agrega el error a la lista de errores del resultado
     *       e incrementa el conteo de errores
     */
    void error(const std::string& expected, const std::string& got);
};