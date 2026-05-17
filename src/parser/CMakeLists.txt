add_library(parser STATIC
    parser.cpp
    ast/ast_node.cpp
    ast/ast_builder.cpp
    ast/ast_visitor.cpp
    trace/trace_manager.cpp
    trace/trace_formatter.cpp
    symbol_manager.cpp
    semantic/semantic_analyzer.cpp
)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/lexer
    ${CMAKE_SOURCE_DIR}/src/lexer/token
    ${CMAKE_SOURCE_DIR}/src/grammar
    ${CMAKE_SOURCE_DIR}/src/grammar/core
    ${CMAKE_SOURCE_DIR}/src/grammar/analysis
    ${CMAKE_SOURCE_DIR}/src/grammar/io
)