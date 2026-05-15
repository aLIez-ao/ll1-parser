#include "follow_set.h"

FirstFollowMap FollowSetCalculator::compute(const Grammar&       g,
                                             const FirstFollowMap& firstSets) {
    FirstFollowMap followSets;

    // Initialise empty sets
    for (const auto& nt : g.nonTerminals) followSets[nt] = {};

    // Rule 1 – start symbol always has $
    followSets[g.startSymbol].insert(END_MARKER);

    // Fixed-point iteration
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& [lhs, prods] : g.rules) {
            for (const auto& prod : prods) {
                for (size_t i = 0; i < prod.size(); ++i) {
                    const Symbol& sym = prod[i];

                    // Only process non-terminals (and skip ε symbols)
                    if (!sym.isNT || sym.isEpsilon()) continue;

                    auto before = followSets[sym.name].size();

                    // β = everything after position i
                    std::vector<Symbol> beta(prod.begin() + i + 1, prod.end());

                    if (beta.empty()) {
                        // Rule 4 – A -> αB: FOLLOW(A) ⊆ FOLLOW(B)
                        for (const auto& s : followSets[lhs])
                            followSets[sym.name].insert(s);
                    } else {
                        // Rule 2 – add FIRST(β) - {ε} to FOLLOW(B)
                        StringSet fb = FirstSetCalculator::firstOfSequence(beta, firstSets);

                        for (const auto& s : fb) {
                            if (s != EPSILON_KW)
                                followSets[sym.name].insert(s);
                        }

                        // Rule 3 – if ε ∈ FIRST(β), add FOLLOW(A) to FOLLOW(B)
                        if (fb.count(EPSILON_KW)) {
                            for (const auto& s : followSets[lhs])
                                followSets[sym.name].insert(s);
                        }
                    }

                    if (followSets[sym.name].size() != before) changed = true;
                }
            }
        }
    }

    return followSets;
}
