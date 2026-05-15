#pragma once
#include "../core/grammar.h"
#include <optional>
#include <string>
#include <vector>

namespace grammar {

// Grammar file parser
// Reads grammars in the format:
//   NonTerminal -> sym1 sym2 sym3
//   NonTerminal -> alt1 | alt2 alt3
class GrammarParser {
public:
    static std::optional<Grammar> parseFile(const std::string& filename);
    static std::optional<Grammar> parseString(const std::string& content);

private:
    static std::optional<Grammar> parseLines(const std::vector<std::string>& lines);
    static std::vector<std::string> tokenize(const std::string& str);
    static void trim(std::string& s);
};

} // namespace grammar