#include "semantic_analyzer.h"
#include <sstream>

// ── Entrada pública ───────────────────────────────────────────────────────────
SemanticResult SemanticAnalyzer::analyze(const std::vector<Token>& tokens) {
    SemanticResult result;
    inDecl_  = false;
    inBlock_ = false;

    firstPass(tokens, result);   // Recolectar declaraciones
    secondPass(tokens, result);  // Validar usos

    // Advertir variables declaradas pero no usadas
    for (auto& [name, sym] : result.symbols) {
        if (!sym.used) {
            std::ostringstream ss;
            ss << "Advertencia: variable '" << name
               << "' declarada en línea " << sym.declLine
               << " nunca se usa.";
            result.warnings.push_back(ss.str());
        }
    }

    if (!result.errors.empty()) result.valid = false;
    return result;
}

// ── Primera pasada: registrar declaraciones ───────────────────────────────────
void SemanticAnalyzer::firstPass(const std::vector<Token>& tokens, SemanticResult& result) {
    bool inDecl = false;
    std::string lastID;
    int lastIDLine = 0, lastIDCol = 0;

    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& t = tokens[i];
        const std::string sym = t.toParserSymbol();

        if (sym == "VAR")   { inDecl = true;  continue; }
        if (sym == "BEGIN") { inDecl = false; continue; }

        if (!inDecl) continue;

        // Capturar "ID : TYPE ;"
        if (sym == "ID") {
            lastID = t.lexeme;
            lastIDLine = t.line;
            lastIDCol  = t.column;
        } else if ((sym == "INT" || sym == "FLOAT") && !lastID.empty()) {
            std::string typeName = (sym == "INT") ? "int" : "float";

            if (result.symbols.count(lastID)) {
                std::ostringstream ss;
                ss << "Error semántico (línea " << lastIDLine << "): variable '"
                   << lastID << "' ya fue declarada en línea "
                   << result.symbols[lastID].declLine << ".";
                result.errors.push_back(ss.str());
            } else {
                result.symbols[lastID] = {lastID, typeName, lastIDLine, lastIDCol, false};
            }
            lastID.clear();
        }
    }
}

// ── Segunda pasada: validar usos en el bloque ─────────────────────────────────
void SemanticAnalyzer::secondPass(const std::vector<Token>& tokens, SemanticResult& result) {
    bool inBlock = false;

    for (size_t i = 0; i + 1 < tokens.size(); i++) {
        const auto& t   = tokens[i];
        const auto& t1  = tokens[i + 1];
        const std::string sym  = t.toParserSymbol();
        const std::string sym1 = t1.toParserSymbol();

        if (sym == "BEGIN") { inBlock = true;  continue; }
        if (sym == "END")   { inBlock = false; continue; }

        if (!inBlock) continue;

        if (sym == "ID") {
            // Asignación a la izquierda: "ID ="
            if (sym1 == "ASSIGN") {
                if (!result.symbols.count(t.lexeme)) {
                    std::ostringstream ss;
                    ss << "Error semántico (línea " << t.line << "): variable '"
                       << t.lexeme << "' usada sin declarar.";
                    result.errors.push_back(ss.str());
                } else {
                    result.symbols[t.lexeme].used = true;
                }
            }
            // Uso en expresión: "ID" sin "=" a continuación
            else if (sym1 != "COLON") {
                if (!result.symbols.count(t.lexeme)) {
                    std::ostringstream ss;
                    ss << "Error semántico (línea " << t.line << "): variable '"
                       << t.lexeme << "' usada sin declarar.";
                    result.errors.push_back(ss.str());
                } else {
                    result.symbols[t.lexeme].used = true;
                }
            }
        }
    }
}
