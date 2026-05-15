#include "validator.h"
#include <sstream>

// ── Public entry point ───────────────────────────────────────────────────────
ValidationResult Validator::validate(const Grammar& g) {
    ValidationResult result;
    result.valid = true;

    checkLeftRecursion   (g, result);
    checkUndefinedSymbols(g, result);

    if (!result.errors.empty()) result.valid = false;
    return result;
}

// ── Rule 1: Direct left recursion ───────────────────────────────────────────
// A production A -> A ... makes FIRST undefined and the grammar unusable
// for top-down parsing.
void Validator::checkLeftRecursion(const Grammar& g, ValidationResult& r) {
    for (const auto& [nt, prods] : g.rules) {
        for (const auto& prod : prods) {
            if (prod.empty()) continue;
            const Symbol& first = prod[0];
            if (first.isNT && first.name == nt) {
                std::ostringstream oss;
                oss << "Recursion izquierda directa en: "
                    << nt << " -> " << nt << " ...";
                r.errors.push_back(oss.str());
            }
        }
    }
}

// ── Rule 2: Undefined non-terminals ─────────────────────────────────────────
// A symbol that LOOKS like a non-terminal (starts with an uppercase letter)
// appears in the RHS of a rule but has no production of its own.
//
// Convention used: NT names start with an uppercase letter (A-Z).
// Terminals are lowercase words, digits, or operator symbols.
// ─────────────────────────────────────────────────────────────────────────────
static bool looksLikeNT(const std::string& name) {
    return !name.empty() && std::isupper(static_cast<unsigned char>(name[0]));
}

void Validator::checkUndefinedSymbols(const Grammar& g, ValidationResult& r) {
    std::set<std::string> seen;

    for (const auto& [nt, prods] : g.rules) {
        for (const auto& prod : prods) {
            for (const auto& sym : prod) {
                if (sym.isEpsilon()) continue;

                // Flag any RHS symbol that looks like a NT but has no rule
                if (looksLikeNT(sym.name) &&
                    g.rules.find(sym.name) == g.rules.end()) {
                    if (seen.insert(sym.name).second) {
                        std::ostringstream oss;
                        oss << "No-terminal '"
                            << sym.name
                            << "' aparece en las producciones pero no tiene regla propia.";
                        r.errors.push_back(oss.str());
                    }
                }
            }
        }
    }
}
