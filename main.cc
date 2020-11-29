#include <iostream>

#include "scanner.hh"
#include "parser.hh"

int main() {
    lol::scanner scanner;
    lol::parser parser(scanner);

    parser();
}