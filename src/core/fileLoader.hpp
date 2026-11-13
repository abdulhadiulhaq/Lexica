#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "my_vector.hpp"

typedef long long size_t;

struct Document 
{
    int         doc_id;
    std::string filename;
    std::string content;
};

class FileLoader 
{
    public:
        static MyVector<Document> loadDirectory(const std::string& dir_path) 
        {
            MyVector<Document> documents;
            int current_id = 1;

            for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
            {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") 
                {
                    std::ifstream file(entry.path());
                    if (file.is_open()) 
                    {
                        std::stringstream buffer;
                        buffer << file.rdbuf();

                        Document doc;
                        doc.doc_id   = current_id++;
                        doc.filename = entry.path().filename().string();
                        doc.content  = buffer.str();

                        documents.push_back(doc);
                    }
                }
            }

            return documents;
        }
};