//
// Created by Yaroslav239 on 20.08.18.
//

#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include "tree.h"
#include "encoder.h"
#include "decoder.h"

void print_help() {
    std::cout << "Usage: huffman [OPTION] [SOURCE] [DESTINATION]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "\t-e, --encode: encode [SOURCE] file and write it to [DESTINATION] file" << std::endl;
    std::cout << "\t-d, --decode: decode [SOURCE] file and write it to [DESTINATION] file" << std::endl;
}

void print_error(std::string message) {
    std::cerr << "Error! " << message << std::endl;
    std::cout.flush();
    print_help();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_error("Wrong number of arguments: " + std::to_string(argc));
        return 0;
    }
    std::string option = argv[1];
    std::string src_name = argv[2];
    std::string dst_name = argv[3];
    if (option == "-e" || option == "--encode") {
        try {
            encoder e(src_name, dst_name);
            e.encode();
        }
        catch (std::exception &ex) {
            print_error(ex.what());
            return 1;
        }
    } else if (option == "-d" || option == "--decode") {
        try {
            decoder d(src_name, dst_name);
            d.decode();
        }
        catch (std::exception &ex) {
            print_error(ex.what());
            return 1;
        }
    } else {
        print_error("Unknown option: " + std::string(option));
        return 0;
    }
    return 0;
}