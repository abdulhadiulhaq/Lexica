#pragma once

#include <string>                                       // for string
#include "1_fileLoader.hpp"                             // for FileLoader and Document
#include "2_tokenizer.hpp"                              // for Tokenizer
#include "3_documentStore.hpp"                          // for DocumentStore
#include "4_invertedIndex.hpp"                          // for InvertedIndex
#include "5_ranker.hpp"                                 // for Ranker and SearchResult
#include "6_editDistance.hpp"                           // for EditDistance
#include "../1_ds/1_vector.hpp"                         // for MyVector
using std::string;


// a struct representing a fully resolved search response containing document metadata and score
struct SearchResponse 
{
    int    docID;
    string fileName;
    double score;

    SearchResponse() 
        : docID(0)
        , fileName("")
        , score(0.0) 
        {}

    SearchResponse(int id, const string& name, double s) 
        : docID(id)
        , fileName(name)
        , score(s) 
        {}
};


// the central Lexica search engine orchestrator class (headless backend)
class Engine 
{
    private:
        DocumentStore documentStore;
        InvertedIndex invertedIndex;
        bool          isInitialized;

    public:
        Engine() 
            : isInitialized(false) 
            {}

        // initializes the engine by scanning a directory, tokenizing docs, and building the inverted index
        inline bool initialize(const string& dirPath) 
        {
            auto docs {FileLoader::loadDirectory(dirPath)};
            if (docs.empty()) 
            {
                return false;
            }

            for (const auto& doc : docs) 
            {
                documentStore.addDocument(doc);
                auto tokens {Tokenizer::tokenize(doc.content)};
                invertedIndex.addDocument(doc.docID, tokens);
            }

            isInitialized = true;
            return true;
        }

        // executes a search query, ranks documents via TF-IDF, and resolves metadata into SearchResponses
        inline MyVector<SearchResponse> search(const string& queryString, size_t topK = 5) const 
        {
            MyVector<SearchResponse> responses;

            if (!isInitialized) 
            {
                return responses;
            }

            auto queryTokens {Tokenizer::tokenize(queryString)};
            if (queryTokens.empty()) 
            {
                return responses;
            }

            // rank query tokens using the inverted index and ranker module
            auto rawResults {Ranker::rankQuery(queryTokens, invertedIndex, documentStore.size(), topK)};

            // resolve docIDs into filenames and scores
            for (const auto& res : rawResults) 
            {
                Document doc(0, "", "");
                if (documentStore.getDocument(res.docID, doc)) 
                {
                    responses.push_back(SearchResponse(res.docID, doc.fileName, res.score));
                }
            }

            return responses;
        }

        // returns the total number of indexed documents
        inline size_t getDocumentCount() const 
        {
            return documentStore.size();
        }
};