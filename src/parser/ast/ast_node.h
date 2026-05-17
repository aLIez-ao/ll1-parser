#pragma once

#include <string>
#include <vector>
#include <memory>

// ── AST Node Types ───────────────────────────────────────────────────────────────
enum class ASTNodeType {
    // Program structure
    PROGRAM,
    DECLARATIONS,
    DECLARATION,
    VARIABLE_LIST,
    TYPE_SPECIFIER,
    BLOCK,

    // Statements
    STATEMENT_LIST,
    STATEMENT,
    ASSIGN_STATEMENT,

    // Expressions
    EXPRESSION,
    EXPRESSION_PRIME,
    FACTOR,

    // Terminals (for leaf nodes)
    IDENTIFIER,
    NUMBER,
    PLUS,
    MINUS,
    MULT,
    ASSIGN,
    SEMICOLON,
    COLON,
    LPAREN,
    RPAREN,
    PROGRAM_KW,
    VAR_KW,
    BEGIN_KW,
    END_KW,
    INT_KW,
    FLOAT_KW,

    // Special
    EPSILON,
    UNKNOWN
};

// ── AST Node ───────────────────────────────────────────────────────────────────
class ASTNode {
public:
    ASTNodeType type;
    std::string lexeme;
    int line;
    int column;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType type, const std::string& lexeme = "", int line = 0, int column = 0);
    ASTNode(ASTNodeType type, int line, int column);

    void addChild(std::shared_ptr<ASTNode> child);
    std::string toString() const;
    std::string typeToString() const;
};

// ── AST Builder ───────────────────────────────────────────────────────────────
// Builds AST during parsing
// ─────────────────────────────────────────────────────────────────────────────
class ASTBuilder {
public:
    std::shared_ptr<ASTNode> root;

    ASTBuilder();

    void setRoot(std::shared_ptr<ASTNode> node);
    std::shared_ptr<ASTNode> createNode(ASTNodeType type, const std::string& lexeme, int line, int column);
    std::shared_ptr<ASTNode> createTerminal(ASTNodeType type, const std::string& lexeme, int line, int column);
    void reset();
};

// ── AST Visitor ───────────────────────────────────────────────────────────────
// Visitor pattern for AST traversal
// ─────────────────────────────────────────────────────────────────────────────
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ASTNode* node) = 0;
    virtual void visitProgram(ASTNode* node) = 0;
    virtual void visitDeclaration(ASTNode* node) = 0;
    virtual void visitStatement(ASTNode* node) = 0;
    virtual void visitExpression(ASTNode* node) = 0;
    virtual void visitTerminal(ASTNode* node) = 0;
};

class ASTPrinter : public ASTVisitor {
public:
    void visit(ASTNode* node) override;
    void visitProgram(ASTNode* node) override;
    void visitDeclaration(ASTNode* node) override;
    void visitStatement(ASTNode* node) override;
    void visitExpression(ASTNode* node) override;
    void visitTerminal(ASTNode* node) override;

    std::string getOutput() const;

private:
    void printNode(ASTNode* node, const std::string& prefix, bool isLast);
    std::string output_;
};