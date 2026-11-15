#pragma once


#include <string>                                       // for string
#include "../../2_core/engine.cpp"                      // for Engine
using std::string;


// a web-based GUI frontend class for Lexica using cpp-httplib
class GUI 
{
    private:
        Engine engine;
        string webRoot;
        string defaultDataDir;
        int    port;

        // helper to escape special characters for valid JSON output
        static string escapeJson(const string& input);

    public:
        GUI(const string& staticDir = "../src/3_frontEnd/2_webGUI/web", 
            const string& dataDir   = "../data/sample", 
            int serverPort          = 8080);

        // initializes the corpus and starts the local HTTP server
        void run();
};