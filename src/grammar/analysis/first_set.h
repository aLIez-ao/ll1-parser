/**
 * @file first_set.h
 * @brief Calculador de conjuntos FIRST para la gramática LL(1)
 * @description
 * Este módulo implementa el algoritmo de cálculo de conjuntos FIRST
 * usando iteración de punto fijo (fixed-point iteration).
 * 
 * Los conjuntos FIRST son fundamentales para construir la tabla LL(1).
 * FIRST(α) = conjunto de terminales que pueden aparecer al inicio
 *            de cualquier derivación de α.
 * 
 * Algoritmo:
 * 1. Inicializar FIRST(X) = {} para todo X
 * 2. Repetir hasta que no hayan cambios:
 *    - Para cada producción A → α:
 *      - Agregar FIRST(α) a FIRST(A)
 * 
 * Donde FIRST(α) se calcula como:
 * - Si α comienza con terminal a: {a}
 * - Si α comienza con no-terminal A:
 *   - Agregar FIRST(A) - {ε}
 *   - Si A puede derivar ε, continuar con el siguiente símbolo
 * - Si α puede derivar ε completamente: {ε}
 * 
 * @author Equipo Gramática
 * @version 1.0
 * @see FollowSetCalculator, LL1TableGenerator
 */

#pragma once
#include "../core/grammar.h"

/**
 * @class FirstSetCalculator
 * @brief Calcula los conjuntos FIRST para todos los no-terminales de una gramática
 * @description
 * Esta clase proporciona métodos estáticos para:
 * 1. Calcular los conjuntos FIRST de todos los no-terminales
 * 2. Calcular FIRST de una secuencia arbitraria de símbolos
 * 
 * El algoritmo usa iteración de punto fijo: se repite hasta que los
 * conjuntos no cambien (se alcanza estabilidad).
 */
class FirstSetCalculator {
public:
    /**
     * @brief Calcula los conjuntos FIRST para toda la gramática
     * @param g Gramática de entrada
     * @return FirstFollowMap Mapa que asocia cada no-terminal con su conjunto FIRST
     * 
     * @complexity O(n * m) donde n = no-terminales, m = producciones
     * @note El algoritmo itera hasta 100 veces como máximo (protección contra bucles infinitos)
     */
    static FirstFollowMap compute(const Grammar& g);

    /**
     * @brief Calcula el conjunto FIRST de una secuencia de símbolos
     * @description
     * Método utilitario para calcular FIRST(α) donde α es una secuencia
     * de símbolos. Esencial para:
     * - Construir la tabla LL(1)
     * - Calcular conjuntos FOLLOW
     * - Determinar si una producción puede derivar ε
     * 
     * @param seq Secuencia de símbolos (puede ser una producción completa o parcial)
     * @param firstSets Conjuntos FIRST ya calculados para cada no-terminal
     * @return StringSet Conjunto FIRST de la secuencia
     * 
     * @note Si la secuencia está vacía, retorna {ε}
     */
    static StringSet firstOfSequence(const std::vector<Symbol>& seq,
                                     const FirstFollowMap&      firstSets);
};
