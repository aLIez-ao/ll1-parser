#pragma once
#include "../core/grammar.h"
#include "first_set.h"

// ── FollowSetCalculator ──────────────────────────────────────────────────────
// Computes FOLLOW sets for all non-terminals using fixed-point iteration.
//
// Rules applied:
//   1. $ ∈ FOLLOW(S)               – start symbol always has $
//   2. A -> αBβ: FIRST(β)-{ε} ⊆ FOLLOW(B)
//   3. A -> αBβ, ε∈FIRST(β): FOLLOW(A) ⊆ FOLLOW(B)
//   4. A -> αB:  FOLLOW(A) ⊆ FOLLOW(B)
// ─────────────────────────────────────────────────────────────────────────────
class FollowSetCalculator {
public:
    static FirstFollowMap compute(const Grammar&       g,
                                  const FirstFollowMap& firstSets);
};
