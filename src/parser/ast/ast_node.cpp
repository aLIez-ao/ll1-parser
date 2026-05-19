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

// ── ANSI color helpers (solo para el printer) ─────────────────────────────────
namespace ASTColor {
    static const char* RESET  = "\033[0m";
    static const char* NT     = "\033[1;36m";   // cyan bold  → no-terminal
    static const char* ID     = "\033[1;33m";   // yellow     → identificador
    static const char* NUM    = "\033[1;35m";   // magenta    → número
    static const char* KW     = "\033[1;34m";   // blue       → palabra clave
    static const char* OP     = "\033[1;32m";   // green      → operador
}

// ── AST Visitor Implementation ───────────────────────────────────────────────
void ASTPrinter::visit(ASTNode* node) {
    if (!node) return;
    printNode(node, "", true);
}

void ASTPrinter::visitProgram   (ASTNode* n) { printNode(n, "", true); }
void ASTPrinter::visitDeclaration(ASTNode* n) { printNode(n, "", true); }
void ASTPrinter::visitStatement (ASTNode* n) { printNode(n, "", true); }
void ASTPrinter::visitExpression(ASTNode* n) { printNode(n, "", true); }
void ASTPrinter::visitTerminal  (ASTNode* n) { printNode(n, "", true); }

// ── Etiqueta visible de un nodo ───────────────────────────────────────────────
static std::string nodeLabel(ASTNode* node) {
    using T = ASTNodeType;
    std::string color, label;

    switch (node->type) {
        // No-terminales
        case T::PROGRAM:        color = ASTColor::NT; label = "PROGRAMA";       break;
        case T::DECLARATIONS:   color = ASTColor::NT; label = "DECLARACIONES";  break;
        case T::DECLARATION:    color = ASTColor::NT; label = "DECLARACION";    break;
        case T::VARIABLE_LIST:  color = ASTColor::NT; label = "LISTA_VAR";      break;
        case T::TYPE_SPECIFIER: color = ASTColor::NT; label = "TIPO";           break;
        case T::BLOCK:          color = ASTColor::NT; label = "BLOQUE";         break;
        case T::STATEMENT_LIST: color = ASTColor::NT; label = "SENTENCIAS";     break;
        case T::STATEMENT:      color = ASTColor::NT; label = "SENTENCIA";      break;
        case T::ASSIGN_STATEMENT: color = ASTColor::NT; label = "ASIGNACION";   break;
        case T::EXPRESSION:     color = ASTColor::NT; label = "EXPRESION";      break;
        case T::EXPRESSION_PRIME: color = ASTColor::NT; label = "EXPR_CONT";    break;
        case T::FACTOR:         color = ASTColor::NT; label = "FACTOR";         break;

        // Palabras clave
        case T::PROGRAM_KW: color = ASTColor::KW; label = "program";  break;
        case T::VAR_KW:     color = ASTColor::KW; label = "var";      break;
        case T::BEGIN_KW:   color = ASTColor::KW; label = "begin";    break;
        case T::END_KW:     color = ASTColor::KW; label = "end";      break;
        case T::INT_KW:     color = ASTColor::KW; label = "int";      break;
        case T::FLOAT_KW:   color = ASTColor::KW; label = "float";    break;

        // Identificadores y literales
        case T::IDENTIFIER: color = ASTColor::ID;  label = "id:" + node->lexeme;   break;
        case T::NUMBER:     color = ASTColor::NUM; label = "num:" + node->lexeme;  break;

        // Operadores
        case T::PLUS:   color = ASTColor::OP; label = "+"; break;
        case T::MINUS:  color = ASTColor::OP; label = "-"; break;
        case T::MULT:   color = ASTColor::OP; label = "*"; break;
        case T::ASSIGN: color = ASTColor::OP; label = "="; break;

        // Puntuación: mostrar brevemente
        case T::SEMICOLON: label = ";"; break;
        case T::COLON:     label = ":"; break;
        case T::LPAREN:    label = "("; break;
        case T::RPAREN:    label = ")"; break;

        default:
            if (!node->lexeme.empty()) label = node->lexeme;
            else label = "?";
            break;
    }

    if (!color.empty())
        return color + label + ASTColor::RESET;
    return label;
}

// ── Nodos que no aportan semánticamente ──────────────────────────────────────
static bool shouldSkip(ASTNode* node) {
    if (!node) return true;
    using T = ASTNodeType;
    // Puntuación pura: nunca visible en el AST semántico
    switch (node->type) {
        case T::SEMICOLON:
        case T::COLON:
        case T::LPAREN:
        case T::RPAREN:
        case T::PROGRAM_KW:
        case T::VAR_KW:
        case T::BEGIN_KW:
        case T::END_KW:
            return true;
        default:
            break;
    }
    // Nodos intermedios SIN hijos visibles y sin significado propio
    // STATEMENT y ASSIGN_STATEMENT NUNCA se omiten: son el contenido semántico
    if (node->children.empty()) {
        switch (node->type) {
            case T::VARIABLE_LIST:
            case T::STATEMENT_LIST:
            case T::EXPRESSION:
            case T::EXPRESSION_PRIME:
            case T::FACTOR:
            case T::TYPE_SPECIFIER:
            case T::UNKNOWN:
            case T::EPSILON:
                return true;
            default:
                break;
        }
    }
    // Nodo EPSILON explícito (hoja)
    if (node->type == T::EPSILON) return true;
    return false;
}

// ── Nodos transparentes: sus hijos suben al padre directamente ───────────────
// L2 (VARIABLE_LIST) y S2 (STATEMENT_LIST) son artefactos de eliminación de
// recursión izquierda. Sus hijos (L/S) deben aparecer al mismo nivel que sus
// hermanos. L (DECLARATION) y S (STATEMENT) son contenido semántico real:
// NO son transparentes.
static bool isTransparent(ASTNode* node) {
    if (!node) return false;
    using T = ASTNodeType;
    // Solo aplanar los nodos de "continuación" (L2 y S2)
    return node->type == T::VARIABLE_LIST || node->type == T::STATEMENT_LIST;
}

// Recolecta hijos visibles, aplanando nodos transparentes recursivamente
static void collectVisible(ASTNode* node, std::vector<ASTNode*>& out) {
    for (auto& child : node->children) {
        ASTNode* c = child.get();
        if (shouldSkip(c)) continue;
        if (isTransparent(c)) {
            collectVisible(c, out);  // los nietos suben
        } else {
            out.push_back(c);
        }
    }
}

// ── Impresión recursiva tipo árbol ────────────────────────────────────────────
void ASTPrinter::printNode(ASTNode* node, const std::string& prefix, bool isLast) {
    if (!node) return;
    if (shouldSkip(node)) return;

    std::string connector = isLast ? "└── " : "├── ";
    std::string childPfx  = prefix + (isLast ? "    " : "│   ");

    // Añadir posición de fuente para identificadores y números
    std::string label = nodeLabel(node);
    if (node->line > 0 && (node->type == ASTNodeType::IDENTIFIER ||
                            node->type == ASTNodeType::NUMBER)) {
        label += ASTColor::RESET;
        label += " \033[2m(" + std::to_string(node->line) + ":"
               + std::to_string(node->column) + ")\033[0m";
    }

    output_ += prefix + connector + label + "\n";

    // Aplanar nodos transparentes para que sus hijos aparezcan al mismo nivel
    std::vector<ASTNode*> visibleChildren;
    if (isTransparent(node)) {
        collectVisible(node, visibleChildren);
    } else {
        collectVisible(node, visibleChildren);
    }

    for (size_t i = 0; i < visibleChildren.size(); i++) {
        bool last = (i == visibleChildren.size() - 1);
        printNode(visibleChildren[i], childPfx, last);
    }
}

std::string ASTPrinter::getOutput() const { return output_; }