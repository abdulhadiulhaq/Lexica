#pragma once


#include <string>                                                                      // for string
#include <fstream>                                                                     // for ifstream
#include <sstream>                                                                     // for stringstream
#include <iostream>                                                                    // for cout, cerr, endl
#include "include/httplib.h"                                                           // for httplib::Server
#include "../../2_core/engine.hpp"                                                     // for Engine
using std::string, std::cout, std::cerr, std::endl, std::ifstream, std::stringstream;


// a header-only web graphical user interface (GUI) class hosting an embedded HTTP server via cpphttplib
class GUI 
{
    private:
        Engine engine;
        int    port;

        // helper to read static web assets from the web/ directory
        inline string readFile(const string& filepath) const 
        {
            ifstream file(filepath);
            if (!file) 
                return "";

            stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

    public:
        inline GUI(int serverPort = 8080) 
            : port(serverPort) 
        {
            cout << "[*] Auto-indexing 'data/sample' corpus for Web GUI...\n";
            auto stats {engine.initialize("data/sample")};
            cout << "[*] Indexed " << stats.fileCount << " files (" << stats.totalTokens << " tokens) successfully.\n";
        }

        // starts the embedded HTTP server loop using cpphttplib
        inline void run() 
        {
            httplib::Server svr;

            // 1. Serve Static Frontend Files (from the web/ folder)
            svr.Get("/", [this](const httplib::Request&, httplib::Response& res) 
            {
                string content {readFile("3_frontEnd/2_webGUI/web/index.html")};
                if (content.empty()) 
                {
                    res.status = 404;
                    res.set_content("404 File Not Found", "text/plain");
                } 
                else 
                {
                    res.set_content(content, "text/html; charset=utf-8");
                }
            });

            svr.Get("/index.html", [this](const httplib::Request&, httplib::Response& res) 
            {
                string content {readFile("3_frontEnd/2_webGUI/web/index.html")};
                if (content.empty()) 
                {
                    res.status = 404;
                    res.set_content("404 File Not Found", "text/plain");
                } 
                else 
                {
                    res.set_content(content, "text/html; charset=utf-8");
                }
            });

            svr.Get("/style.css", [this](const httplib::Request&, httplib::Response& res) 
            {
                string content {readFile("3_frontEnd/2_webGUI/web/style.css")};
                if (content.empty()) 
                {
                    res.status = 404;
                    res.set_content("404 File Not Found", "text/plain");
                } 
                else 
                {
                    res.set_content(content, "text/css; charset=utf-8");
                }
            });

            svr.Get("/script.js", [this](const httplib::Request&, httplib::Response& res) 
            {
                string content {readFile("3_frontEnd/2_webGUI/web/script.js")};
                if (content.empty()) 
                {
                    res.status = 404;
                    res.set_content("404 File Not Found", "text/plain");
                } 
                else 
                {
                    res.set_content(content, "application/javascript; charset=utf-8");
                }
            });

            // 2. REST API: Search endpoint (/api/search?q=query&k=5)
            svr.Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) 
            {
                string rawQuery {req.has_param("q") ? req.get_param_value("q") : ""};
                string rawK {req.has_param("k") ? req.get_param_value("k") : ""};
                size_t topK {rawK.empty() ? 5 : std::stoul(rawK)};

                bool cacheHit {false};
                auto results {engine.search(rawQuery, topK, cacheHit)};

                stringstream json;
                json << "{\"cacheHit\": " << (cacheHit ? "true" : "false") << ", \"results\": [";
                for (size_t i {0}; i < results.size(); ++i) 
                {
                    const auto& resItem {results[i]};
                    json << "{\"docID\": " << resItem.docID << ", \"fileName\": \"" << resItem.fileName << "\", \"score\": " << resItem.score << "}";
                    if (i + 1 < results.size()) 
                        json << ",";
                }
                json << "]}";

                res.set_content(json.str(), "application/json; charset=utf-8");
            });

            // 3. REST API: Autocomplete endpoint (/api/suggest?q=prefix)
            svr.Get("/api/suggest", [this](const httplib::Request& req, httplib::Response& res) 
            {
                string prefix {req.has_param("q") ? req.get_param_value("q") : ""};
                auto suggestions {engine.suggest(prefix, 5)};

                stringstream json;
                json << "{\"suggestions\": [";
                for (size_t i {0}; i < suggestions.size(); ++i) 
                {
                    const auto& pair {suggestions[i]};
                    json << "{\"term\": \"" << pair.first << "\", \"freq\": " << pair.second << "}";
                    if (i + 1 < suggestions.size()) 
                        json << ",";
                }
                json << "]}";

                res.set_content(json.str(), "application/json; charset=utf-8");
            });

            // 4. REST API: Fuzzy Match endpoint (/api/fuzzy?q=term)
            svr.Get("/api/fuzzy", [this](const httplib::Request& req, httplib::Response& res) 
            {
                string term {req.has_param("q") ? req.get_param_value("q") : ""};
                string bestMatch {""};
                int distance {0};

                bool found {engine.fuzzyMatch(term, bestMatch, distance, 3)};

                stringstream json;
                json << "{\"found\": " << (found ? "true" : "false") << ", \"bestMatch\": \"" << bestMatch << "\", \"distance\": " << distance << "}";

                res.set_content(json.str(), "application/json; charset=utf-8");
            });

            // 5. REST API: Stats endpoint (/api/stats)
            svr.Get("/api/stats", [this](const httplib::Request&, httplib::Response& res) 
            {
                stringstream json;
                json << "{\"documents\": " << engine.getDocumentCount() << ", \"uniqueTerms\": " << engine.getUniqueTermCount() << "}";
                res.set_content(json.str(), "application/json; charset=utf-8");
            });

            cout << "\n========================================\n";
            cout << "     LEXICA WEB GUI RUNNING AT          \n";
            cout << "     http://localhost:" << port << "          \n";
            cout << "========================================\n\n";

            svr.listen("0.0.0.0", port);
        }
};