#pragma once


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../2_core/engine.hpp"

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;


// a header-only web graphical user interface (GUI) class hosting an embedded HTTP server for Lexica
class GUI 
{
    private:
        Engine engine;
        int    port;

        // helper to parse URL query parameters (e.g., ?q=neural&k=5)
        inline string getQueryParam(const string& url, const string& param) const 
        {
            auto key {param + "="};
            auto startPos {url.find(key)};
            if (startPos == string::npos) 
                return "";

            startPos += key.length();
            auto endPos {url.find('&', startPos)};
            if (endPos == string::npos) 
                return url.substr(startPos);

            return url.substr(startPos, endPos - startPos);
        }

        // helper to generate HTTP JSON/HTML responses
        inline string handleRequest(const string& requestPath) 
        {
            // 1. Serve Static Frontend Files
            if (requestPath == "/" || requestPath == "/index.html") 
            {
                ifstream file("3_frontEnd/2_webGUI/index.html");
                if (!file) return "HTTP/1.1 404 Not Found\r\n\r\n404 File Not Found";
                stringstream buffer;
                buffer << file.rdbuf();
                return "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n" + buffer.str();
            }
            else if (requestPath == "/style.css") 
            {
                ifstream file("3_frontEnd/2_webGUI/style.css");
                if (!file) return "HTTP/1.1 404 Not Found\r\n\r\n404 File Not Found";
                stringstream buffer;
                buffer << file.rdbuf();
                return "HTTP/1.1 200 OK\r\nContent-Type: text/css; charset=utf-8\r\n\r\n" + buffer.str();
            }
            else if (requestPath == "/script.js") 
            {
                ifstream file("3_frontEnd/2_webGUI/script.js");
                if (!file) return "HTTP/1.1 404 Not Found\r\n\r\n404 File Not Found";
                stringstream buffer;
                buffer << file.rdbuf();
                return "HTTP/1.1 200 OK\r\nContent-Type: application/javascript; charset=utf-8\r\n\r\n" + buffer.str();
            }

            // 2. REST API: Search endpoint (/api/search?q=query&k=5)
            if (requestPath.rfind("/api/search", 0) == 0) 
            {
                string rawQuery {getQueryParam(requestPath, "q")};
                string rawK {getQueryParam(requestPath, "k")};
                size_t topK {rawK.empty() ? 5 : std::stoul(rawK)};

                // URL decoding space '+'
                for (auto& ch : rawQuery) 
                    if (ch == '+') ch = ' ';

                bool cacheHit {false};
                auto results {engine.search(rawQuery, topK, cacheHit)};

                stringstream json;
                json << "{\"cacheHit\": " << (cacheHit ? "true" : "false") << ", \"results\": [";
                for (size_t i {0}; i < results.size(); ++i) 
                {
                    const auto& res {results[i]};
                    json << "{\"docID\": " << res.docID << ", \"fileName\": \"" << res.fileName << "\", \"score\": " << res.score << "}";
                    if (i + 1 < results.size()) json << ",";
                }
                json << "]}";

                return "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n\r\n" + json.str();
            }

            // 3. REST API: Autocomplete endpoint (/api/suggest?q=prefix)
            if (requestPath.rfind("/api/suggest", 0) == 0) 
            {
                string prefix {getQueryParam(requestPath, "q")};
                auto suggestions {engine.suggest(prefix, 5)};

                stringstream json;
                json << "{\"suggestions\": [";
                for (size_t i {0}; i < suggestions.size(); ++i) 
                {
                    const auto& pair {suggestions[i]};
                    json << "{\"term\": \"" << pair.first << "\", \"freq\": " << pair.second << "}";
                    if (i + 1 < suggestions.size()) json << ",";
                }
                json << "]}";

                return "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n\r\n" + json.str();
            }

            // 4. REST API: Fuzzy Match endpoint (/api/fuzzy?q=term)
            if (requestPath.rfind("/api/fuzzy", 0) == 0) 
            {
                string term {getQueryParam(requestPath, "q")};
                string bestMatch {""};
                int distance {0};

                bool found {engine.fuzzyMatch(term, bestMatch, distance, 3)};

                stringstream json;
                json << "{\"found\": " << (found ? "true" : "false") << ", \"bestMatch\": \"" << bestMatch << "\", \"distance\": " << distance << "}";

                return "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n\r\n" + json.str();
            }

            // 5. REST API: Stats endpoint (/api/stats)
            if (requestPath == "/api/stats") 
            {
                stringstream json;
                json << "{\"documents\": " << engine.getDocumentCount() << ", \"uniqueTerms\": " << engine.getUniqueTermCount() << "}";
                return "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\n\r\n" + json.str();
            }

            return "HTTP/1.1 404 Not Found\r\n\r\nEndpoint Not Found";
        }

    public:
        inline GUI(int serverPort = 8080) 
            : port(serverPort) 
        {
            cout << "[*] Auto-indexing 'data/sample' corpus for Web GUI...\n";
            auto stats {engine.initialize("data/sample")};
            cout << "[*] Indexed " << stats.fileCount << " files (" << stats.totalTokens << " tokens) successfully.\n";
        }

        // starts the embedded HTTP server loop
        inline void run() 
        {
            int serverFD {socket(AF_INET, SOCK_STREAM, 0)};
            if (serverFD == 0) 
            {
                std::cerr << "[!] Error: Socket creation failed.\n";
                return;
            }

            int opt {1};
            setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);

            if (bind(serverFD, (struct sockaddr*)&address, sizeof(address)) < 0) 
            {
                std::cerr << "[!] Error: Bind failed on port " << port << ".\n";
                return;
            }

            if (listen(serverFD, 10) < 0) 
            {
                std::cerr << "[!] Error: Listen failed.\n";
                return;
            }

            cout << "\n========================================\n";
            cout << "     LEXICA WEB GUI RUNNING AT          \n";
            cout << "     http://localhost:" << port << "          \n";
            cout << "========================================\n\n";

            while (true) 
            {
                sockaddr_in clientAddr;
                socklen_t clientLen {sizeof(clientAddr)};
                int newSocket {accept(serverFD, (struct sockaddr*)&clientAddr, &clientLen)};
                if (newSocket < 0) continue;

                char buffer[3048] = {0};
                read(newSocket, buffer, 3048);

                string reqStr(buffer);
                string path {"/"};
                auto firstSpace {reqStr.find(' ')};
                if (firstSpace != string::npos) 
                {
                    auto secondSpace {reqStr.find(' ', firstSpace + 1)};
                    if (secondSpace != string::npos) 
                    {
                        path = reqStr.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                    }
                }

                string response {handleRequest(path)};
                write(newSocket, response.c_str(), response.length());
                close(newSocket);
            }
        }
};