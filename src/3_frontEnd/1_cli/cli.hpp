#pragma once


#include <string>                                           // for string
#include <thread>                                           // for sleep_for
#include <chrono>                                           // for milliseconds
#include <iomanip>                                          // for setprecision, fixed
#include <iostream>                                         // for cout
#include <algorithm>                                        // for min
#include "include/rang.hpp"                                 // for namespace rang
#include "../../2_core/engine.hpp"                          // for engine
using std::cin, std::min, std::cout, std::string;


// a command-line interface (CLI) frontend class for Lexica
class CLI 
{
    private:
        Engine engine;
        string programName;

        // helper to print the help menu
        void printHelp() const 
        {
            cout    << rang::fg::green << "\n[INFO] " << rang::style::reset << "Lexica Help & Available Commands:\n"
                    << "--------------------------------------------------------------------------------\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "index <path>"                << rang::style::reset << "          - Load and index a directory of .txt files\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "suggest <prefix>"            << rang::style::reset << "        - Get autocomplete suggestions for a prefix\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "search <query> [-k <num>]"   << rang::style::reset << "       - Search the corpus using TF-IDF ranking\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "fuzzy <word>"                << rang::style::reset << "          - Find spelling corrections using Levenshtein distance\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "stats"                       << rang::style::reset << "            - Display current corpus and indexing statistics\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "help"                        << rang::style::reset << "             - Show this help menu\n"
                    <<   "  "     <<  rang::fgB::cyan     <<   "quit (or exit)"              << rang::style::reset << "         - Exit Lexica CLI\n"
                    << "--------------------------------------------------------------------------------\n";
        }

        // helper to parse and execute the search command (supporting optional -k flag)
        void handleSearch(const string& args) 
        {
            if (args.empty()) {
                cout << rang::fg::yellow << "\n[!] " << rang::style::reset << "Error: Search query cannot be empty.\n";
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

            cout    << rang::fg::green << "\n[INFO] " << rang::style::reset << "Search Results for: \"" << rang::style::bold << query << rang::style::reset << "\"\n"

                    << "--------------------------------------------------\n"
                    << "  " << rang::fgB::cyan << "Cache Status" << rang::style::reset << " : " 
                    << (cacheHit? rang::fg::green : rang::fg::yellow) << (cacheHit? "HIT" : "MISS") << rang::style::reset << "\n"
                    << "--------------------------------------------------\n";

            if (results.empty())
                cout << "  No matching documents found.\n";
            
            else {
                for (auto i {0}; i < results.size(); ++i) 
                {
                    const auto& res {results[i]};
                    cout    << "  " << rang::fgB::cyan << (i + 1) << "." << rang::style::reset << " " << res.fileName 
                            << "  " << rang::fg::green << "[" << std::fixed << std::setprecision(4) << res.score << "]" << rang::style::reset << "\n";
                }
            }

            cout << "--------------------------------------------------\n";
        }

        // helper to parse and execute the suggest command
        void handleSuggest(const string& prefix) 
        {
            if (prefix.empty()) {
                cout << rang::fg::yellow << "\n[!] " << rang::style::reset << "Error: Suggest prefix cannot be empty.\n";
                return;
            }

            auto suggestions {engine.suggest(prefix, 5)};
            cout    << rang::fg::green << "\n[INFO] " << rang::style::reset << "Autocomplete Suggestions for prefix: \"" << rang::style::bold << prefix << rang::style::reset << "\"\n"
                    << "--------------------------------------------------\n";

            if (suggestions.empty())
                cout << "  No suggestions found for prefix.\n";

            else {
                for (auto i {0}; i < suggestions.size(); ++i) {
                    const auto& pair {suggestions[i]};

                    cout    << "  " << rang::fgB::cyan << (i + 1) << "." << rang::style::reset << " " << pair.first 
                            << "  " << rang::fg::green << "(freq: " << pair.second << ")" << rang::style::reset << "\n";
                }
            }

            cout << "--------------------------------------------------\n";
        }

        // helper to parse and execute the fuzzy match command
        void handleFuzzy(const string& term) 
        {
            if (term.empty()) {
                cout << rang::fg::yellow << "\n[!] " << rang::style::reset << "Error: Fuzzy term cannot be empty.\n";
                return;
            }

            string bestMatch {"lib"};
            int    distance  {0};

            cout    << rang::fg::green << "\n[INFO] " << rang::style::reset << "Spell Check / Fuzzy Search for: \"" << rang::style::bold << term << rang::style::reset << "\"\n"
                    << "--------------------------------------------------\n";

            if (engine.fuzzyMatch(term, bestMatch, distance, 3)) {
                if (!distance) 
                    cout << "  Exact match found: " << rang::fgB::cyan << term << rang::style::reset << " (distance 0)\n";
                else 
                    cout << "  Did you mean: " << rang::fgB::cyan << bestMatch << rang::style::reset << " (distance " << distance << ")\n";
            } 

            else
                cout << "  No close matches found for: " << term << "\n";

            cout << "--------------------------------------------------\n";
        }

        void printIndexStats(const int fileCount, const size_t totalTokens, const size_t uniqueTerms, const double elapsedSeconds) {
            cout    << rang::fg::green << "\n[INFO] " << rang::style::reset 
                    << "Indexing completed successfully!\n"

                    << "--------------------------------------------------\n"
                    << "  " << rang::fgB::cyan << "Files Indexed" << rang::style::reset << " : " << fileCount   << "\n"
                    << "  " << rang::fgB::cyan << "Total Tokens " << rang::style::reset << " : " << totalTokens << "\n"
                    << "  " << rang::fgB::cyan << "Unique Terms " << rang::style::reset << " : " << uniqueTerms << "\n"
                    << "  " << rang::fgB::cyan << "Time Elapsed " << rang::style::reset << " : "
                    << std::fixed << std::setprecision(5) << elapsedSeconds << " s\n"
                    << "--------------------------------------------------\n";
        }

        bool dispatchCommmand(const string& cmd, const string& args){
            // index
            if (cmd == "index") 
            {
                if (args.empty()) {
                    cout << rang::fg::yellow << "[!] " << rang::style::reset << "Error: Missing directory path. Usage: index <path>\n";
                }
                
                else {
                    cout << rang::fg::green << "[INFO] " << rang::style::reset << "Reading files from: " << rang::style::bold << args << rang::style::reset << "...\n";
                    auto stats {engine.initialize(args)};

                    if (!stats.fileCount) 
                        cout << rang::fg::yellow << "[!] " << rang::style::reset << "Error: No documents found or failed to load directory.\n";
                    else
                        printIndexStats(stats.fileCount, stats.totalTokens, stats.uniqueTerms, stats.elapsedSeconds);
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
                cout    << rang::fg::green << "\n[INFO] " << rang::style::reset 
                        << "Corpus & Engine Statistics\n"

                        << "--------------------------------------------------\n"
                        << "  " << rang::fgB::cyan << "Document Count" << rang::style::reset << " : " << engine.getDocumentCount()   << "\n"
                        << "  " << rang::fgB::cyan << "Unique Terms  " << rang::style::reset << " : " << engine.getUniqueTermCount() << "\n"
                        << "--------------------------------------------------\n";
            }

            // exit
            else if (cmd == "quit" || cmd == "exit") {
                cout << rang::fg::green << "Shutting down Lexica. Goodbye!\n" << rang::style::reset;
                return false;
            } 

            // help
            else if (cmd == "help") {
                printHelp();
            } 

            // invalid command
            else {
                cout    << rang::fg::yellow  << "[!] " << rang::style::reset 
                        << "Unknown command: '" 
                        << rang::style::bold << cmd    << rang::style::reset 
                        << "'. Type 'help' for instructions.\n";    
            }

            return true;
        }

        void printProgramName(const int pauseDuration = 100){
            cout << rang::fgB::green;
            cout << rang::style::bold;

            for (const auto& ch : (programName + "> ")){
                cout << ch << std::flush;

                std::this_thread::sleep_for(std::chrono::milliseconds(pauseDuration));                    // sleep/pause for 100 ms
            }

            cout << rang::style::reset;
        }

    public:
        CLI(const string& programName = "Lexica") 
            : programName(programName)
            {}

        // starts the interactive terminal loop
        void run() 
        {
            // cout << "========================================\n";
            // cout << "       LEXICA SEARCH ENGINE v1.0        \n";
            // cout << "========================================\n";
            // cout << "Type 'help' to see available commands.\n\n";

            cout << "\n\n";

            string line {""};

            while (true) 
            {
                printProgramName(100);

                if (!std::getline(cin, line)) break;

                if (line.empty()) {
                    cout << '\n';
                    continue;
                }

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
                if (!dispatchCommmand(cmd, args)) break;

                cout << "\n\n";
            }
        }
};