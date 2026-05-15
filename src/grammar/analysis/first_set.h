#pragma once
#include "../core/grammar.h"

// ── FirstSetCalculator ───────────────────────────────────────────────────────
// Computes FIRST sets for all non-terminals in a grammar using a
// fixed-point (stability) iteration.
//
// FIRST(X):
//   • X is terminal      → { X }
//   • X is EPS           → { EPS }
//   • X is non-terminal  → union of FIRST of each of its productions,
//                          propagating ε through sequences.
// ─────────────────────────────────────────────────────────────────────────────
class FirstSetCalculator {
public:
    // Compute FIRST for the entire grammar
    static FirstFollowMap compute(const Grammar& g);

    // Compute FIRST of an arbitrary sequence of symbols.
    // Useful when building FOLLOW sets.
    static StringSet firstOfSequence(const std::vector<Symbol>& seq,
                                     const FirstFollowMap&      firstSets);
};
