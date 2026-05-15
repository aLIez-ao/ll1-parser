#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stack>

// ── Trace Level ────────────────────────────────────────────────────────────────
enum class TraceLevel {
    NONE = 0,
    MINIMAL = 1,
    NORMAL = 2,
    VERBOSE = 3,
    DEBUG = 4
};

// ── Trace Entry ────────────────────────────────────────────────────────────────
enum class TraceAction {
    INITIALIZATION,
    STEP,
    MATCH,
    EXPAND,
    ERROR,
    REDUCE,
    SUCCESS,
    WARNING
};

struct TraceEntry {
    int step;
    TraceAction action;
    std::string message;
    std::string stackState;
    std::string tokenInfo;
    std::string production;
};

// ── Trace Manager ──────────────────────────────────────────────────────────────
// Manages trace output with configurable level and destination
// ──────────────────────────────────────────────────────────────────────────────
class TraceManager {
public:
    TraceManager();
    ~TraceManager();

    void setLevel(TraceLevel level);
    void setOutputFile(const std::string& filename);
    void setOutputStream(std::ostream* stream);

    bool isEnabled(TraceLevel level) const;

    void logInitialization(const std::string& startSymbol, int tokenCount);
    void logStep(int step, const std::string& stackTop, const std::string& currentToken);
    void logMatch(int step, const std::string& terminal, const std::string& lexeme);
    void logExpand(int step, const std::string& nonTerminal, const std::string& production);
    void logError(int step, const std::string& expected, const std::string& received, int line);
    void logWarning(int step, const std::string& message);
    void logSuccess(int step, const std::string& message);
    void logReduction(int step, const std::string& nonTerminal);

    void logStackState(const std::string& state);
    void logProduction(const std::string& production);
    void logDebug(const std::string& message);

    void flush();
    std::string getTrace() const;

private:
    TraceLevel level_;
    std::ofstream file_;
    std::ostream* stream_;
    std::vector<TraceEntry> entries_;
    int stepCount_;
    bool fileOpened_;

    void writeEntry(const TraceEntry& entry);
    std::string actionToString(TraceAction action);
    std::string getIndent(TraceAction action);
};

// ── Trace Formatter ────────────────────────────────────────────────────────────
// Formats trace entries for different output styles
// ──────────────────────────────────────────────────────────────────────────────
class TraceFormatter {
public:
    static std::string formatMinimal(const TraceEntry& entry);
    static std::string formatNormal(const TraceEntry& entry);
    static std::string formatVerbose(const TraceEntry& entry);
    static std::string formatDebug(const TraceEntry& entry);

    static std::string formatHeader(const std::string& title);
    static std::string formatSeparator();
    static std::string formatStack(const std::string& stack);
    static std::string formatToken(const std::string& token, const std::string& lexeme, int line);
};