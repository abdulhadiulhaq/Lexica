#pragma once


#include <string>                                           // for string
#include <iostream>                                         // for cout
#include <algorithm>                                        // for min
#include "../../2_core/engine.cpp"                          // for engine
using std::cin, std::min, std::cout, std::string;


// a command-line interface (CLI) frontend class for Lexica
class CLI 
{
    private:
        Engine engine;

        // helper to print the help menu
        void printHelp() const 
        {
            cout    << "\nAvailable Commands:\n"
                    << "    index <path>                        - Load and index a directory of .txt files\n"
                    << "    suggest <prefix>                    - Get autocomplete suggestions for a prefix\n"
                    << "    search <query> [-k <num>]           - Search the corpus using TF-IDF ranking\n"
                    << "    fuzzy <word>                        - Find spelling corrections using Levenshtein distance\n"
                    << "    stats                               - Display current corpus and indexing statistics\n"
                    << "    help                                - Show this help menu\n"
                    << "    quit (or exit)                      - Exit Lexica CLI\n\n";
        }

        // helper to parse and execute the search command (supporting optional -k flag)
        void handleSearch(const string& args) 
        {
            if (args.empty()) {
                cout << "[!] Error: Search query cannot be empty.\n";
                return;
            }

            size_t topK  {5};                                        // default top-k limit
            string query {args};

            // check if user specified a custom -k flag (e.g., "neural network -k 3")
                auto kPos {args.find("-k")};
                if (kPos != string::npos) 
                {
                    query = args.substr(0, kPos);
                    string kStr {args.substr(kPos + 2)};
                    
                    // trim whitespace from query and kStr
                    while (!query.empty() && query.back() == ' ') query.pop_back();
                    while (!kStr.empty() && kStr.front() == ' ') kStr.erase(kStr.begin());

                    if (!kStr.empty()) 
                    {
                        try {
                            topK = std::stoul(kStr);
                        } catch (...) {
                            topK = 5;
                        }
                    }
                }

            bool cacheHit {false};
            auto results {engine.search(query, topK, cacheHit)};

            cout << "Cache: " << (cacheHit? "HIT" : "MISS") << "\n";

            if (results.empty()) 
                cout << "No matching documents found.\n";

            else {
                for (auto i {0}; i < results.size(); ++i) 
                {
                    const auto& res {results[i]};
                    cout << "  " << (i + 1) << ". " << res.fileName << "     [" << res.score << "]\n";
                }
            }
        }

        // helper to parse and execute the suggest command
        void handleSuggest(const string& prefix) 
        {
            if (prefix.empty()) {
                cout << "[!] Error: Suggest prefix cannot be empty.\n";
                return;
            }

            auto suggestions {engine.suggest(prefix, 5)};
            if (suggestions.empty()) {
                cout << "No suggestions found for prefix: " << prefix << "\n";
                return;
            }

            for (auto i {0}; i < suggestions.size(); ++i) {
                const auto& pair {suggestions[i]};
                cout << "  " << pair.first << "    (" << pair.second << ")\n";
            }
        }

        // helper to parse and execute the fuzzy match command
        void handleFuzzy(const string& term) 
        {
            if (term.empty()) {
                cout << "[!] Error: Fuzzy term cannot be empty.\n";
                return;
            }

            string bestMatch {"lib"};
            int    distance  {0};

            if (engine.fuzzyMatch(term, bestMatch, distance, 3)) {
                if (!distance) 
                    cout << "Exact match found: " << term << " (distance 0)\n";
                
                else 
                    cout << "Did you mean: " << bestMatch << " (distance " << distance << ")\n";
            } 

            else 
                cout << "No close matches found for: " << term << "\n";                             // ie distance > maxTolerance
        }

        void dispatchCommmand(const string& cmd, const string& args){
            // index
            if (cmd == "index") 
            {
                if (args.empty()) {
                    cout << "[!] Error: Missing directory path. Usage: index <path>\n";
                    return;
                }

                cout << "Reading files from: " << args << "...\n";
                auto stats {engine.initialize(args)};

                if (!stats.fileCount) 
                    cout << "[!] Error: No documents found or failed to load directory.\n";

                else {
                    cout << "Indexed "       << stats.fileCount      << " files\n";
                    cout << "Total tokens: " << stats.totalTokens    << "\n";
                    cout << "Unique terms: " << stats.uniqueTerms    << "\n";
                    cout << "Indexed in "    << stats.elapsedSeconds << " s\n";
                }
            } 
    
            // suggest
            else if (cmd == "suggest") {
                handleSuggest(args);
            } 

            // search
            else if (cmd == "search") {
                handleSearch(args);
            } 

            // fuzzy
            else if (cmd == "fuzzy") {
                handleFuzzy(args);
            } 

            // stats
            else if (cmd == "stats") {
                cout << "Document Count: " << engine.getDocumentCount() << "\n";
                cout << "Unique Terms:   " << engine.getUniqueTermCount() << "\n";
            } 

            // exit
            else if (cmd == "quit" || cmd == "exit") {
                cout << "Shutting down Lexica. Goodbye!\n";
                return;
            } 

            // help
            else if (cmd == "help") {
                printHelp();
            } 

            // invalid command
            else {
                cout << "[!] Unknown command: '" << cmd << "'. Type 'help' for instructions.\n";
            }
        }

    public:
        CLI() 
            {}

        // starts the interactive terminal loop
        void run() 
        {
            // cout << "========================================\n";
            // cout << "       LEXICA SEARCH ENGINE v1.0        \n";
            // cout << "========================================\n";
            // cout << "Type 'help' to see available commands.\n\n";

            string line {""};
            while (true) 
            {
                cout << "lexica> ";

                if (!std::getline(cin, line)) break;
                if (line.empty())             continue;

                // parse command and arguments
                    string cmd      {""};
                    string args     {""};
                    auto   spacePos {line.find(' ')};

                    if (spacePos == string::npos)                                       // if the cmd contains no spaces             
                        cmd = line;

                    else 
                    {
                        cmd  = line.substr(0, spacePos);                                // the command, if valid, will be from the start of the line till before the first space char
                        args = line.substr(spacePos + 1);                               // the args, if any, will be after the first space char

                        // trim leading spaces from args
                        while (!args.empty() && (args.front() == ' ')) 
                            args.erase(args.begin());                                   // erase the first char
                    }

                // dispatch command
                dispatchCommmand(cmd, args);
            }
        }
};