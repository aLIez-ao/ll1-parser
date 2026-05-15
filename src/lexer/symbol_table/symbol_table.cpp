/**
 * @file symbol_table.cpp
 * @brief Implementación de la clase SymbolTable y SymbolEntry.
 *
 * @details
 * Implementa todas las operaciones de la tabla de símbolos:
 * inserción sin duplicados, búsqueda por lexema, reporte
 * tabulado y limpieza para reutilización.
 *
 * Decisiones de implementación:
 *  - Se usa std::vector en lugar de std::map para mantener
 *    el orden de inserción (aparición en el código fuente).
 *  - La búsqueda es lineal O(n), aceptable para programas
 *    pequeños como los de esta actividad.
 *  - Solo se aceptan tokens de tipo IDENT.
 *
 * @author  Equipo Lexer
 * @version 1.0
 */

#include "symbol_table.h"
#include "../token/token_type.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// =============================================================
//  SymbolEntry — Implementación
// =============================================================

/**
 * @brief Convierte la entrada a una línea formateada para imprimir.
 *
 * @details
 * Genera la fila de la tabla con columnas fijas usando
 * std::setw para alineación. El formato es:
 * @code
 *   suma           IDENT         1       5
 * @endcode
 *
 * @return String formateado con la información de la entrada.
 */
std::string SymbolEntry::toString() const {
    std::ostringstream oss;
    oss << "  "
        << std::left  << std::setw(16) << lexeme
        << std::left  << std::setw(12) << tokenTypeToString(type)
        << std::right << std::setw(5)  << line
        << std::right << std::setw(8)  << column;
    return oss.str();
}


// =============================================================
//  SymbolTable — Constructor
// =============================================================

/**
 * @brief Constructor por defecto.
 *        Inicializa el vector de entradas vacío.
 */
SymbolTable::SymbolTable() {
    entries_.clear();
}


// =============================================================
//  SymbolTable — Métodos públicos: Inserción
// =============================================================

/**
 * @brief Agrega un identificador a la tabla si no existe.
 *
 * @details
 * Flujo de decisión:
 * @code
 *  add("suma", IDENT, 1, 5)
 *       │
 *       ├─ ¿type != IDENT? → retorna false (ignorado)
 *       │
 *       ├─ ¿"suma" ya existe? → retorna false (sin duplicado)
 *       │
 *       └─ No existe → inserta → retorna true
 * @endcode
 *
 * @param lexeme  Texto del identificador.
 * @param type    Tipo de token. Solo IDENT es aceptado.
 * @param line    Línea de primera aparición.
 * @param column  Columna de primera aparición.
 *
 * @return true  si fue insertado correctamente.
 * @return false si ya existía o el tipo no es IDENT.
 */
bool SymbolTable::add(const std::string& lexeme,
                       TokenType type,
                       int line,
                       int column)
{
    // Solo aceptamos identificadores en la tabla de símbolos
    if (type != TokenType::IDENT) {
        return false;
    }

    // Si el lexema ya está registrado, no se duplica
    if (exists(lexeme)) {
        return false;
    }

    // Insertar la nueva entrada al final del vector
    entries_.emplace_back(lexeme, type, line, column);
    return true;
}


// =============================================================
//  SymbolTable — Métodos públicos: Consulta
// =============================================================

/**
 * @brief Verifica si un identificador ya está en la tabla.
 *
 * @details Delega la búsqueda a findIndex() y evalúa
 *          si el índice retornado es válido (distinto de -1).
 *
 * @param lexeme  Texto del identificador a buscar.
 * @return true   si el identificador existe.
 * @return false  si no ha sido registrado.
 */
bool SymbolTable::exists(const std::string& lexeme) const {
    return findIndex(lexeme) != -1;
}

/**
 * @brief Busca y retorna un puntero a la entrada por su lexema.
 *
 * @details
 * Usa findIndex() para obtener el índice. Si es válido,
 * retorna un puntero al elemento del vector. Si no existe,
 * retorna nullptr.
 *
 * @warning No almacenar el puntero retornado más allá del
 *          scope actual, ya que puede invalidarse si se
 *          agrega otra entrada al vector.
 *
 * @param lexeme  Texto del identificador a buscar.
 * @return        Puntero constante a la entrada, o nullptr.
 */
const SymbolEntry* SymbolTable::find(const std::string& lexeme) const {
    int idx = findIndex(lexeme);
    if (idx == -1) return nullptr;
    return &entries_[idx];
}

/**
 * @brief Retorna el número de entradas únicas en la tabla.
 *
 * @return Entero con el total de identificadores registrados.
 */
int SymbolTable::getSize() const {
    return static_cast<int>(entries_.size());
}

/**
 * @brief Indica si la tabla no tiene ninguna entrada.
 *
 * @return true  si entries_ está vacío.
 * @return false si hay al menos una entrada.
 */
bool SymbolTable::isEmpty() const {
    return entries_.empty();
}

/**
 * @brief Retorna todas las entradas de la tabla (solo lectura).
 *
 * @details Exposición controlada del vector interno.
 *          No permite modificaciones externas.
 *
 * @return Referencia constante a entries_.
 */
const std::vector<SymbolEntry>& SymbolTable::getEntries() const {
    return entries_;
}


// =============================================================
//  SymbolTable — Métodos públicos: Modificación
// =============================================================

/**
 * @brief Elimina todas las entradas del vector interno.
 *
 * @details Llama a std::vector::clear() sobre entries_.
 *          Después de clear(), getSize() retorna 0
 *          e isEmpty() retorna true.
 */
void SymbolTable::clear() {
    entries_.clear();
}


// =============================================================
//  SymbolTable — Métodos públicos: Reporte
// =============================================================

/**
 * @brief Imprime la tabla de símbolos completa con formato tabulado.
 *
 * @details
 * Estructura del reporte impreso:
 * @code
 * -----------------------------------------------
 *         TABLA DE SÍMBOLOS
 * -----------------------------------------------
 *  #    Lexema          Token       Línea   Col
 * -----------------------------------------------
 *  1    suma            IDENT         1       5
 *  2    promedio        IDENT         2       7
 *  3    resultado       IDENT         4       5
 * -----------------------------------------------
 *  Total: 3 identificador(es)
 * -----------------------------------------------
 * @endcode
 *
 * Recorre entries_ e imprime cada SymbolEntry usando
 * su método toString() con alineación de columnas fijas.
 *
 * @param out  Stream de salida (std::cout, archivo, etc.)
 */
void SymbolTable::print(std::ostream& out) const {
    const std::string separator(50, '-');

    out << "\n" << separator << "\n";
    out << "         TABLA DE SÍMBOLOS\n";
    out << separator << "\n";

    if (entries_.empty()) {
        out << "  (Tabla vacía — no se encontraron identificadores)\n";
        out << separator << "\n";
        return;
    }

    // Encabezado de columnas
    out << "  "
        << std::left  << std::setw(16) << "Lexema"
        << std::left  << std::setw(12) << "Token"
        << std::right << std::setw(5)  << "Línea"
        << std::right << std::setw(8)  << "Col"
        << "\n";
    out << separator << "\n";

    // Filas de la tabla
    int index = 1;
    for (const auto& entry : entries_) {
        out << std::right << std::setw(2) << index++ << ".  ";
        out << entry.toString() << "\n";
    }

    out << separator << "\n";
    printSummary(out);
    out << separator << "\n";
}

/**
 * @brief Imprime solo el resumen de la tabla.
 *
 * @details Muestra el total de identificadores únicos registrados
 *          sin listar las entradas individuales.
 *
 * Formato:
 * @code
 *  Total: 3 identificador(es)
 * @endcode
 *
 * @param out  Stream de salida.
 */
void SymbolTable::printSummary(std::ostream& out) const {
    out << "  Total: " << entries_.size()
        << " identificador(es) único(s).\n";
}


// =============================================================
//  SymbolTable — Métodos privados
// =============================================================

/**
 * @brief Busca el índice de un lexema en el vector entries_.
 *
 * @details
 * Realiza búsqueda lineal O(n) comparando el lexema de cada
 * entrada. Método auxiliar centralizado para no duplicar
 * la lógica de búsqueda en exists() y find().
 *
 * Complejidad: O(n) donde n = número de entradas.
 *
 * @param lexeme  Texto del identificador a buscar.
 * @return        Índice en entries_ si fue encontrado.
 * @return        -1 si el lexema no existe en la tabla.
 */
int SymbolTable::findIndex(const std::string& lexeme) const {
    for (int i = 0; i < static_cast<int>(entries_.size()); i++) {
        if (entries_[i].lexeme == lexeme) {
            return i;
        }
    }
    return -1;
}