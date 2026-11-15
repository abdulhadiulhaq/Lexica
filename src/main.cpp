#include <iostream>                                     // for cout
#include "3_frontEnd/1_cli/cli.hpp"                     // for CLI
#include "3_frontEnd/2_webGUI/gui.hpp"                  // for GUI
using std::cout;


int main(int argc, char* argv[]) 
{
    switch(argc)
    {
        case 1:                                                 // zero flags provided; defaults to the cli version if no flags are provided
        {
            CLI cli;
            cli.run();

            return 0;
        }

        case 2:                                                 // one flag provided
        {
            if (argv[1] == "--cli") {
                CLI cli;
                cli.run();
            } 

            else if (argv[1] == "--gui") {
                GUI gui;
                gui.run();
            } 

            else {
                cout    << "[!] Invalid flag: " << argv[1] << "\n"
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