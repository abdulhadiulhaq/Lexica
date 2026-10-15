#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
using namespace std;



// Struct representing a parsed word tied to its source document name
struct Word {
    string text;
    string docName;
};

class Parser {
    private:
        unordered_set<string> stopWords;

        bool loadStopWords(const filesystem::path& stopWordsFilePath){
            if (!filesystem::exists(stopWordsFilePath)){
                cerr << "[!] Could not open stop words file: " << stopWordsFilePath << "\n";
                return false;
            }

            ifstream file(stopWordsFilePath);
            if (file){                                      // redundant condition
                string line;
                
                while (getline(file, line))
                    if (!line.empty()) 
                        stopWords.insert(line);
    
                file.close();
                return true;
            }

            return false;                                   // will never reach here ideally
        }

        // Helper method to clean, tokenize, and map words
        vector<string> cleanAndTokenize(const string& rawText) {
            string         cleanedText = "";
            vector<string> tokens;

            // lowercase all words & replace punctuation/numbers with spaces
                for (auto& c : rawText) {
                    if (isalpha(c)){
                        cleanedText += tolower(c);
                    } 
                    else{
                        cleanedText += ' ';
                    }
                }

            // split words by spaces and filter out stop words & remove those below length 2
                stringstream ssCleanedText(cleanedText);
                string word;
                while (ssCleanedText >> word) {
                    if ((word.length() > 1) && (stopWords.find(word) == stopWords.end())) {
                        tokens.push_back(word);
                    }
                }

            return tokens;
        }

        // Helper method to parse a single file and return tokens as Word structs
        vector<Word> parseSingleFile(const filesystem::path& filePath) {
            vector<Word> fileWords;
            ifstream file(filePath);
            if (!file.is_open()) {
                cerr << "[Error] Could not open file: " << filePath.string() << "\n";
                return fileWords;
            }

            stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            vector<string> tokens = cleanAndTokenize(buffer.str());
            string docName = filePath.filename().string();

            for (const string& token : tokens) {
                fileWords.push_back({token, docName});
            }

            return fileWords;
        }

    public:
        // Constructor: Loads stop words from the comma-separated CSV file
        Parser(const filesystem::path& stopWordsFilePath){

        }

        // Main method: Takes an input path (file or directory) and returns all parsed Word structs
        vector<Word> parse(const string& inputPath) {
            vector<Word> allParsedWords;
            filesystem::path path(inputPath);

            if (!filesystem::exists(path)) {
                cerr << "[Error] Path does not exist: " << inputPath << "\n";
                return allParsedWords;
            }

            // Case 1: Input is a directory
            if (filesystem::is_directory(path)) {
                cout << "[Info] Scanning directory: " << inputPath << "\n";
                for (const auto& entry : filesystem::directory_iterator(path)) {
                    if (entry.is_regular_file()) {
                        if (entry.path().extension() == ".txt") {
                            vector<Word> fileWords = parseSingleFile(entry.path());
                            allParsedWords.insert(allParsedWords.end(), fileWords.begin(), fileWords.end());
                        }
                    }
                }
            }
            // Case 2: Input is a single file
            else if (filesystem::is_regular_file(path)) {
                cout << "[Info] Parsing single file: " << inputPath << "\n";
                allParsedWords = parseSingleFile(path);
            }

            return allParsedWords;
        }
};

// --- Example Test Driver (`main`) ---
int main() {
    // Initialize Parser with stop words
    Parser parser("stop_words.csv");

    // Can pass either a single file ("sample.txt") or a folder ("./documents/")
    vector<Word> parsedData = parser.parse("./documents/");

    cout << "\nTotal parsed tokens extracted: " << parsedData.size() << "\n";
    for (size_t i = 0; i < min(size_t(5), parsedData.size()); ++i) {
        cout << "-> Word: \"" << parsedData[i].text << "\" | Doc Name: \"" << parsedData[i].docName << "\"\n";
    }

    return 0;
}