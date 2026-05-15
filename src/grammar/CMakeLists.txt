add_library(grammar STATIC
    core/grammar.cpp
    io/grammar_parser.cpp
    analysis/validator.cpp
    analysis/first_set.cpp
    analysis/follow_set.cpp
    analysis/ll1_table.cpp
)

target_include_directories(grammar PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)