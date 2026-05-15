#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

// ── Reserved keywords ────────────────────────────────────────────────────────
inline const std::string EPSILON_KW  = "EPS";   // epsilon keyword
inline const std::string NULL_KW     = "null";  // alternate epsilon keyword
inline const std::string END_MARKER  = "$";     // end-of-string marker

// ── Symbol ───────────────────────────────────────────────────────────────────
struct Symbol {
    std::string name;
    bool        isNT;   // true  → Non-Terminal
                        // false → Terminal | Epsilon

    bool isEpsilon()   const { return name == EPSILON_KW || name == NULL_KW; }
    bool isEndMarker() const { return name == END_MARKER; }

    bool operator==(const Symbol& o) const { return name == o.name; }
    bool operator< (const Symbol& o) const { return name  < o.name; }
};

// ── Type aliases ─────────────────────────────────────────────────────────────
using Production     = std::vector<Symbol>;
using RuleMap        = std::map<std::string, std::vector<Production>>;
using StringSet      = std::set<std::string>;
using FirstFollowMap = std::map<std::string, StringSet>;

// ── Grammar ──────────────────────────────────────────────────────────────────
struct Grammar {
    std::string              startSymbol;
    std::vector<std::string> ntOrder;      // Insertion order (for display)
    StringSet                nonTerminals;
    StringSet                terminals;
    RuleMap                  rules;

    // Build a correctly-typed Symbol (looks up whether name is NT or T)
    Symbol makeSymbol(const std::string& name) const;
};
