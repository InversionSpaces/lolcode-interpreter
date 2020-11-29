#pragma once

#include <fstream>
#include <string>

#include "location.hh"
#include "scanner.hh"
#include "parser.hh"
#include "ast.hh"

namespace lol {
class driver {
private:
    lol::scanner scanner;
    lol::parser parser;

    std::ifstream stream;
    ast::AST result;
public:
    driver();

    ast::AST parse(const std::string& fname);

    // parser needs our location
    friend class lol::parser;
};
} // namespace lol