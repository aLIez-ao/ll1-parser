#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

// Forward declaration
namespace lexer {
    struct Token;
}

// ── Symbol Entry for Parser ───────────────────────────────────────────────────
struct ParserSymbolEntry {
    std::string name;
    std::string type;
    int line;
    int column;
    bool isInitialized;

    ParserSymbolEntry() : name(""), type(""), line(0), column(0), isInitialized(false) {}

    ParserSymbolEntry(const std::string& n, const std::string& t, int l, int c)
        : name(n), type(t), line(l), column(c), isInitialized(false) {}
};

// ── Symbol Manager ─────────────────────────────────────────────────────────────
// Manages symbol table for the parser: tracks declarations and usages
// Validates duplicate declarations and ensures variables are declared before use
// ─────────────────────────────────────────────────────────────────────────────
class SymbolManager {
public:
    SymbolManager();

    // Declaration management
    bool declare(const std::string& name, const std::string& type, int line, int column);
    bool isDeclared(const std::string& name) const;
    const ParserSymbolEntry* lookup(const std::string& name) const;

    // Usage tracking
    void markInitialized(const std::string& name);
    bool isInitialized(const std::string& name) const;

    // Validation
    bool hasDuplicateDeclaration(const std::string& name) const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;

    // Output
    void printTable(std::ostream& out) const;
    std::string toString() const;

    // Scope management (for future extension)
    void pushScope();
    void popScope();

    // Count
    int getSymbolCount() const;

private:
    struct Scope {
        std::map<std::string, ParserSymbolEntry> symbols;
    };

    std::vector<Scope> scopes_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;

    Scope& currentScope();
    const Scope& currentScope() const;
};

// ── Parser Result ───────────────────────────────────────────────────────────────
struct ParseResult {
    bool success;
    std::shared_ptr<class ASTNode> ast;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    int errorCount;
    int warningCount;

    ParseResult()
        : success(false), errorCount(0), warningCount(0) {}
};