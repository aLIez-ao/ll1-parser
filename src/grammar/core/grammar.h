/**
 * @file grammar.h
 * @brief Estructuras de datos fundamentales para representar la gramática LL(1)
 * @description
 * Este archivo define las estructuras principales usadas por el módulo de gramática:
 * - Symbol: representa un símbolo (terminal o no-terminal)
 * - Grammar: representa una gramática completa con todas sus reglas
 * - Type aliases: definiciones de tipos para simplificar el código
 * 
 * La gramática se representa mediante:
 * - Conjunto de no-terminales
 * - Conjunto de terminales
 * - Mapa de reglas de producción
 * - Símbolo inicial
 * 
 * @author Equipo Gramática
 * @version 1.0
 */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * @brief Constantes para palabras reservadas del sistema de gramática
 * @details
 * Estas constantes definen los identificadores especiales usados internamente
 * para representar elementos de la gramática que no son parte del lenguaje fuente.
 */
///@{
/**
 * @brief Representa la producción vacía (epsilon/λ)
 * @details En la gramática, EPS representa una producción que no genera ningún símbolo.
 * Ejemplo: D -> EPS significa que la sección de declaraciones puede ser vacía.
 */
inline const std::string EPSILON_KW  = "EPS";   

/**
 * @brief Alternativa para representar epsilon
 * @details Algunas gramáticas usan "null" en lugar de "EPS" para la producción vacía.
 */
inline const std::string NULL_KW     = "null";  

/**
 * @brief Marcador de fin de cadena de entrada
 * @details El símbolo $ marca el final de la entrada en el parser.
 * FOLLOW(A) siempre incluye $ si A puede derivar la cadena vacía hasta el final.
 */
inline const std::string END_MARKER  = "$";     
///@}

/**
 * @struct Symbol
 * @brief Representa un símbolo en la gramática (terminal o no-terminal)
 * @description
 * Cada símbolo tiene un nombre y un indicador de tipo. Los símbolos son los
 * bloques fundamentales que forman las producciones de la gramática.
 * 
 * Un símbolo puede ser:
 * - No-terminal (isNT = true): Variables gramaticales que se pueden expandir
 * - Terminal (isNT = false): Símbolos concretos del lenguaje
 * - Epsilon (isNT = false): Producción vacía
 */
struct Symbol {
    /**
     * @brief Nombre del símbolo
     * @details Para no-terminales: mayúsculas (P, D, L, etc.)
     *          Para terminales: palabras clave o símbolos (PROGRAM, ID, +, etc.)
     */
    std::string name;
    
    /**
     * @brief Indicador de tipo de símbolo
     * @details true = No-terminal (se puede expandir)
     *          false = Terminal o Epsilon
     */
    bool        isNT;   

    /**
     * @brief Verifica si el símbolo representa producción vacía (epsilon)
     * @return true si el símbolo es epsilon (EPS o null)
     */
    bool isEpsilon()   const { return name == EPSILON_KW || name == NULL_KW; }
    
    /**
     * @brief Verifica si el símbolo es el marcador de fin de entrada
     * @return true si el símbolo es "$"
     */
    bool isEndMarker() const { return name == END_MARKER; }

    /**
     * @brief Operador de igualdad para comparar símbolos
     */
    bool operator==(const Symbol& o) const { return name == o.name; }
    
    /**
     * @brief Operador de comparación para usar en contenedores ordenados
     */
    bool operator< (const Symbol& o) const { return name  < o.name; }
};

/**
 * @name Type aliases
 * @brief Definiciones de tipos para simplificar el código del módulo de gramática
 * @{
 */

/**
 * @brief Representa una producción: secuencia de símbolos
 * @details Una producción es un vector de símbolos que constituyen el lado derecho
 *          de una regla gramatical. Ejemplo: P -> PROGRAM ID SEMICOLON D B
 *          se representa como: Production = [PROGRAM, ID, SEMICOLON, D, B]
 */
using Production     = std::vector<Symbol>;

/**
 * @brief Mapa de reglas: no-terminal -> lista de producciones
 * @details Para cada no-terminal, almacenamos todas sus producciones alternativas.
 *          Ejemplo: D -> VAR L | EPS se almacena como:
 *          rules["D"] = [ [VAR, L], [EPS] ]
 */
using RuleMap        = std::map<std::string, std::vector<Production>>;

/**
 * @brief Conjunto de cadenas (strings)
 * @details Usado para representar conjuntos FIRST y FOLLOW
 */
using StringSet      = std::set<std::string>;

/**
 * @brief Mapa de conjuntos FIRST/FOLLOW por no-terminal
 * @details Primero[F] = { elementos }, FOLLOW[F] = { elementos }
 */
using FirstFollowMap = std::map<std::string, StringSet>;

/** @} */

/**
 * @struct Grammar
 * @brief Representa una gramática LL(1) completa
 * @description
 * Almacena todos los componentes de una gramática:
 * - Símbolo inicial: el no-terminal por donde empieza el análisis
 * - No-terminales: conjunto de todos los no-terminales de la gramática
 * - Terminales: conjunto de todos los terminales
 * - Reglas: mapa de producciones por cada no-terminal
 * - Orden: lista que preserva el orden de inserción de no-terminales
 */
struct Grammar {
    /**
     * @brief Símbolo inicial de la gramática
     * @details Por convención, usually es 'P' (Programa). Este es el símbolo
     *          que se apila inicialmente en el parser.
     */
    std::string              startSymbol;
    
    /**
     * @brief Lista de no-terminales en orden de inserción
     * @details Useful for display purposes, mantener consistencia en la salida
     */
    std::vector<std::string> ntOrder;      
    
    /**
     * @brief Conjunto de no-terminales
     * @details Contain all non-terminal symbols like P, D, L, T, B, S, E, F, etc.
     */
    StringSet                nonTerminals;
    
    /**
     * @brief Conjunto de terminales
     * @details Contains all terminal symbols: PROGRAM, ID, NUM, PLUS, etc.
     */
    StringSet                terminals;
    
    /**
     * @brief Mapa de reglas de producción
     * @details Mapea cada no-terminal a sus producciones alternativas
     */
    RuleMap                  rules;

    /**
     * @brief Crea un símbolo con el tipo correcto (NT o Terminal)
     * @param name Nombre del símbolo a crear
     * @return Symbol con isNT = true si es no-terminal, false si es terminal
     * @note Si el nombre no corresponde a un no-terminal conocido, se asume terminal
     */
    Symbol makeSymbol(const std::string& name) const;
};
