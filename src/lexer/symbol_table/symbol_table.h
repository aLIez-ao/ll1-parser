#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <ostream>
#include "../token/token_type.h"

/**
 * @file symbol_table.h
 * @brief Declaración de la clase SymbolTable y la estructura SymbolEntry.
 *
 * @details
 * Este módulo implementa la Tabla de Símbolos del analizador léxico.
 * La tabla almacena únicamente identificadores encontrados durante
 * el análisis, sin repetir entradas y registrando la primera línea
 * de aparición de cada uno.
 *
 * Estructura de la tabla:
 * @code
 * ┌─────────────┬───────────┬────────┐
 * │   Lexema    │   Token   │ Línea  │
 * ├─────────────┼───────────┼────────┤
 * │ suma        │ IDENT     │   1    │
 * │ promedio    │ IDENT     │   2    │
 * │ resultado   │ IDENT     │   4    │
 * └─────────────┴───────────┴────────┘
 * @endcode
 *
 * @author  Equipo Lexer
 * @version 1.0
 */


/**
 * @struct SymbolEntry
 * @brief Representa una entrada en la tabla de símbolos.
 *
 * @details
 * Cada entrada corresponde a un identificador único encontrado
 * en el código fuente. Guarda el lexema, su tipo de token,
 * la línea de primera aparición y la columna donde fue detectado.
 */
struct SymbolEntry {

    /**
     * @brief Texto original del identificador tal como aparece
     *        en el código fuente.
     * @example "suma", "promedio", "resultado"
     */
    std::string lexeme;

    /**
     * @brief Tipo de token asignado al identificador.
     * @details Siempre será TokenType::IDENT para esta tabla,
     *          ya que solo se almacenan identificadores.
     */
    TokenType type;

    /**
     * @brief Número de línea donde el identificador aparece
     *        por primera vez en el código fuente.
     * @note  La numeración inicia en 1.
     */
    int line;

    /**
     * @brief Número de columna donde el identificador inicia
     *        en su primera aparición.
     * @note  La numeración inicia en 1.
     */
    int column;

    /**
     * @brief Constructor completo de SymbolEntry.
     *
     * @param lexeme  Texto del identificador.
     * @param type    Tipo de token (generalmente IDENT).
     * @param line    Línea de primera aparición (desde 1).
     * @param column  Columna de primera aparición (desde 1).
     */
    SymbolEntry(const std::string& lexeme,
                TokenType type,
                int line,
                int column)
        : lexeme(lexeme)
        , type(type)
        , line(line)
        , column(column)
    {}

    /**
     * @brief Convierte la entrada a una línea formateada.
     *
     * @details Genera una línea con columnas alineadas lista
     *          para imprimir en la tabla de símbolos.
     *
     * Formato:
     * @code
     *   suma           IDENT       1
     * @endcode
     *
     * @return String con la entrada formateada.
     */
    std::string toString() const;
};


/**
 * @class SymbolTable
 * @brief Tabla de símbolos del analizador léxico.
 *
 * @details
 * Almacena y gestiona todos los identificadores encontrados
 * durante el análisis léxico. Garantiza:
 *  - Sin entradas duplicadas (se conserva solo la primera aparición).
 *  - Búsqueda eficiente por lexema.
 *  - Reporte tabulado para visualización.
 *
 * Uso típico:
 * @code
 *   SymbolTable table;
 *
 *   // Agregar identificadores
 *   table.add("suma",      TokenType::IDENT, 1, 5);
 *   table.add("promedio",  TokenType::IDENT, 2, 7);
 *   table.add("suma",      TokenType::IDENT, 3, 1); // ignorado: ya existe
 *
 *   // Consultar
 *   if (table.exists("suma")) { ... }
 *
 *   // Imprimir tabla
 *   table.print(std::cout);
 * @endcode
 */
class SymbolTable {
public:

    /**
     * @brief Constructor por defecto.
     *        Inicializa la tabla vacía.
     */
    SymbolTable();

    /**
     * @brief Destructor por defecto.
     */
    ~SymbolTable() = default;

    // =========================================================
    //  Inserción
    // =========================================================

    /**
     * @brief Agrega un identificador a la tabla si no existe.
     *
     * @details
     * Si el lexema ya está registrado, la llamada se ignora
     * silenciosamente y se conserva la entrada original
     * (primera aparición).
     *
     * Solo se agregan tokens de tipo IDENT. Si se recibe otro
     * tipo, la llamada se ignora.
     *
     * @param lexeme  Texto del identificador.
     * @param type    Tipo de token (debe ser TokenType::IDENT).
     * @param line    Línea de aparición en el código fuente.
     * @param column  Columna de inicio del identificador.
     *
     * @return true  si fue agregado exitosamente.
     * @return false si ya existía o el tipo no es IDENT.
     */
    bool add(const std::string& lexeme,
             TokenType type,
             int line,
             int column);

    // =========================================================
    //  Consulta
    // =========================================================

    /**
     * @brief Verifica si un identificador ya está en la tabla.
     *
     * @param lexeme  Texto del identificador a buscar.
     * @return true   si el identificador existe en la tabla.
     * @return false  si no ha sido registrado.
     */
    bool exists(const std::string& lexeme) const;

    /**
     * @brief Busca y retorna una entrada por su lexema.
     *
     * @details
     * Retorna un puntero constante a la entrada encontrada.
     * Si no existe, retorna nullptr. El llamador debe verificar
     * el puntero antes de usarlo.
     *
     * @code
     *   const SymbolEntry* entry = table.find("suma");
     *   if (entry) {
     *       std::cout << entry->line;
     *   }
     * @endcode
     *
     * @param lexeme  Texto del identificador a buscar.
     * @return        Puntero constante a la entrada, o nullptr.
     */
    const SymbolEntry* find(const std::string& lexeme) const;

    /**
     * @brief Retorna el número de entradas en la tabla.
     *
     * @return Entero con el total de identificadores únicos.
     */
    int getSize() const;

    /**
     * @brief Indica si la tabla está vacía.
     *
     * @return true  si no hay ninguna entrada registrada.
     * @return false si hay al menos una entrada.
     */
    bool isEmpty() const;

    /**
     * @brief Retorna todas las entradas de la tabla (solo lectura).
     *
     * @details Útil para tests o procesamiento externo
     *          sin exponer la estructura interna.
     *
     * @return Referencia constante al vector de SymbolEntry.
     */
    const std::vector<SymbolEntry>& getEntries() const;

    // =========================================================
    //  Modificación
    // =========================================================

    /**
     * @brief Elimina todos los registros de la tabla.
     *
     * @details Permite reutilizar la misma instancia para
     *          múltiples análisis sin crear objetos nuevos.
     */
    void clear();

    // =========================================================
    //  Reporte
    // =========================================================

    /**
     * @brief Imprime la tabla de símbolos completa a un stream.
     *
     * @details
     * Genera una tabla con encabezado, separadores y todas
     * las entradas alineadas por columnas.
     *
     * Formato de salida:
     * @code
     * -----------------------------------------------
     *        TABLA DE SÍMBOLOS
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
     * Si la tabla está vacía imprime un mensaje indicándolo.
     *
     * @param out  Stream de salida (std::cout, archivo, etc.)
     */
    void print(std::ostream& out) const;

    /**
     * @brief Imprime solo el resumen de la tabla.
     *
     * @details Muestra únicamente el total de identificadores
     *          sin listar las entradas individuales.
     *
     * @param out  Stream de salida.
     */
    void printSummary(std::ostream& out) const;

private:

    /**
     * @brief Almacenamiento interno de las entradas.
     *
     * @details Se usa vector para mantener el orden de inserción,
     *          lo que facilita la lectura del reporte final.
     */
    std::vector<SymbolEntry> entries_;

    /**
     * @brief Busca el índice de un lexema en el vector interno.
     *
     * @details Método auxiliar usado por exists() y find()
     *          para no duplicar la lógica de búsqueda.
     *
     * @param lexeme  Texto del identificador a buscar.
     * @return        Índice en entries_ si existe, -1 si no.
     */
    int findIndex(const std::string& lexeme) const;
};

#endif // SYMBOL_TABLE_H