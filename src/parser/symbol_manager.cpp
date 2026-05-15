#include "symbol_manager.h"
#include <sstream>
#include <iomanip>

SymbolManager::SymbolManager() {
    scopes_.push_back(Scope());  // Global scope
}

bool SymbolManager::declare(const std::string& name, const std::string& type, int line, int column) {
    auto& scope = currentScope();

    if (scope.symbols.count(name) > 0) {
        std::ostringstream ss;
        ss << "Error: Declaracion duplicada de '" << name << "' en linea " << line;
        errors_.push_back(ss.str());
        return false;
    }

    scope.symbols[name] = ParserSymbolEntry(name, type, line, column);
    return true;
}

bool SymbolManager::isDeclared(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->symbols.count(name) > 0) {
            return true;
        }
    }
    return false;
}

const ParserSymbolEntry* SymbolManager::lookup(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->symbols.find(name);
        if (found != it->symbols.end()) {
            return &found->second;
        }
    }
    return nullptr;
}

void SymbolManager::markInitialized(const std::string& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->symbols.find(name);
        if (found != it->symbols.end()) {
            found->second.isInitialized = true;
            return;
        }
    }
}

bool SymbolManager::isInitialized(const std::string& name) const {
    const auto* entry = lookup(name);
    return entry && entry->isInitialized;
}

bool SymbolManager::hasDuplicateDeclaration(const std::string& name) const {
    return currentScope().symbols.count(name) > 0;
}

std::vector<std::string> SymbolManager::getErrors() const {
    return errors_;
}

std::vector<std::string> SymbolManager::getWarnings() const {
    return warnings_;
}

void SymbolManager::printTable(std::ostream& out) const {
    out << "\n═══════════════════════════════════════════════════════════\n";
    out << "                    TABLA DE SIMBOLOS (Parser)\n";
    out << "═══════════════════════════════════════════════════════════\n";
    out << std::setw(15) << std::left << "Nombre"
        << std::setw(10) << "Tipo"
        << std::setw(8) << "Linea"
        << std::setw(8) << "Columna"
        << "Inicializado\n";
    out << "─────────────────────────────────────────────────────────────\n";

    for (const auto& scope : scopes_) {
        for (const auto& entry : scope.symbols) {
            out << std::setw(15) << std::left << entry.second.name
                << std::setw(10) << entry.second.type
                << std::setw(8) << entry.second.line
                << std::setw(8) << entry.second.column
                << (entry.second.isInitialized ? "Si" : "No")
                << "\n";
        }
    }

    out << "─────────────────────────────────────────────────────────────\n";
    out << "Total: " << getSymbolCount() << " simbolo(s)\n";
    out << "═══════════════════════════════════════════════════════════\n";
}

std::string SymbolManager::toString() const {
    std::ostringstream ss;
    printTable(ss);
    return ss.str();
}

void SymbolManager::pushScope() {
    scopes_.push_back(Scope());
}

void SymbolManager::popScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

int SymbolManager::getSymbolCount() const {
    int count = 0;
    for (const auto& scope : scopes_) {
        count += static_cast<int>(scope.symbols.size());
    }
    return count;
}

SymbolManager::Scope& SymbolManager::currentScope() {
    return scopes_.back();
}

const SymbolManager::Scope& SymbolManager::currentScope() const {
    return scopes_.back();
}