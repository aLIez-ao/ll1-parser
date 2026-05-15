add_library(lexer STATIC
    lexer/lexer.cpp
    token/token.cpp
    utils/utils.cpp
    symbol_table/symbol_table.cpp
    error_handler/error_handler.cpp
)

target_include_directories(lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)