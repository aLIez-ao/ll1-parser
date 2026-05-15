#pragma once
#include "../core/grammar.h"
#include "first_set.h"
#include "follow_set.h"
#include <map>
#include <set>
#include <vector>
#include <optional>
#include <string>

// ── LL1TableEntry ─────────────────────────────────────────────────────────────
// Represents a single entry in the LL(1) parsing table
// ─────────────────────────────────────────────────────────────────────────────
struct LL1TableEntry {
    bool isValid;                    // true if entry is valid
    Production production;           // the production to apply
    std::string productionStr;      // string representation for debugging

    LL1TableEntry() : isValid(false) {}
    LL1TableEntry(const Production& prod, const std::string& prodStr)
        : isValid(true), production(prod), productionStr(prodStr) {}
};

// ── LL1Table ───────────────────────────────────────────────────────────────────
// The complete LL(1) parsing table: [NonTerminal][Terminal] -> Production
// ─────────────────────────────────────────────────────────────────────────────
using LL1Table = std::map<std::string, std::map<std::string, LL1TableEntry>>;

// ── LL1TableGenerator ───────────────────────────────────────────────────────────
// Builds LL(1) parsing table from Grammar, FIRST and FOLLOW sets
// Detects conflicts if grammar is not LL(1)
// ─────────────────────────────────────────────────────────────────────────────
class LL1TableGenerator {
public:
    struct Result {
        bool isValid;                // true if grammar is LL(1)
        LL1Table table;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };

    static Result build(const Grammar& grammar,
                        const FirstFollowMap& firstSets,
                        const FirstFollowMap& followSets);

private:
    static void addProductionsForSymbol(const std::string& nonTerminal,
                                         const Production& production,
                                         const StringSet& lookahead,
                                         LL1Table& table,
                                         std::vector<std::string>& conflicts);
};