// Cmake test program
//
// @author teirm
// @date   2021-03-22

#include <cpp/parse_flags.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    int a = 0;
    parse_flags::ParseFlags parser;
    parser.add_message("A test program using the C++ parse flags lib");
    parser.add_flag("wombat-fish", a, "an amazing int");
    parser.parse_args(argc, argv);
    
    std::cout << "a = " << a << '\n';

    return 0;
}
