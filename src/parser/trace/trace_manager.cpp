#include "trace_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

TraceManager::TraceManager()
    : level_(TraceLevel::NONE)
    , stream_(&std::cout)
    , stepCount_(0)
    , fileOpened_(false) {}

TraceManager::~TraceManager() {
    if (fileOpened_) {
        file_.close();
    }
}

void TraceManager::setLevel(TraceLevel level) {
    level_ = level;
}

void TraceManager::setOutputFile(const std::string& filename) {
    if (fileOpened_) {
        file_.close();
        fileOpened_ = false;
    }

    if (!filename.empty()) {
        file_.open(filename);
        if (file_.is_open()) {
            fileOpened_ = true;
            stream_ = &file_;
        }
    }
}

void TraceManager::setOutputStream(std::ostream* stream) {
    if (stream) {
        stream_ = stream;
    }
}

bool TraceManager::isEnabled(TraceLevel level) const {
    return static_cast<int>(level_) >= static_cast<int>(level);
}

void TraceManager::logInitialization(const std::string& startSymbol, int tokenCount) {
    if (!isEnabled(TraceLevel::NORMAL)) return;

    TraceEntry entry;
    entry.step = stepCount_++;
    entry.action = TraceAction::INITIALIZATION;
    entry.message = "Inicializacion del parser";
    entry.stackState = "$ " + startSymbol;
    entry.tokenInfo = "Tokens de entrada: " + std::to_string(tokenCount);

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logStep(int step, const std::string& stackTop, const std::string& currentToken) {
    if (!isEnabled(TraceLevel::NORMAL)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::STEP;
    entry.message = "Paso de analisis";
    entry.stackState = stackTop;
    entry.tokenInfo = currentToken;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logMatch(int step, const std::string& terminal, const std::string& lexeme) {
    if (!isEnabled(TraceLevel::NORMAL)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::MATCH;
    entry.message = "Match - Terminal coincide: " + terminal;
    entry.tokenInfo = "Token: '" + lexeme + "'";

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logExpand(int step, const std::string& nonTerminal, const std::string& production) {
    if (!isEnabled(TraceLevel::NORMAL)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::EXPAND;
    entry.message = "Expandir no-terminal: " + nonTerminal;
    entry.production = production;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logError(int step, const std::string& expected, const std::string& received, int line) {
    if (!isEnabled(TraceLevel::MINIMAL)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::ERROR;
    entry.message = "Error sintactico en linea " + std::to_string(line);
    entry.tokenInfo = "Se esperaba: " + expected + ", se recibio: " + received;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logWarning(int step, const std::string& message) {
    if (!isEnabled(TraceLevel::VERBOSE)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::WARNING;
    entry.message = message;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logSuccess(int step, const std::string& message) {
    if (!isEnabled(TraceLevel::MINIMAL)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::SUCCESS;
    entry.message = message;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logReduction(int step, const std::string& nonTerminal) {
    if (!isEnabled(TraceLevel::VERBOSE)) return;

    TraceEntry entry;
    entry.step = step;
    entry.action = TraceAction::REDUCE;
    entry.message = "Reduccion a: " + nonTerminal;

    entries_.push_back(entry);
    writeEntry(entry);
}

void TraceManager::logStackState(const std::string& state) {
    if (!isEnabled(TraceLevel::DEBUG)) return;
    *stream_ << "  [STACK] " << state << "\n";
}

void TraceManager::logProduction(const std::string& production) {
    if (!isEnabled(TraceLevel::VERBOSE)) return;
    *stream_ << "  [PRODUCTION] " << production << "\n";
}

void TraceManager::logDebug(const std::string& message) {
    if (!isEnabled(TraceLevel::DEBUG)) return;
    *stream_ << "  [DEBUG] " << message << "\n";
}

void TraceManager::flush() {
    if (fileOpened_) {
        file_.flush();
    }
}

std::string TraceManager::getTrace() const {
    std::ostringstream ss;
    for (const auto& entry : entries_) {
        ss << "Step " << entry.step << ": " << entry.message << "\n";
    }
    return ss.str();
}

void TraceManager::writeEntry(const TraceEntry& entry) {
    if (level_ == TraceLevel::NONE) return;

    std::string formatted;
    switch (level_) {
        case TraceLevel::MINIMAL:
            formatted = TraceFormatter::formatMinimal(entry);
            break;
        case TraceLevel::NORMAL:
            formatted = TraceFormatter::formatNormal(entry);
            break;
        case TraceLevel::VERBOSE:
            formatted = TraceFormatter::formatVerbose(entry);
            break;
        case TraceLevel::DEBUG:
            formatted = TraceFormatter::formatDebug(entry);
            break;
        default:
            return;
    }

    *stream_ << formatted;
}

std::string TraceManager::actionToString(TraceAction action) {
    switch (action) {
        case TraceAction::INITIALIZATION: return "INIT";
        case TraceAction::STEP: return "STEP";
        case TraceAction::MATCH: return "MATCH";
        case TraceAction::EXPAND: return "EXPAND";
        case TraceAction::ERROR: return "ERROR";
        case TraceAction::REDUCE: return "REDUCE";
        case TraceAction::SUCCESS: return "OK";
        case TraceAction::WARNING: return "WARN";
        default: return "????";
    }
}

std::string TraceManager::getIndent(TraceAction action) {
    switch (action) {
        case TraceAction::INITIALIZATION: return "[INIT] ";
        case TraceAction::STEP: return "[STEP] ";
        case TraceAction::MATCH: return "[PASS] ";
        case TraceAction::EXPAND: return "[PASS] ";
        case TraceAction::ERROR: return "[ERR!] ";
        case TraceAction::REDUCE: return "[PASS] ";
        case TraceAction::SUCCESS: return "[OK]   ";
        case TraceAction::WARNING: return "[WARN] ";
        default: return "[????] ";
    }
}

// ── Trace Formatter Implementation ────────────────────────────────────────────
std::string TraceFormatter::formatMinimal(const TraceEntry& entry) {
    std::ostringstream ss;
    if (entry.action == TraceAction::ERROR) {
        ss << entry.message << " | " << entry.tokenInfo << "\n";
    } else if (entry.action == TraceAction::SUCCESS) {
        ss << entry.message << "\n";
    }
    return ss.str();
}

std::string TraceFormatter::formatNormal(const TraceEntry& entry) {
    std::ostringstream ss;
    ss << std::setw(4) << entry.step << " | ";

    switch (entry.action) {
        case TraceAction::INITIALIZATION:
            ss << "[INIT] " << entry.message << "\n";
            ss << "         Pila: " << entry.stackState << "\n";
            ss << "         " << entry.tokenInfo << "\n";
            break;
        case TraceAction::STEP:
            ss << "[STEP] " << entry.stackState << " vs " << entry.tokenInfo << "\n";
            break;
        case TraceAction::MATCH:
            ss << "[MATCH] " << entry.message << " - " << entry.tokenInfo << "\n";
            break;
        case TraceAction::EXPAND:
            ss << "[EXPAND] " << entry.message;
            if (!entry.production.empty()) {
                ss << " -> " << entry.production;
            }
            ss << "\n";
            break;
        case TraceAction::ERROR:
            ss << "[ERROR] " << entry.message << "\n";
            ss << "         " << entry.tokenInfo << "\n";
            break;
        case TraceAction::SUCCESS:
            ss << "[OK] " << entry.message << "\n";
            break;
        default:
            ss << entry.message << "\n";
    }
    return ss.str();
}

std::string TraceFormatter::formatVerbose(const TraceEntry& entry) {
    std::ostringstream ss;
    ss << formatNormal(entry);

    if (!entry.stackState.empty() && entry.action != TraceAction::INITIALIZATION) {
        ss << "         Pila: " << entry.stackState << "\n";
    }

    return ss.str();
}

std::string TraceFormatter::formatDebug(const TraceEntry& entry) {
    std::ostringstream ss;
    ss << "═══════════════════════════════════════════════════════════\n";
    ss << formatVerbose(entry);
    ss << "═══════════════════════════════════════════════════════════\n";
    return ss.str();
}

std::string TraceFormatter::formatHeader(const std::string& title) {
    std::ostringstream ss;
    ss << "\n═══════════════════════════════════════════════════════════\n";
    ss << std::setw(56) << std::left << title << "\n";
    ss << "═══════════════════════════════════════════════════════════\n";
    return ss.str();
}

std::string TraceFormatter::formatSeparator() {
    return "────────────────────────────────────────────────────────────\n";
}

std::string TraceFormatter::formatStack(const std::string& stack) {
    return "Stack: " + stack + "\n";
}

std::string TraceFormatter::formatToken(const std::string& token, const std::string& lexeme, int line) {
    std::ostringstream ss;
    ss << "Token: " << token << " '" << lexeme << "' [linea: " << line << "]";
    return ss.str();
}