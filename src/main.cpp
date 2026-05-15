#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "lexer/lexer/lexer.h"
#include "grammar/core/grammar.h"
#include "grammar/io/grammar_parser.h"
#include "grammar/analysis/first_set.h"
#include "grammar/analysis/follow_set.h"
#include "grammar/analysis/ll1_table.h"
#include "grammar/analysis/validator.h"
#include "parser/parser.h"

using namespace grammar;

void printHelp(const char* name) {
    std::cout << "LL1 Parser\n";
    std::cout << "Uso: " << name << " [--input file] [--trace] [--trace-level n]\n";
    std::cout << "  --input: archivo fuente\n";
    std::cout << "  --trace: generar traza a traza_analisis.txt\n";
    std::cout << "  --trace-level: 0-4 (NONE, MINIMAL, NORMAL, VERBOSE, DEBUG)\n";
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    bool trace = false;
    TraceLevel traceLevel = TraceLevel::NORMAL;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) inputFile = argv[++i];
        else if (arg == "--trace") trace = true;
        else if (arg == "--trace-level" && i + 1 < argc) traceLevel = (TraceLevel)std::stoi(argv[++i]);
        else if (arg == "--help" || arg == "-h") { printHelp(argv[0]); return 0; }
    }

    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "                     LL1 PARSER v1.0\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";

    // Default test code
    std::string source;
    if (!inputFile.empty()) {
        std::ifstream f(inputFile);
        std::stringstream ss;
        ss << f.rdbuf();
        source = ss.str();
    } else {
        source = R"(program miPrograma;
var
  x: int;
  y: float;
begin
  x = 10;
  y = x + 5;
end)";
    }

    std::cout << "Codigo fuente:\n─────────────────────────────────────────────\n";
    std::istringstream ss(source);
    std::string line;
    int lc = 1;
    while (std::getline(ss, line)) std::cout << lc++ << " | " << line << "\n";
    std::cout << "───────────────────────────────────────────────────────────\n\n";

    // Lexical analysis
    std::cout << "[1] Analisis lexico...\n";
    Lexer lexer(source);
    auto lexResult = lexer.analyze();
    std::cout << "  Tokens: " << lexResult.tokens.size() << "\n";
    std::cout << "  Identificadores: " << lexResult.symbolTable.getSize() << "\n";

    if (lexResult.errorHandler.hasErrors()) {
        std::cout << "  Errores lexicos: " << lexResult.errorHandler.getErrorCount() << "\n";
    }

    // Show tokens
    std::cout << "\n  Lista de tokens:\n";
    for (size_t i = 0; i < lexResult.tokens.size(); i++) {
        auto& t = lexResult.tokens[i];
        std::cout << "    " << i << ": " << t.toParserSymbol() << " '" << t.lexeme << "'\n";
    }
    std::cout << "\n";

    // Grammar - using exact terminal names that match token symbols
    std::cout << "[2] Cargando gramatica...\n";
    Grammar g;
    g.startSymbol = "P";
    g.nonTerminals = {"P", "D", "L", "L2", "T", "B", "S", "S2", "E", "E2", "F"};
    g.terminals = {"PROGRAM", "ID", "SEMICOLON", "VAR", "COLON", "INT", "FLOAT", "BEGIN", "END", "ASSIGN", "PLUS", "MINUS", "MULT", "NUM", "LPAREN", "RPAREN"};
    g.ntOrder = {"P", "D", "L", "L2", "T", "B", "S", "S2", "E", "E2", "F"};

    // Production rules - matching token symbols exactly
    // P -> PROGRAM ID SEMICOLON D B
    g.rules["P"] = {{Symbol{"PROGRAM"}, Symbol{"ID"}, Symbol{"SEMICOLON"}, Symbol{"D"}, Symbol{"B"}}};
    // D -> VAR L
    g.rules["D"] = {{Symbol{"VAR"}, Symbol{"L"}}};
    // L -> ID COLON T SEMICOLON L2
    g.rules["L"] = {{Symbol{"ID"}, Symbol{"COLON"}, Symbol{"T"}, Symbol{"SEMICOLON"}, Symbol{"L2"}}};
    // L2 -> L | EPS
    g.rules["L2"] = {{Symbol{"L"}}, {Symbol{"EPS"}}};
    // T -> INT | FLOAT
    g.rules["T"] = {{Symbol{"INT"}}, {Symbol{"FLOAT"}}};
    // B -> BEGIN S END
    g.rules["B"] = {{Symbol{"BEGIN"}, Symbol{"S"}, Symbol{"END"}}};
    // S -> ID ASSIGN E SEMICOLON S2
    g.rules["S"] = {{Symbol{"ID"}, Symbol{"ASSIGN"}, Symbol{"E"}, Symbol{"SEMICOLON"}, Symbol{"S2"}}};
    // S2 -> S | EPS
    g.rules["S2"] = {{Symbol{"S"}}, {Symbol{"EPS"}}};
    // E -> F E2
    g.rules["E"] = {{Symbol{"F"}, Symbol{"E2"}}};
    // E2 -> PLUS F E2 | EPS
    g.rules["E2"] = {{Symbol{"PLUS"}, Symbol{"F"}, Symbol{"E2"}}, {Symbol{"EPS"}}};
    // F -> ID | NUM
    g.rules["F"] = {{Symbol{"ID"}}, {Symbol{"NUM"}}};

    std::cout << "  Gramatica cargada.\n";

    // Validate
    std::cout << "\n[3] Validando gramatica...\n";
    auto validation = Validator::validate(g);
    if (!validation.valid) {
        std::cout << "  ERROR: Gramatica invalida:\n";
        for (auto& e : validation.errors) std::cout << "    - " << e << "\n";
        return 1;
    }
    std::cout << "  Gramatica valida.\n";

    // First/Follow
    std::cout << "\n[4] Calculando conjuntos FIRST y FOLLOW...\n";
    auto first = FirstSetCalculator::compute(g);
    auto follow = FollowSetCalculator::compute(g, first);
    std::cout << "  Conjuntos calculados.\n";

    // LL1 Table
    std::cout << "\n[5] Construyendo tabla LL(1)...\n";
    auto ll1Result = LL1TableGenerator::build(g, first, follow);
    if (!ll1Result.isValid) {
        std::cout << "  ERROR: La gramatica no es LL(1)\n";
        for (auto& e : ll1Result.errors) std::cout << "    - " << e << "\n";
        return 1;
    }
    std::cout << "  Tabla construida.\n";

    // Parse
    std::cout << "\n[6] Analisis sintactico...\n";
    LL1Parser parser;
    if (trace) {
        parser.setTraceLevel(traceLevel);
        parser.setTraceFile("traza_analisis.txt");
    }
    auto parseResult = parser.parse(lexResult.tokens, ll1Result.table, g.startSymbol);

    std::cout << "\n═══════════════════════════════════════════════════════════\n";
    std::cout << "                       RESULTADOS\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "  Estado: " << (parseResult.success ? "EXITO" : "FALLIDO") << "\n";
    std::cout << "  Errores: " << parseResult.errorCount << "\n";

    if (!parseResult.errors.empty()) {
        std::cout << "\n  Errores:\n";
        for (auto& e : parseResult.errors) {
            std::cout << "    - " << e << "\n";
        }
    }

    if (trace) {
        std::cout << "\n  Traza guardada en: traza_analisis.txt\n";
    }

    std::cout << "\n═══════════════════════════════════════════════════════════\n";

    return parseResult.success ? 0 : 1;
}