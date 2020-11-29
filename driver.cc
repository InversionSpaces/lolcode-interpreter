#include "driver.hh"
#include "ast.hh"
#include <string>

namespace lol{
driver::driver() :
    scanner(),
    parser(scanner, *this) {}

ast::AST driver::parse(const std::string& fname) {
    stream.open(fname);
    scanner.restart(fname, stream);
    parser();
    stream.close();
    return result;
}
} // namespace lol