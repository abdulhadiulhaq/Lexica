#include "gui.hpp"
#include "include/httplib.h"
#include <iostream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
using std::cout, std::cerr, std::string;


GUI::GUI(const string& staticDir, const string& dataDir, int serverPort)
    : webRoot(staticDir)
    , defaultDataDir(dataDir)
    , port(serverPort) 
{}


string GUI::escapeJson(const string& input) 
{
    std::ostringstream ss;
    for (char c : input) 
    {
        switch (c) 
        {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:   ss << c;      break;
        }
    }
    return ss.str();
}


void GUI::run() 
{
    // 1. Resolve paths whether executable is run from project root or build/
    string resolvedWebRoot = fs::exists(webRoot) ? webRoot : "src/3_frontEnd/2_webGUI/web";
    string resolvedDataDir = fs::exists(defaultDataDir) ? defaultDataDir : "data/sample";

    // 2. Auto-index the corpus before starting the server
    cout << "Indexing corpus from: " << resolvedDataDir << "...\n";
    auto stats = engine.initialize(resolvedDataDir);
    if (stats.fileCount == 0) 
        cerr << "[!] Warning: No documents indexed from '" << resolvedDataDir << "'.\n";
    else 
        cout << "Indexed " << stats.fileCount << " files (" << stats.uniqueTerms << " unique terms) in " 
             << stats.elapsedSeconds << " s\n";

    // 3. Configure cpp-httplib server
    httplib::Server svr;

    if (!svr.set_mount_point("/", resolvedWebRoot)) 
    {
        cerr << "[!] Error: Could not mount web GUI directory at: " << resolvedWebRoot << "\n";
        return;
    }

    // GET /api/stats
    svr.Get("/api/stats", [this](const httplib::Request&, httplib::Response& res) {
        std::ostringstream json;
        json << "{"
             << "\"documents\":"   << engine.getDocumentCount() << ","
             << "\"uniqueTerms\":" << engine.getUniqueTermCount()
             << "}";
        res.set_content(json.str(), "application/json");
    });

    // GET /api/suggest?q=<prefix>
    svr.Get("/api/suggest", [this](const httplib::Request& req, httplib::Response& res) {
        string query = req.has_param("q") ? req.get_param_value("q") : "";
        auto suggestions = engine.suggest(query, 5);

        std::ostringstream json;
        json << "{\"suggestions\":[";
        for (size_t i = 0; i < suggestions.size(); ++i) 
        {
            if (i > 0) json << ",";
            json << "{"
                 << "\"term\":\"" << escapeJson(suggestions[i].first) << "\","
                 << "\"freq\":"   << suggestions[i].second
                 << "}";
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    // GET /api/search?q=<query>&k=<topK>
    svr.Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
        string query = req.has_param("q") ? req.get_param_value("q") : "";
        size_t topK  = 5;

        if (req.has_param("k")) 
        {
            try {
                topK = std::stoul(req.get_param_value("k"));
            } catch (...) {
                topK = 5;
            }
        }

        bool cacheHit = false;
        auto results  = engine.search(query, topK, cacheHit);

        std::ostringstream json;
        json << "{"
             << "\"cacheHit\":" << (cacheHit ? "true" : "false") << ","
             << "\"results\":[";
        for (size_t i = 0; i < results.size(); ++i) 
        {
            if (i > 0) json << ",";
            json << "{"
                 << "\"fileName\":\"" << escapeJson(results[i].fileName) << "\","
                 << "\"score\":"      << results[i].score
                 << "}";
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    // GET /api/fuzzy?q=<word>
    svr.Get("/api/fuzzy", [this](const httplib::Request& req, httplib::Response& res) {
        string query     = req.has_param("q") ? req.get_param_value("q") : "";
        string bestMatch = "";
        int    distance  = 0;

        bool found = engine.fuzzyMatch(query, bestMatch, distance, 3);

        std::ostringstream json;
        json << "{"
             << "\"found\":"       << (found ? "true" : "false") << ","
             << "\"bestMatch\":\"" << escapeJson(bestMatch) << "\","
             << "\"distance\":"    << distance
             << "}";
        res.set_content(json.str(), "application/json");
    });

    cout << "Lexica Web GUI running at http://localhost:" << port << "\n";
    svr.listen("0.0.0.0", port);
}