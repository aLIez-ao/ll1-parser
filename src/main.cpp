/**
 * @file main.cpp
 * @brief Punto de entrada y orquestador del compilador LL(1)
 * @description
 * Este archivo contiene la función main() que coordina todas las fases
 * del análisis de compiladores:
 * 
 * [1] Lectura del código fuente
 * [2] Análisis léxico (Lexer)
 * [3] Carga y parseo de la gramática
 * [4] Validación de la gramática
 * [5] Cálculo de conjuntos FIRST y FOLLOW
 * [6] Construcción de la tabla LL(1)
 * [7] Análisis sintáctico (Parser LL(1))
 * [8] Análisis semántico
 * [9] Tabla de símbolos
 * [10] Construcción y visualización del AST
 * 
 * El programa acepta argumentos de línea de comandos para:
 * - Especificar el archivo de entrada (--input)
 * - Especificar archivo de gramática (--grammar)
 * - Mostrar conjuntos FIRST/FOLLOW (--sets)
 * - Mostrar tabla LL(1) (--table)
 * - Generar traza de análisis (--trace)
 * - Nivel de detalle de la traza (--trace-level)
 * 
 * Uso:
 *   ./ll1-parser --input <archivo> [--sets] [--table] [--trace]
 * 
 * @author Equipo Compiler
 * @version 2.0
 * @see Lexer, LL1Parser, SemanticAnalyzer
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "lexer/lexer/lexer.h"
#include "grammar/core/grammar.h"
#include "grammar/io/grammar_parser.h"
#include "grammar/analysis/first_set.h"
#include "grammar/analysis/follow_set.h"
#include "grammar/analysis/ll1_table.h"
#include "grammar/analysis/validator.h"
#include "parser/parser.h"
#include "parser/ast/ast_node.h"
#include "parser/semantic/semantic_analyzer.h"

using namespace grammar;

/**
 * @name Funciones auxiliares de visualización
 * @brief Funciones para formatear la salida en consola
 * @{
 */

/**
 * @brief Imprime un separador horizontal de línea
 * @param c Carácter de repetición (default: ─)
 * @param len Longitud del separador (default: 61)
 */
static void printSeparator(const std::string& c = "─", int len = 61) {
    std::string line;
    line.reserve(len * c.size());
    for (int i = 0; i < len; i++) line += c;
    std::cout << line << "\n";
}

static void printHeader(const std::string& title) {
    std::cout << "\n";
    printSeparator("═");
    std::string pad(std::max(0, (61 - (int)title.size()) / 2), ' ');
    std::cout << pad << title << "\n";
    printSeparator("═");
}

static void printSection(int num, const std::string& title) {
    std::cout << "\n[" << num << "] " << title << "\n";
    printSeparator();
}

// ── Mostrar reglas de la gramática ───────────────────────────────────────────
static void displayGrammar(const Grammar& g) {
    for (const auto& nt : g.ntOrder) {
        auto it = g.rules.find(nt);
        if (it == g.rules.end()) continue;
        const auto& prods = it->second;
        for (size_t i = 0; i < prods.size(); i++) {
            if (i == 0)
                std::cout << "    " << std::setw(4) << std::left << nt << " →  ";
            else
                std::cout << "    " << std::setw(4) << " "   << "  │  ";
            for (size_t j = 0; j < prods[i].size(); j++) {
                if (j > 0) std::cout << " ";
                std::cout << prods[i][j].name;
            }
            std::cout << "\n";
        }
    }
}

// ── Mostrar conjuntos FIRST y FOLLOW ────────────────────────────────────────
static void displayFirstFollow(const Grammar& g,
                                const FirstFollowMap& first,
                                const FirstFollowMap& follow) {
    // Anchura de columnas
    const int colNT   = 5;
    const int colSet  = 38;

    auto setToStr = [](const StringSet& s) {
        std::string r = "{ ";
        bool comma = false;
        for (const auto& x : s) {
            if (comma) r += ", ";
            r += x;
            comma = true;
        }
        r += " }";
        return r;
    };

    std::cout << "    " << std::left
              << std::setw(colNT)  << "NT"
              << std::setw(colSet) << "FIRST"
              << "FOLLOW\n";
    std::cout << "    " << std::string(colNT + colSet + 30, '-') << "\n";

    for (const auto& nt : g.ntOrder) {
        auto fi = first.find(nt);
        auto fo = follow.find(nt);
        std::string fs = (fi != first.end())  ? setToStr(fi->second) : "{}";
        std::string fw = (fo != follow.end()) ? setToStr(fo->second) : "{}";
        std::cout << "    " << std::left
                  << std::setw(colNT)  << nt
                  << std::setw(colSet) << fs
                  << fw << "\n";
    }
}

// ── Mostrar tabla LL(1) compacta (bloques de columnas) ──────────────────────
static void displayLL1Table(const Grammar& g, const LL1Table& table) {
    // Recopilar todos los terminales con entradas válidas
    std::vector<std::string> terminals;
    for (const auto& [nt, row] : table)
        for (const auto& [term, entry] : row)
            if (entry.isValid &&
                std::find(terminals.begin(), terminals.end(), term) == terminals.end())
                terminals.push_back(term);
    std::sort(terminals.begin(), terminals.end());

    // Calcular ancho máximo de cada columna terminal
    const int colNT = 5;
    const int MAX_COLS = 6;          // máximo de terminales por bloque
    const int MIN_COL_W = 8;

    auto cellStr = [&](const std::string& nt, const std::string& t) -> std::string {
        auto rowIt = table.find(nt);
        if (rowIt == table.end()) return "-";
        auto cellIt = rowIt->second.find(t);
        if (cellIt == rowIt->second.end() || !cellIt->second.isValid) return "-";
        // Formato compacto: solo el RHS de la producción
        return cellIt->second.productionStr;
    };

    // Imprimir en bloques de MAX_COLS terminales
    for (size_t start = 0; start < terminals.size(); start += MAX_COLS) {
        size_t end = std::min(start + (size_t)MAX_COLS, terminals.size());
        std::vector<std::string> block(terminals.begin() + start,
                                       terminals.begin() + end);

        // Calcular ancho de cada columna en este bloque
        std::vector<int> widths(block.size(), MIN_COL_W);
        for (size_t bi = 0; bi < block.size(); bi++) {
            widths[bi] = std::max(widths[bi], (int)block[bi].size() + 2);
            for (const auto& nt : g.ntOrder) {
                int w = (int)cellStr(nt, block[bi]).size() + 2;
                widths[bi] = std::max(widths[bi], w);
            }
        }

        // Separador del bloque
        int totalW = colNT;
        for (int w : widths) totalW += w;
        std::string sep = "    " + std::string(totalW, '-');

        if (start > 0) std::cout << "\n";
        // Encabezado de columnas
        std::cout << sep << "\n";
        std::cout << "    " << std::setw(colNT) << std::left << "NT";
        for (size_t bi = 0; bi < block.size(); bi++)
            std::cout << std::setw(widths[bi]) << std::left << block[bi];
        std::cout << "\n" << sep << "\n";

        // Filas
        for (const auto& nt : g.ntOrder) {
            std::cout << "    " << std::setw(colNT) << std::left << nt;
            for (size_t bi = 0; bi < block.size(); bi++) {
                std::string cell = cellStr(nt, block[bi]);
                if ((int)cell.size() >= widths[bi] - 1)
                    cell = cell.substr(0, widths[bi] - 3) + "..";
                std::cout << std::setw(widths[bi]) << std::left << cell;
            }
            std::cout << "\n";
        }
        std::cout << sep << "\n";
    }
}

// ── Mostrar tabla de símbolos semántica ──────────────────────────────────────
static void displaySymbolTable(const SemanticResult& sem) {
    if (sem.symbols.empty()) {
        std::cout << "    (sin símbolos declarados)\n";
        return;
    }
    const int cName  = 16;
    const int cType  = 8;
    const int cLine  = 7;
    const int cVal   = 14;
    const int cUsed  = 6;
    std::string sep = "    " + std::string(cName+cType+cLine+cVal+cUsed+2, '-');
    std::cout << "    " << std::left
              << std::setw(cName) << "Variable"
              << std::setw(cType) << "Tipo"
              << std::setw(cLine) << "Línea"
              << std::setw(cVal)  << "Valor inicial"
              << "Usada\n";
    std::cout << sep << "\n";
    for (const auto& [name, sym] : sem.symbols) {
        std::cout << "    " << std::left
                  << std::setw(cName) << name
                  << std::setw(cType) << sym.type
                  << std::setw(cLine) << sym.declLine
                  << std::setw(cVal)  << "-"   // sin evaluación de expresiones aún
                  << (sym.used ? "sí" : "no") << "\n";
    }
    std::cout << sep << "\n";
    std::cout << "    Total: " << sem.symbols.size() << " símbolo(s)\n";
}

// ── Ayuda ────────────────────────────────────────────────────────────────────
static void printHelp(const char* name) {
    std::cout << "Uso: " << name << " [opciones]\n\n";
    std::cout << "  --input  <archivo>   Fuente a analizar\n";
    std::cout << "  --grammar <archivo>  Archivo de gramática (por defecto: grammar.txt)\n";
    std::cout << "  --sets               Mostrar conjuntos FIRST y FOLLOW\n";
    std::cout << "  --table              Mostrar tabla LL(1)\n";
    std::cout << "  --trace              Guardar traza en traza_analisis.txt\n";
    std::cout << "  --trace-level <0-4>  Nivel de detalle de la traza\n";
    std::cout << "  --help               Mostrar esta ayuda\n";
}

// ── Buscar archivo de gramática ───────────────────────────────────────────────
static std::string findGrammarFile(const std::string& hint) {
    if (!hint.empty()) return hint;
    for (const auto& path : {
            "grammar.txt",
            "src/grammar/grammar.txt",
            "../grammar/grammar.txt",
            "../src/grammar/grammar.txt",
            "../../src/grammar/grammar.txt",
            "../../grammar/grammar.txt"}) {
        std::ifstream f(path);
        if (f.good()) return path;
    }
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════
//  main
// ═══════════════════════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    // ── Argumentos ───────────────────────────────────────────────────────────
    std::string inputFile, grammarFile;
    bool trace      = false;
    bool showSets   = false;
    bool showTable  = false;
    TraceLevel traceLevel = TraceLevel::NORMAL;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if      (arg == "--input"       && i+1 < argc) inputFile   = argv[++i];
        else if (arg == "--grammar"     && i+1 < argc) grammarFile = argv[++i];
        else if (arg == "--trace")                     trace       = true;
        else if (arg == "--sets")                      showSets    = true;
        else if (arg == "--table")                     showTable   = true;
        else if (arg == "--trace-level" && i+1 < argc) traceLevel = (TraceLevel)std::stoi(argv[++i]);
        else if (arg == "--help" || arg == "-h") { printHelp(argv[0]); return 0; }
    }

    printHeader("LL1 PARSER  v2.0");

    // ── [1] Código fuente ────────────────────────────────────────────────────
    printSection(1, "Código fuente");
    std::string source;
    if (!inputFile.empty()) {
        std::ifstream f(inputFile);
        if (!f.is_open()) {
            std::cerr << "  Error: no se pudo abrir '" << inputFile << "'\n";
            return 1;
        }
        std::ostringstream ss; ss << f.rdbuf();
        source = ss.str();
        std::cout << "  Archivo: " << inputFile << "\n";
    } else {
        source = "program miPrograma;\nvar\n  x: int;\n  y: float;\nbegin\n  x = 10;\n  y = x + 5;\nend";
        std::cout << "  (usando fuente interna de ejemplo)\n";
    }
    std::cout << "\n";
    {
        std::istringstream ss(source); std::string line; int lc = 1;
        while (std::getline(ss, line))
            std::cout << "  " << std::setw(3) << lc++ << " │ " << line << "\n";
    }

    // ── [2] Análisis léxico ──────────────────────────────────────────────────
    printSection(2, "Análisis léxico");
    Lexer lexer(source);
    auto lexResult = lexer.analyze();

    std::cout << "  Tokens generados : " << lexResult.tokens.size() << "\n";
    std::cout << "  Identificadores  : " << lexResult.symbolTable.getSize() << "\n";
    if (lexResult.errorHandler.hasErrors())
        std::cout << "  Errores léxicos  : " << lexResult.errorHandler.getErrorCount() << "\n";

    // Categoría legible de un token
    auto tokenCategory = [](const Token& t) -> std::string {
        if (t.isKeyword())   return "Palabra clave";
        if (t.type == TokenType::IDENT) return "Identificador";
        if (t.isLiteral())   return "Literal";
        if (t.isOperator())  return "Operador";
        if (t.isDelimiter()) return "Delimitador";
        if (t.isEOF())       return "Fin de archivo";
        if (t.isError())     return "ERROR LEXICO";
        return "Desconocido";
    };

    // Cabecera
    std::cout << "\n";
    const int W_IDX=5, W_LIN=7, W_COL=6, W_SYM=14, W_CAT=16;
    std::cout << "  " << std::string(W_IDX+W_LIN+W_COL+W_SYM+W_CAT+10, '-') << "\n";
    std::cout << "  " << std::left
              << std::setw(W_IDX) << "#"
              << std::setw(W_LIN) << "Línea"
              << std::setw(W_COL) << "Col"
              << std::setw(W_SYM) << "Símbolo"
              << std::setw(W_CAT) << "Categoría"
              << "Lexema\n";
    std::cout << "  " << std::string(W_IDX+W_LIN+W_COL+W_SYM+W_CAT+10, '-') << "\n";

    for (size_t i = 0; i < lexResult.tokens.size(); i++) {
        const auto& t = lexResult.tokens[i];
        // Marcar errores léxicos con "!"
        std::string marker = t.isError() ? "! " : "  ";
        std::cout << marker
                  << std::setw(W_IDX) << std::left << i
                  << std::setw(W_LIN) << t.line
                  << std::setw(W_COL) << t.column
                  << std::setw(W_SYM) << t.toParserSymbol()
                  << std::setw(W_CAT) << tokenCategory(t)
                  << "'" << t.lexeme << "'\n";
    }
    std::cout << "  " << std::string(W_IDX+W_LIN+W_COL+W_SYM+W_CAT+10, '-') << "\n";

    // ── [3] Gramática ────────────────────────────────────────────────────────
    printSection(3, "Gramática LL(1)");
    std::string gPath = findGrammarFile(grammarFile);
    std::optional<Grammar> gOpt;
    if (!gPath.empty()) {
        gOpt = GrammarParser::parseFile(gPath);
        if (gOpt) std::cout << "  Cargada desde: " << gPath << "\n\n";
    }
    if (!gOpt) {
        std::cout << "  (gramática no encontrada en disco, usando interna)\n\n";
        // Gramática interna de respaldo
        Grammar g;
        g.startSymbol = "P";
        g.nonTerminals = {"P","D","L","L2","T","B","S","S2","E","E2","F"};
        g.ntOrder      = {"P","D","L","L2","T","B","S","S2","E","E2","F"};
        g.terminals    = {"PROGRAM","ID","SEMICOLON","VAR","COLON","INT","FLOAT",
                          "BEGIN","END","ASSIGN","PLUS","MINUS","MULT","NUM","FNUM",
                          "LPAREN","RPAREN"};
        auto S = [&](const std::string& n, bool nt) { return Symbol{n, nt}; };
        g.rules["P"]  = {{S("PROGRAM",false),S("ID",false),S("SEMICOLON",false),S("D",true),S("B",true)}};
        g.rules["D"]  = {{S("VAR",false),S("L",true)}};
        g.rules["L"]  = {{S("ID",false),S("COLON",false),S("T",true),S("SEMICOLON",false),S("L2",true)}};
        g.rules["L2"] = {{S("L",true)},{S("EPS",false)}};
        g.rules["T"]  = {{S("INT",false)},{S("FLOAT",false)}};
        g.rules["B"]  = {{S("BEGIN",false),S("S",true),S("END",false)}};
        g.rules["S"]  = {{S("ID",false),S("ASSIGN",false),S("E",true),S("SEMICOLON",false),S("S2",true)}};
        g.rules["S2"] = {{S("S",true)},{S("EPS",false)}};
        g.rules["E"]  = {{S("F",true),S("E2",true)}};
        g.rules["E2"] = {{S("PLUS",false),S("F",true),S("E2",true)},
                         {S("MINUS",false),S("F",true),S("E2",true)},
                         {S("EPS",false)}};
        g.rules["F"]  = {{S("ID",false)},{S("NUM",false)},{S("FNUM",false)}};
        gOpt = g;
    }
    const Grammar& g = *gOpt;
    displayGrammar(g);

    // ── [4] Validación ───────────────────────────────────────────────────────
    printSection(4, "Validación de la gramática");
    auto valResult = Validator::validate(g);
    if (valResult.valid) {
        std::cout << "  ✓ Gramática bien formada (sin recursión izquierda ni NT indefinidos)\n";
    } else {
        std::cout << "  Advertencias:\n";
        for (auto& e : valResult.errors) std::cout << "    ⚠  " << e << "\n";
    }

    // ── [5] Conjuntos FIRST y FOLLOW ─────────────────────────────────────────
    printSection(5, "Conjuntos FIRST y FOLLOW");
    auto first  = FirstSetCalculator::compute(g);
    auto follow = FollowSetCalculator::compute(g, first);
    if (showSets) {
        displayFirstFollow(g, first, follow);
    } else {
        std::cout << "  Calculados. (usa --sets para verlos)\n";
    }

    // ── [6] Tabla LL(1) ──────────────────────────────────────────────────────
    printSection(6, "Tabla LL(1)");
    auto ll1Result = LL1TableGenerator::build(g, first, follow);
    if (!ll1Result.isValid) {
        std::cout << "  ✗ La gramática NO es LL(1):\n";
        for (auto& e : ll1Result.errors) std::cout << "    - " << e << "\n";
        return 1;
    }
    std::cout << "  ✓ Gramática LL(1) válida. ";
    int entries = 0;
    for (auto& [nt, row] : ll1Result.table)
        for (auto& [t, e] : row) if (e.isValid) entries++;
    std::cout << entries << " entradas.\n";
    if (showTable) {
        std::cout << "\n";
        displayLL1Table(g, ll1Result.table);
    } else {
        std::cout << "  (usa --table para mostrarla)\n";
    }

    // ── [7] Análisis sintáctico ──────────────────────────────────────────────
    printSection(7, "Análisis sintáctico (LL1)");
    LL1Parser parser;
    parser.enableASTBuilding(true);  // AST activado por defecto
    if (trace) {
        parser.setTraceLevel(traceLevel);
        parser.setTraceFile("traza_analisis.txt");
    }
    std::vector<std::string> ntList(g.nonTerminals.begin(), g.nonTerminals.end());
    auto parseResult = parser.parse(lexResult.tokens, ll1Result.table, g.startSymbol, ntList);

    if (parseResult.success) {
        std::cout << "  ✓ Análisis sintáctico exitoso.\n";
    } else {
        std::cout << "  ✗ Errores sintácticos (" << parseResult.errorCount << "):\n";
        for (auto& e : parseResult.errors)
            std::cout << "    - " << e << "\n";
    }

    // ── [8] Análisis semántico ───────────────────────────────────────────────
    printSection(8, "Análisis semántico");
    SemanticAnalyzer semAnalyzer;
    auto semResult = semAnalyzer.analyze(lexResult.tokens);

    if (semResult.valid) {
        std::cout << "  ✓ Sin errores semánticos.\n";
    } else {
        std::cout << "  ✗ Errores semánticos (" << semResult.errors.size() << "):\n";
        for (auto& e : semResult.errors) std::cout << "    - " << e << "\n";
        parseResult.success = false;
    }
    if (!semResult.warnings.empty()) {
        std::cout << "  Advertencias (" << semResult.warnings.size() << "):\n";
        for (auto& w : semResult.warnings) std::cout << "    ⚠  " << w << "\n";
    }

    // ── [9] Tabla de símbolos ────────────────────────────────────────────────
    printSection(9, "Tabla de símbolos");
    displaySymbolTable(semResult);

    // ── [10] AST ─────────────────────────────────────────────────────────────
    if (parseResult.ast) {
        printSection(10, "Árbol Sintáctico (AST)");
        ASTPrinter printer;
        printer.visit(parseResult.ast.get());
        std::cout << printer.getOutput();
    }

    // ── Resumen final ────────────────────────────────────────────────────────
    printHeader("RESULTADO FINAL");
    bool ok = parseResult.success && semResult.valid;
    std::cout << "  Estado   : " << (ok ? "✓ ÉXITO" : "✗ FALLIDO") << "\n";
    int totalErrors = parseResult.errorCount + (int)semResult.errors.size();
    std::cout << "  Errores  : " << totalErrors << "\n";
    std::cout << "  Warnings : " << semResult.warnings.size() << "\n";
    if (trace)
        std::cout << "  Traza    : traza_analisis.txt\n";
    printSeparator("═");
    std::cout << "\n";

    return ok ? 0 : 1;
}
