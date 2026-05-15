#include "ast_node.h"
#include <sstream>
#include <iostream>

ASTNode::ASTNode(ASTNodeType type, const std::string& lexeme, int line, int column)
    : type(type), lexeme(lexeme), line(line), column(column) {}

ASTNode::ASTNode(ASTNodeType type, int line, int column)
    : type(type), line(line), column(column) {}

void ASTNode::addChild(std::shared_ptr<ASTNode> child) {
    children.push_back(child);
}

std::string ASTNode::typeToString() const {
    switch (type) {
        case ASTNodeType::PROGRAM: return "PROGRAM";
        case ASTNodeType::DECLARATIONS: return "DECLARATIONS";
        case ASTNodeType::DECLARATION: return "DECLARATION";
        case ASTNodeType::VARIABLE_LIST: return "VARIABLE_LIST";
        case ASTNodeType::TYPE_SPECIFIER: return "TYPE_SPECIFIER";
        case ASTNodeType::BLOCK: return "BLOCK";
        case ASTNodeType::STATEMENT_LIST: return "STATEMENT_LIST";
        case ASTNodeType::STATEMENT: return "STATEMENT";
        case ASTNodeType::ASSIGN_STATEMENT: return "ASSIGN_STATEMENT";
        case ASTNodeType::EXPRESSION: return "EXPRESSION";
        case ASTNodeType::EXPRESSION_PRIME: return "EXPRESSION_PRIME";
        case ASTNodeType::FACTOR: return "FACTOR";
        case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
        case ASTNodeType::NUMBER: return "NUMBER";
        case ASTNodeType::PLUS: return "PLUS";
        case ASTNodeType::MINUS: return "MINUS";
        case ASTNodeType::MULT: return "MULT";
        case ASTNodeType::ASSIGN: return "ASSIGN";
        case ASTNodeType::SEMICOLON: return "SEMICOLON";
        case ASTNodeType::COLON: return "COLON";
        case ASTNodeType::LPAREN: return "LPAREN";
        case ASTNodeType::RPAREN: return "RPAREN";
        case ASTNodeType::PROGRAM_KW: return "PROGRAM_KW";
        case ASTNodeType::VAR_KW: return "VAR_KW";
        case ASTNodeType::BEGIN_KW: return "BEGIN_KW";
        case ASTNodeType::END_KW: return "END_KW";
        case ASTNodeType::INT_KW: return "INT_KW";
        case ASTNodeType::FLOAT_KW: return "FLOAT_KW";
        case ASTNodeType::EPSILON: return "EPSILON";
        case ASTNodeType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string ASTNode::toString() const {
    std::ostringstream ss;
    ss << "[" << typeToString() << " '" << lexeme << "' (" << line << ":" << column << ")]";
    return ss.str();
}

// ── AST Builder Implementation ─────────────────────────────────────────────────
ASTBuilder::ASTBuilder() : root(nullptr) {}

void ASTBuilder::setRoot(std::shared_ptr<ASTNode> node) {
    root = node;
}

std::shared_ptr<ASTNode> ASTBuilder::createNode(ASTNodeType type, const std::string& lexeme, int line, int column) {
    return std::make_shared<ASTNode>(type, lexeme, line, column);
}

std::shared_ptr<ASTNode> ASTBuilder::createTerminal(ASTNodeType type, const std::string& lexeme, int line, int column) {
    return std::make_shared<ASTNode>(type, lexeme, line, column);
}

void ASTBuilder::reset() {
    root = nullptr;
}

// ── AST Visitor Implementation ───────────────────────────────────────────────
void ASTPrinter::visit(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case ASTNodeType::PROGRAM:
        case ASTNodeType::DECLARATIONS:
        case ASTNodeType::DECLARATION:
        case ASTNodeType::VARIABLE_LIST:
        case ASTNodeType::TYPE_SPECIFIER:
        case ASTNodeType::BLOCK:
        case ASTNodeType::STATEMENT_LIST:
        case ASTNodeType::STATEMENT:
        case ASTNodeType::ASSIGN_STATEMENT:
        case ASTNodeType::EXPRESSION:
        case ASTNodeType::EXPRESSION_PRIME:
        case ASTNodeType::FACTOR:
            printNode(node, depth_);
            break;
        default:
            visitTerminal(node);
            break;
    }
}

void ASTPrinter::visitProgram(ASTNode* node) { printNode(node, depth_); }
void ASTPrinter::visitDeclaration(ASTNode* node) { printNode(node, depth_); }
void ASTPrinter::visitStatement(ASTNode* node) { printNode(node, depth_); }
void ASTPrinter::visitExpression(ASTNode* node) { printNode(node, depth_); }

void ASTPrinter::visitTerminal(ASTNode* node) {
    for (int i = 0; i < depth_; ++i) output_ += "  ";
    output_ += node->toString() + "\n";
}

void ASTPrinter::printNode(ASTNode* node, int depth) {
    for (int i = 0; i < depth; ++i) output_ += "  ";
    output_ += node->typeToString();
    if (!node->lexeme.empty()) {
        output_ += " '" + node->lexeme + "'";
    }
    output_ += "\n";

    depth_ = depth + 1;
    for (auto& child : node->children) {
        visit(child.get());
    }
    depth_ = depth;
}

std::string ASTPrinter::getOutput() const {
    return output_;
}