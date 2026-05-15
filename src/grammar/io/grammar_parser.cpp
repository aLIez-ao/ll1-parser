#include "grammar_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace grammar;

void GrammarParser::trim(std::string& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         [](unsigned char c){ return !std::isspace(c); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char c){ return !std::isspace(c); }).base(),
            s.end());
}

std::vector<std::string> GrammarParser::tokenize(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

std::optional<Grammar> GrammarParser::parseLines(const std::vector<std::string>& lines) {
    struct RawRule {
        std::string lhs;
        std::vector<std::vector<std::string>> prods;
    };

    std::vector<RawRule> rawRules;
    StringSet ntSet;
    std::vector<std::string> ntOrder;

    for (const auto& rawLine : lines) {
        std::string line = rawLine;
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto arrowPos = line.find("->");
        if (arrowPos == std::string::npos) {
            std::cerr << "[Error] Formato invalido (falta '->'): " << line << "\n";
            return std::nullopt;
        }

        std::string lhs = line.substr(0, arrowPos);
        trim(lhs);
        if (lhs.empty()) {
            std::cerr << "[Error] LHS vacio en: " << line << "\n";
            return std::nullopt;
        }

        RawRule* existing = nullptr;
        for (auto& r : rawRules) {
            if (r.lhs == lhs) { existing = &r; break; }
        }

        if (!existing) {
            ntOrder.push_back(lhs);
            ntSet.insert(lhs);
            rawRules.push_back({lhs, {}});
            existing = &rawRules.back();
        }

        std::string rhs = line.substr(arrowPos + 2);
        std::string cur;
        for (char c : rhs) {
            if (c == '|') {
                trim(cur);
                if (!cur.empty()) {
                    auto toks = tokenize(cur);
                    if (!toks.empty()) existing->prods.push_back(toks);
                }
                cur.clear();
            } else {
                cur += c;
            }
        }
        trim(cur);
        if (!cur.empty()) {
            auto toks = tokenize(cur);
            if (!toks.empty()) existing->prods.push_back(toks);
        }
    }

    if (rawRules.empty()) {
        std::cerr << "[Error] No se encontraron reglas.\n";
        return std::nullopt;
    }

    Grammar g;
    g.nonTerminals = ntSet;
    g.ntOrder = ntOrder;
    g.startSymbol = ntOrder[0];

    for (const auto& rule : rawRules) {
        for (const auto& prod : rule.prods) {
            for (const auto& sym : prod) {
                if (ntSet.count(sym) == 0 &&
                    sym != EPSILON_KW && sym != NULL_KW) {
                    g.terminals.insert(sym);
                }
            }
        }
    }

    for (const auto& rule : rawRules) {
        for (const auto& prod : rule.prods) {
            Production typed;
            for (const auto& sym : prod) typed.push_back(g.makeSymbol(sym));
            g.rules[rule.lhs].push_back(typed);
        }
    }

    return g;
}

std::optional<Grammar> GrammarParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Error] No se pudo abrir el archivo: " << filename << "\n";
        return std::nullopt;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    return parseLines(lines);
}

std::optional<Grammar> GrammarParser::parseString(const std::string& content) {
    std::istringstream iss(content);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(iss, line)) lines.push_back(line);
    return parseLines(lines);
}