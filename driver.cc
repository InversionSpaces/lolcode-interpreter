#include "driver.hh"
#include <string>

namespace lol{
driver::driver() :
    scanner(),
    parser(scanner, *this) {}

void driver::parse(const std::string& fname) {
    stream.open(fname);
    scanner.restart(fname, stream);
    parser();
    stream.close();
}
} // namespace lol