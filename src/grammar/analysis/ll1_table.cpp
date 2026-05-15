#include "ll1_table.h"
#include "../core/grammar.h"
#include <sstream>

LL1TableGenerator::Result LL1TableGenerator::build(
    const Grammar& grammar,
    const FirstFollowMap& firstSets,
    const FirstFollowMap& followSets)
{
    Result result;
    result.isValid = true;

    // Initialize table for each non-terminal
    for (const auto& nt : grammar.nonTerminals) {
        result.table[nt] = {};
    }

    // Build table entries for each production
    for (const auto& [nonTerminal, productions] : grammar.rules) {
        for (const auto& production : productions) {
            // Calculate FIRST of the production
            StringSet firstOfProd = FirstSetCalculator::firstOfSequence(
                production, firstSets);

            // For each terminal in FIRST(production) except epsilon
            for (const auto& terminal : firstOfProd) {
                if (terminal == EPSILON_KW) continue;

                addProductionsForSymbol(nonTerminal, production, {terminal},
                                        result.table, result.errors);
            }

            // If epsilon is in FIRST(production), use FOLLOW(nonTerminal)
            if (firstOfProd.count(EPSILON_KW) > 0) {
                auto it = followSets.find(nonTerminal);
                if (it != followSets.end()) {
                    addProductionsForSymbol(nonTerminal, production,
                                            it->second, result.table, result.errors);
                }
            }
        }
    }

    // Check for conflicts
    if (!result.errors.empty()) {
        result.isValid = false;
        std::stringstream ss;
        ss << "Grammar has " << result.errors.size() << " conflict(s):\n";
        for (const auto& err : result.errors) {
            ss << "  - " << err << "\n";
        }
        result.errors.push_back(ss.str());
    }

    return result;
}

void LL1TableGenerator::addProductionsForSymbol(
    const std::string& nonTerminal,
    const Production& production,
    const StringSet& lookahead,
    LL1Table& table,
    std::vector<std::string>& conflicts)
{
    // Build string representation of production
    std::string prodStr;
    for (size_t i = 0; i < production.size(); ++i) {
        if (i > 0) prodStr += " ";
        prodStr += production[i].name;
    }
    if (production.empty() || (production.size() == 1 && production[0].isEpsilon())) {
        prodStr = "EPS";
    }

    LL1TableEntry entry(production, prodStr);

    // Add entry for each lookahead symbol
    for (const auto& symbol : lookahead) {
        auto& cell = table[nonTerminal][symbol];

        // Check for conflict
        if (cell.isValid && cell.productionStr != prodStr) {
            std::stringstream ss;
            ss << "Conflict at [" << nonTerminal << ", " << symbol << "]: "
               << cell.productionStr << " vs " << prodStr;
            conflicts.push_back(ss.str());
        } else {
            cell = entry;
        }
    }
}