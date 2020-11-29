#include <iostream>
#include <fstream>
#include <string>

#include "scanner.hh"
#include "parser.hh"
#include "driver.hh"

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

    driver.parse(fname);
}