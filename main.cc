#include <iostream>
#include <fstream>
#include <string>

#include "driver.hh"
#include "ast.hh"

int main(int argc, char* argv[]) {
    if (argc < 1)
        return 1;

    if (argc != 2) {
        std::cout 
            << "Usage: " 
            << argv[0]
            << " SCRIPT_FILE"
            << endl;
        return 0;
    }

    std::string fname(argv[1]);
    lol::driver driver;

    ast::AST result = driver.parse(fname);
    ast::PrintVisitor visitor;
    visitor.traverse(result);
}