#pragma once

#include <fstream>
#include <string>

#include "location.hh"
#include "scanner.hh"
#include "parser.hh"

namespace lol {
class driver {
private:
    lol::scanner scanner;
    lol::parser parser;

    std::ifstream stream;
public:
    driver();

    void parse(const std::string& fname);

    // parser needs our location
    friend class lol::parser;
};
} // namespace lol