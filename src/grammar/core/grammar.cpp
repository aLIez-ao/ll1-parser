#include "grammar.h"

Symbol Grammar::makeSymbol(const std::string& name) const {
    Symbol s;
    s.name = name;
    // Epsilon variants are treated as terminals
    if (name == EPSILON_KW || name == NULL_KW) {
        s.name = EPSILON_KW;  // normalise to canonical form
        s.isNT = false;
    } else {
        s.isNT = (nonTerminals.count(name) > 0);
    }
    return s;
}
