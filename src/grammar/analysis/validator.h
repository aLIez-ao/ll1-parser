#pragma once
#include "../core/grammar.h"
#include <vector>
#include <string>

// ── ValidationResult ─────────────────────────────────────────────────────────
struct ValidationResult {
    bool                     valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// ── Validator ────────────────────────────────────────────────────────────────
// Checks:
//   1. Direct left recursion  (A -> A ...)
//   2. Undefined non-terminals (NT used in RHS but has no production)
// ─────────────────────────────────────────────────────────────────────────────
class Validator {
public:
    static ValidationResult validate(const Grammar& g);

private:
    static void checkLeftRecursion   (const Grammar& g, ValidationResult& r);
    static void checkUndefinedSymbols(const Grammar& g, ValidationResult& r);
};
