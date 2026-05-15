#pragma once

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <set>

#include "../lexer/token/token.h"
#include "../grammar/analysis/ll1_table.h"
#include "ast/ast_node.h"
#include "trace/trace_manager.h"
#include "symbol_manager.h"

class LL1Parser {
public:
    LL1Parser();
    ~LL1Parser();

    void setTraceLevel(TraceLevel level);
    void setTraceFile(const std::string& filename);
    void enableASTBuilding(bool enable);

    struct Result {
        bool success = false;
        std::shared_ptr<ASTNode> ast;
        std::vector<std::string> errors;
        int errorCount = 0;
    };

    Result parse(const std::vector<Token>& tokens,
                 const LL1Table& ll1Table,
                 const std::string& startSymbol,
                 const std::vector<std::string>& nonTerminals);

    std::shared_ptr<ASTNode> getAST() const;
    const TraceManager& getTraceManager() const;
    const SymbolManager& getSymbolManager() const;

private:
    std::stack<std::string> stack_;
    std::vector<Token> tokens_;
    size_t currentTokenIndex_ = 0;
    LL1Table ll1Table_;
    std::string startSymbol_;
    std::set<std::string> nonTerminals_;
    Result* currentResult_ = nullptr;

    ASTBuilder astBuilder_;
    TraceManager traceManager_;
    SymbolManager symbolManager_;
    bool buildAST_ = true;

    void init(const std::vector<Token>& tokens, const LL1Table& table, const std::string& start, const std::vector<std::string>& nonTerminals);
    bool isTerminal(const std::string& sym) const;
    bool isNonTerminal(const std::string& sym) const;
    std::string currentSymbol() const;
    bool hasMoreTokens() const;
    void nextToken();
    void matchTerminal(const std::string& term);
    void expandNonTerminal(const std::string& nt);
    void error(const std::string& expected, const std::string& got);
};