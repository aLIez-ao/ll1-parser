#include "parser.h"
#include <iostream>

LL1Parser::LL1Parser() = default;
LL1Parser::~LL1Parser() = default;

void LL1Parser::setTraceLevel(TraceLevel level) {
    traceManager_.setLevel(level);
}

void LL1Parser::setTraceFile(const std::string& filename) {
    traceManager_.setOutputFile(filename);
}

void LL1Parser::enableASTBuilding(bool enable) {
    buildAST_ = enable;
}

LL1Parser::Result LL1Parser::parse(const std::vector<Token>& tokens,
                                     const LL1Table& ll1Table,
                                     const std::string& startSymbol) {
    Result result;
    init(tokens, ll1Table, startSymbol);

    traceManager_.logInitialization(startSymbol, static_cast<int>(tokens.size()));

    int step = 0;
    while (!stack_.empty() && hasMoreTokens()) {
        step++;
        std::string top = stack_.top();
        std::string cur = currentSymbol();

        traceManager_.logStep(step, top, cur);

        if (isTerminal(top)) {
            if (top == cur) {
                traceManager_.logMatch(step, top, tokens_[currentTokenIndex_].lexeme);
                stack_.pop();
                nextToken();
            } else {
                error(top, cur);
                stack_.pop();
                result.errors.push_back("Error: esperado " + top + ", recibido " + cur);
                result.errorCount++;
            }
        }
        else if (isNonTerminal(top)) {
            expandNonTerminal(top);
        }
        else if (top == "$") {
            if (cur == "$") {
                traceManager_.logSuccess(step, "Analisis exitoso");
                result.success = true;
                break;
            } else {
                error("$", cur);
                break;
            }
        }
        else {
            stack_.pop();
        }
    }

    if (!result.success && result.errorCount == 0) {
        result.success = true;
    }

    traceManager_.flush();
    return result;
}

std::shared_ptr<ASTNode> LL1Parser::getAST() const {
    return astBuilder_.root;
}

const TraceManager& LL1Parser::getTraceManager() const {
    return traceManager_;
}

const SymbolManager& LL1Parser::getSymbolManager() const {
    return symbolManager_;
}

void LL1Parser::init(const std::vector<Token>& tokens, const LL1Table& table, const std::string& start) {
    while (!stack_.empty()) stack_.pop();
    tokens_ = tokens;
    currentTokenIndex_ = 0;
    ll1Table_ = table;
    startSymbol_ = start;
    stack_.push("$");
    stack_.push(start);
    astBuilder_.reset();
    symbolManager_ = SymbolManager();
}

bool LL1Parser::isTerminal(const std::string& sym) const {
    if (sym == "$" || sym == "EPS" || sym.empty()) return false;
    return sym[0] >= 'A' && sym[0] <= 'Z';
}

bool LL1Parser::isNonTerminal(const std::string& sym) const {
    return ll1Table_.count(sym) > 0;
}

std::string LL1Parser::currentSymbol() const {
    if (!hasMoreTokens()) return "$";
    return tokens_[currentTokenIndex_].toParserSymbol();
}

bool LL1Parser::hasMoreTokens() const {
    return currentTokenIndex_ < tokens_.size();
}

void LL1Parser::nextToken() {
    if (hasMoreTokens()) currentTokenIndex_++;
}

void LL1Parser::matchTerminal(const std::string& term) {
    if (currentSymbol() == term) {
        nextToken();
    }
}

void LL1Parser::expandNonTerminal(const std::string& nt) {
    auto it = ll1Table_.find(nt);
    if (it == ll1Table_.end()) {
        stack_.pop();
        return;
    }

    const auto& cell = it->second;
    std::string cur = currentSymbol();
    auto prodIt = cell.find(cur);

    if (prodIt == cell.end() || !prodIt->second.isValid) {
        traceManager_.logError(static_cast<int>(currentTokenIndex_), nt, cur, 
            hasMoreTokens() ? tokens_[currentTokenIndex_].line : 0);
        stack_.pop();
        return;
    }

    const auto& prod = prodIt->second;
    traceManager_.logExpand(static_cast<int>(currentTokenIndex_), nt, prod.productionStr);

    stack_.pop();

    if (!prod.production.empty() && !(prod.production.size() == 1 && prod.production[0].isEpsilon())) {
        for (int i = static_cast<int>(prod.production.size()) - 1; i >= 0; i--) {
            stack_.push(prod.production[i].name);
        }
    }
}

void LL1Parser::error(const std::string& expected, const std::string& got) {
    int line = hasMoreTokens() ? tokens_[currentTokenIndex_].line : 0;
    traceManager_.logError(static_cast<int>(currentTokenIndex_), expected, got, line);
}