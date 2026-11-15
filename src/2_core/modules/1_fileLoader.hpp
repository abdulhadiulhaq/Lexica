#pragma once


#include <string>                                           // for string
#include <fstream>                                          // for ifstream
#include <sstream>                                          // for stringstream
#include <filesystem>                                       // for filesystem
#include "../../1_ds/1_vector.hpp"                          // for MyVector
using std::string, std::ifstream, std::stringstream;
namespace fs = std::filesystem;


// a struct representing a loaded document from disk
struct Document 
{
    int    docID;
    string fileName;
    string content;

    Document() 
        : docID(0)
        , fileName("")
        , content("") 
        {}

    Document(const int id, const string& name, const string& text) 
        : docID(id)
        , fileName(name)
        , content(text) 
        {}
};


// a file loader class to scan and parse text documents from a directory
class FileLoader 
{
    public:
        FileLoader() 
            {}

        // scans a directory and loads all .txt files into a custom MyVector of Documents
        inline static MyVector<Document> loadDirectory(const string& dirPath) 
        {
            MyVector<Document> documents;                                                   // making a vector of Document nodes
            auto currentID {1};

            for (const auto& entry : fs::directory_iterator(dirPath))                       // iterates through every object in the directory
            {
                if (entry.is_regular_file() && (entry.path().extension() == ".txt"))        // checking if the entry is a text file
                {
                    ifstream file(entry.path());                                            // opening the file in read-mode
                    
                    if (file) {
                        stringstream buffer;
                        buffer << file.rdbuf();                                             // reading the whole contents of the file at once

                        Document doc(currentID++, entry.path().filename().string(), buffer.str());
                        documents.push_back(doc);
                    }
                }
            }

            return documents;
        }
};