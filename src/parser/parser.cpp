#include "parser.h"
#include "ast/ast_node.h"
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
                                      const std::string& startSymbol,
                                      const std::vector<std::string>& nonTerminals) {
    Result result;
    currentResult_ = &result;
    init(tokens, ll1Table, startSymbol, nonTerminals);

    traceManager_.logInitialization(startSymbol, static_cast<int>(tokens.size()));

    // Helper: convierte la pila a string (fondo→tope, separado por espacios)
    // Copia la pila para no modificarla
    auto stackToStr = [](std::stack<std::string> s) -> std::string {
        std::vector<std::string> v;
        while (!s.empty()) { v.push_back(s.top()); s.pop(); }
        std::string result;
        for (auto it = v.rbegin(); it != v.rend(); ++it) {
            if (*it == "$$AST_POP$$") continue;  // ocultar marcadores internos
            if (!result.empty()) result += " ";
            result += *it;
        }
        return result;
    };

    int step = 0;
    bool parsingComplete = false;
    while (!stack_.empty()) {
        step++;
        std::string top = stack_.top();
        std::string cur = currentSymbol();

        // Procesar marcador de cierre de nodo AST
        if (top == AST_POP_MARKER) {
            stack_.pop();
            if (buildAST_ && astStack_.size() > 1) {
                astStack_.pop();
            }
            step--; // No contar como paso de análisis
            continue;
        }

        traceManager_.logStep(step, stackToStr(stack_), cur);

        if (top == "$") {
            if (cur == "$") {
                traceManager_.logSuccess(step, "Analisis exitoso");
                stack_.pop();
                parsingComplete = true;
                break;
            } else {
                const auto& tk = tokens_[currentTokenIndex_];
                std::string msg = "Error sintáctico (línea " + std::to_string(tk.line) +
                                  ", col " + std::to_string(tk.column) +
                                  "): se esperaba fin de entrada ($), se recibió " +
                                  cur + "('" + tk.lexeme + "')";
                error("$", cur);
                result.errors.push_back(msg);
                result.errorCount++;
                break;
            }
        }
        else if (isTerminal(top)) {
            if (top == cur) {
                traceManager_.logMatch(step, top, tokens_[currentTokenIndex_].lexeme);

                if (buildAST_ && !astStack_.empty()) {
                    ASTNodeType terminalType = ASTNodeType::UNKNOWN;
                    if (top == "ID") terminalType = ASTNodeType::IDENTIFIER;
                    else if (top == "NUM" || top == "FNUM") terminalType = ASTNodeType::NUMBER;
                    else if (top == "PLUS") terminalType = ASTNodeType::PLUS;
                    else if (top == "MINUS") terminalType = ASTNodeType::MINUS;
                    else if (top == "MULT") terminalType = ASTNodeType::MULT;
                    else if (top == "ASSIGN") terminalType = ASTNodeType::ASSIGN;
                    else if (top == "SEMICOLON") terminalType = ASTNodeType::SEMICOLON;
                    else if (top == "COLON") terminalType = ASTNodeType::COLON;
                    else if (top == "PROGRAM") terminalType = ASTNodeType::PROGRAM_KW;
                    else if (top == "VAR") terminalType = ASTNodeType::VAR_KW;
                    else if (top == "BEGIN") terminalType = ASTNodeType::BEGIN_KW;
                    else if (top == "END") terminalType = ASTNodeType::END_KW;
                    else if (top == "INT") terminalType = ASTNodeType::INT_KW;
                    else if (top == "FLOAT") terminalType = ASTNodeType::FLOAT_KW;

                    auto terminalNode = std::make_shared<ASTNode>(
                        terminalType,
                        tokens_[currentTokenIndex_].lexeme,
                        tokens_[currentTokenIndex_].line,
                        tokens_[currentTokenIndex_].column
                    );
                    astStack_.top()->addChild(terminalNode);
                }

                stack_.pop();
                nextToken();
            } else {
                const auto& tk = tokens_[currentTokenIndex_];
                std::string msg = "Error sintáctico (línea " + std::to_string(tk.line) +
                                  ", col " + std::to_string(tk.column) +
                                  "): se esperaba " + top +
                                  ", se recibió " + cur + "('" + tk.lexeme + "')";
                error(top, cur);
                stack_.pop();
                result.errors.push_back(msg);
                result.errorCount++;
            }
        }
        else if (isNonTerminal(top)) {
            expandNonTerminal(top);
        }
        else {
            stack_.pop();
        }
    }

    result.success = parsingComplete && result.errorCount == 0;
    if (!parsingComplete && result.errorCount == 0) {
        // Llegó a EOF antes de consumir toda la gramática
        result.errors.push_back("Error sintáctico: análisis incompleto — entrada terminó inesperadamente");
        result.errorCount++;
    }

    if (buildAST_ && result.success) {
        // El centinela debe quedar en el fondo del stack con el árbol como hijo
        while (astStack_.size() > 1) astStack_.pop();
        if (!astStack_.empty() && !astStack_.top()->children.empty()) {
            astBuilder_.root = astStack_.top()->children[0];
        }
        result.ast = astBuilder_.root;
    }

    currentResult_ = nullptr;

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

void LL1Parser::init(const std::vector<Token>& tokens, const LL1Table& table, const std::string& start, const std::vector<std::string>& nonTerminals) {
    while (!stack_.empty()) stack_.pop();
    while (!astStack_.empty()) astStack_.pop();
    tokens_ = tokens;
    currentTokenIndex_ = 0;
    ll1Table_ = table;
    startSymbol_ = start;

    nonTerminals_.clear();
    for (const auto& nt : nonTerminals) {
        nonTerminals_.insert(nt);
    }

    stack_.push("$");
    stack_.push(start);

    if (buildAST_) {
        astBuilder_.reset();
        // Nodo centinela: su primer hijo será la raíz real del AST
        auto sentinel = std::make_shared<ASTNode>(ASTNodeType::UNKNOWN, "__sentinel__", 0, 0);
        astStack_.push(sentinel);
    }

    symbolManager_ = SymbolManager();
}

bool LL1Parser::isTerminal(const std::string& sym) const {
    if (sym == "$" || sym == "EPS" || sym.empty()) return false;
    return nonTerminals_.count(sym) == 0;
}

bool LL1Parser::isNonTerminal(const std::string& sym) const {
    return nonTerminals_.count(sym) > 0;
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
        int errLine   = hasMoreTokens() ? tokens_[currentTokenIndex_].line   : 0;
        int errCol    = hasMoreTokens() ? tokens_[currentTokenIndex_].column : 0;
        std::string lex = hasMoreTokens() ? tokens_[currentTokenIndex_].lexeme : "";
        traceManager_.logError(static_cast<int>(currentTokenIndex_), nt, cur, errLine);
        stack_.pop();
        if (currentResult_) {
            std::string msg = "Error sintáctico (línea " + std::to_string(errLine) +
                              ", col " + std::to_string(errCol) +
                              "): token inesperado " + cur + "('" + lex + "')" +
                              " al expandir no-terminal " + nt;
            currentResult_->errors.push_back(msg);
            currentResult_->errorCount++;
        }
        return;
    }

    const auto& prod = prodIt->second;
    traceManager_.logExpand(static_cast<int>(currentTokenIndex_), nt, prod.productionStr);

    if (buildAST_ && !astStack_.empty()) {
        ASTNodeType nodeType = ASTNodeType::UNKNOWN;
        if (nt == "P") nodeType = ASTNodeType::PROGRAM;
        else if (nt == "D")  nodeType = ASTNodeType::DECLARATIONS;
        else if (nt == "L")  nodeType = ASTNodeType::DECLARATION;      // una var declarada
        else if (nt == "L2") nodeType = ASTNodeType::VARIABLE_LIST;    // continuación lista
        else if (nt == "T")  nodeType = ASTNodeType::TYPE_SPECIFIER;
        else if (nt == "B")  nodeType = ASTNodeType::BLOCK;
        else if (nt == "S")  nodeType = ASTNodeType::STATEMENT;        // sentencia asignación
        else if (nt == "S2") nodeType = ASTNodeType::STATEMENT_LIST;   // continuación
        else if (nt == "E")  nodeType = ASTNodeType::EXPRESSION;
        else if (nt == "E2") nodeType = ASTNodeType::EXPRESSION_PRIME;
        else if (nt == "F")  nodeType = ASTNodeType::FACTOR;

        auto newNode = std::make_shared<ASTNode>(nodeType, nt, 0, 0);
        astStack_.top()->addChild(newNode);
        astStack_.push(newNode);
    }

    stack_.pop();

    const bool isEpsilonProd = prod.production.empty() ||
        (prod.production.size() == 1 && prod.production[0].isEpsilon());

    if (!isEpsilonProd) {
        // Insertar AST_POP al fondo de esta expansión (se ejecutará último)
        if (buildAST_) stack_.push(AST_POP_MARKER);

        for (int i = static_cast<int>(prod.production.size()) - 1; i >= 0; i--) {
            stack_.push(prod.production[i].name);
        }
    } else {
        // Producción epsilon: cerrar el nodo inmediatamente
        if (buildAST_ && astStack_.size() > 1) astStack_.pop();
    }
}

void LL1Parser::error(const std::string& expected, const std::string& got) {
    int line = hasMoreTokens() ? tokens_[currentTokenIndex_].line : 0;
    traceManager_.logError(static_cast<int>(currentTokenIndex_), expected, got, line);
}