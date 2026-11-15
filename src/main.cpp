#include <iostream>
#include "3_frontEnd/1_cli/cli.hpp"
#include "3_frontEnd/2_webGUI/gui.hpp"
using std::cout;


int main(int argc, char* argv[]) 
{
    if (argc == 0){
        CLI cli;
        cli.run();
    }
    else if (argc == 1){
        if (argv[1] == "--cli"){
            CLI cli;
            cli.run();
        }
        else if (argv[1] == "--gui"){
            GUI gui;
            gui.run();
        }
        else{
            cout << "[!] Invalid flag.\n";
        }
    }
    else{
        cout << "[!] Invalid number of commands.\n";
    }
}