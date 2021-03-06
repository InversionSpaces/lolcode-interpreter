#include <string>

#include "driver.hh"
#include "ast.hh"

namespace lol{
driver::driver() :
    scanner(),
    parser(scanner, *this) {
    parser.set_debug_level(1);
}

ast::AST driver::parse(const std::string& fname) {
    stream.open(fname);
    scanner.restart(fname, stream);
    parser();
    stream.close();
    return result;
}
} // namespace lol