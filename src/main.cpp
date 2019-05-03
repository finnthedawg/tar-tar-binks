#include <iostream>
#include <string>
#include <vector>
#include "common.h"

int main(int argc, char *argv[]) {

    /* Checking for correct number of arguments */
    if ( argc != 4 ) {
        std::std::cout << "Incorrect Usage: adtar -flag <archive-file> <file/directory list>" << '\n';
    }

    std::cout << "ADTAR WILL NOW COMMENCE\n";

    std::string inputS;
    std::getline(std::cin, inputS);
    std::cout << "It is " << inputS << std::endl;
    return 0;
}
