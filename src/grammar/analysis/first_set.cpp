#include "first_set.h"

// ── FIRST of a sequence ──────────────────────────────────────────────────────
// Used both internally and by the FOLLOW calculator.
StringSet FirstSetCalculator::firstOfSequence(const std::vector<Symbol>& seq,
                                               const FirstFollowMap&      firstSets) {
    StringSet result;

    if (seq.empty()) {
        result.insert(EPSILON_KW);
        return result;
    }

    bool allCanBeEpsilon = true;

    for (const auto& sym : seq) {
        // ε symbol in the sequence → whole sequence can derive ε so far
        if (sym.isEpsilon()) {
            result.insert(EPSILON_KW);
            break;
        }

        // Terminal: add it and stop — the rest of the sequence is unreachable
        // if this terminal is present
        if (!sym.isNT) {
            result.insert(sym.name);
            allCanBeEpsilon = false;
            break;
        }

        // Non-terminal: add its FIRST minus ε; if it cannot derive ε, stop
        auto it = firstSets.find(sym.name);
        if (it != firstSets.end()) {
            for (const auto& s : it->second) {
                if (s != EPSILON_KW) result.insert(s);
            }
            if (it->second.count(EPSILON_KW) == 0) {
                allCanBeEpsilon = false;
                break;
            }
        } else {
            // NT not yet computed — assume it cannot derive ε
            allCanBeEpsilon = false;
            break;
        }
    }

    if (allCanBeEpsilon) result.insert(EPSILON_KW);
    return result;
}

// ── Main compute (fixed-point) ───────────────────────────────────────────────
FirstFollowMap FirstSetCalculator::compute(const Grammar& g) {
    FirstFollowMap firstSets;

    // Initialise empty sets for every NT
    for (const auto& nt : g.nonTerminals) firstSets[nt] = {};

    // Iterate until stable
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& [nt, prods] : g.rules) {
            for (const auto& prod : prods) {
                auto before = firstSets[nt].size();

                StringSet toAdd = firstOfSequence(prod, firstSets);
                for (const auto& s : toAdd) firstSets[nt].insert(s);

                if (firstSets[nt].size() != before) changed = true;
            }
        }
    }

    return firstSets;
}
