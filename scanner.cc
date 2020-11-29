#include "scanner.hh"

namespace lol {
void scanner::restart(
    const std::string &fname,
    std::ifstream &file
) {
    loc.initialize(&fname);
    yyrestart(&file);
}
}