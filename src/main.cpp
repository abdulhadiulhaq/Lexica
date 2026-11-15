#include <string>                                       // for string
#include <iostream>                                     // for cout
#include "3_frontEnd/1_cli/cli.hpp"                     // for CLI
#include "3_frontEnd/2_webGUI/gui.hpp"                  // for GUI
using std::cout, std::string;


int main(int argc, char* argv[]) 
{
    switch(argc)
    {
        case 1:                                                 // zero flags provided; defaults to the cli version
        {
            CLI cli;
            cli.run();

            return 0;
        }

        case 2:                                                 // one flag provided
        {
            string flag {argv[1]};                              // wrap char* into std::string for correct comparison

            if (flag == "--cli") {
                CLI cli;
                cli.run();
            } 

            else if (flag == "--gui") {
                GUI gui;
                gui.run();
            } 

            else {
                cout    << "[!] Invalid flag: " << flag << "\n"
                        << "Usage: ./lexica [--cli | --gui]\n";

                return 1;
            }

            return 0;
        }

        default:                                                // >= 2 arguments provided
        {
            cout    << "[!] Invalid number of arguments.\n"
                    << "Usage: ./lexica [--cli | --gui]\n";

            return 1;
        }
    }
}