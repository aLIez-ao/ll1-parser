#pragma once

#include <string>
#include <vector>
#include <map>
#include "../../lexer/token/token.h"

// ── Entrada de la tabla de símbolos semántica ─────────────────────────────────
struct SemanticSymbol {
    std::string name;
    std::string type;       // "int" | "float"
    int declLine;
    int declCol;
    bool used = false;
};

// ── Resultado del análisis semántico ─────────────────────────────────────────
struct SemanticResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, SemanticSymbol> symbols; // tabla de símbolos
};

// ── Analizador Semántico ──────────────────────────────────────────────────────
// Recorre la lista de tokens (ya válida sintácticamente) y verifica:
//  1. Declaraciones duplicadas de variables.
//  2. Uso de variables no declaradas.
//  3. Variables declaradas pero nunca usadas (advertencia).
// ─────────────────────────────────────────────────────────────────────────────
class SemanticAnalyzer {
public:
    SemanticResult analyze(const std::vector<Token>& tokens);

private:
    void firstPass(const std::vector<Token>& tokens, SemanticResult& result);
    void secondPass(const std::vector<Token>& tokens, SemanticResult& result);

    // Estado interno durante el análisis
    bool inDecl_  = false;
    bool inBlock_ = false;
};
