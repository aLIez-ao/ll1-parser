/**
 * @file follow_set.h
 * @brief Calculador de conjuntos FOLLOW para la gramática LL(1)
 * @description
 * Este módulo implementa el algoritmo de cálculo de conjuntos FOLLOW
 * usando iteración de punto fijo.
 * 
 * Los conjuntos FOLLOW son necesarios para construir la tabla LL(1)
 * cuando existen producciones que pueden derivar epsilon (ε).
 * 
 * FOLLOW(A) = conjunto de terminales que pueden aparecer justo después
 *             de una derivación de A en alguna posición del programa.
 * 
 * Reglas del algoritmo:
 * 1. $ ∈ FOLLOW(S) donde S es el símbolo inicial
 * 2. Si A → αBβ, entonces FIRST(β) - {ε} ⊆ FOLLOW(B)
 * 3. Si A → αBβ y ε ∈ FIRST(β), entonces FOLLOW(A) ⊆ FOLLOW(B)
 * 4. Si A → αB (β puede ser vacío), entonces FOLLOW(A) ⊆ FOLLOW(B)
 * 
 * @author Equipo Gramática
 * @version 1.0
 * @see FirstSetCalculator, LL1TableGenerator
 */

#pragma once
#include "../core/grammar.h"
#include "first_set.h"

/**
 * @class FollowSetCalculator
 * @brief Calcula los conjuntos FOLLOW para todos los no-terminales
 * @description
 * FOLLOW(A) contiene los símbolos que pueden aparecer inmediatamente
 * después de A en una derivación válida del programa.
 * 
 * El símbolo inicial siempre tiene $ (fin de archivo) en su FOLLOW.
 * 
 * Ejemplo:
 * Dada la gramática: E -> T E'
 *                      E' -> + T E' | ε
 * FOLLOW(E) = { $, SEMICOLON }  (viene después de E en el programa)
 * FOLLOW(E') = FOLLOW(E) = { $, SEMICOLON }
 */
class FollowSetCalculator {
public:
    /**
     * @brief Calcula los conjuntos FOLLOW para toda la gramática
     * @param g Gramática de entrada
     * @param firstSets Conjuntos FIRST precalculados (necesarios para el cálculo)
     * @return FirstFollowMap Mapa que asocia cada no-terminal con su conjunto FOLLOW
     * 
     * @complexity O(n² * m) donde n = no-terminales, m = producciones
     * @note Requiere que los conjuntos FIRST estén calculados primero
     * @note El algoritmo itera hasta que los conjuntos se estabilicen
     */
    static FirstFollowMap compute(const Grammar&       g,
                                  const FirstFollowMap& firstSets);
};
