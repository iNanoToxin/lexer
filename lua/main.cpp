#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Parser.h"

int main() {


    // std::string source;
    // {
    //     std::ifstream file("../tests/test_2.lua");
    //     assert(file.is_open(), "failed to open file");
    //
    //     std::stringstream stream;
    //     stream << file.rdbuf();
    //     source = stream.str();
    //     file.close();
    // }
    //
    // generator generator;
    // std::string generated = generator.generate(source);
    //
    // {
    //     std::ofstream file("../tests/output.lua");
    //     assert(file.is_open(), "Failed to open the file.");
    //     file << generated;
    //     file.close();
    // }
    // std::cout << generated;

    Parser p;
    p.g();

    return 0;
}