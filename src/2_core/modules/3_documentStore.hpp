#pragma once


#include <string>                                       // for string
#include "../../1_ds/1_vector.hpp"                      // for MyVector
#include "1_fileLoader.hpp"                             // for Document struct
using std::string;


// a document store class to manage, index, and retrieve documents by their docID
class DocumentStore 
{
    private:
        MyVector<Document> documents;                   // custom vector holding all documents sequentially

    public:
        DocumentStore() 
            {}

        // adds a single document to the store
        inline void addDocument(const Document& doc) {
            documents.push_back(doc);
        }

        // loads a batch of documents (e.g., directly from FileLoader) into the store
        inline void loadDocuments(const MyVector<Document>& docs) 
        {
            for (const auto& doc : docs) 
                documents.push_back(doc);
        }

        // retrieves a document by its docID (1-indexed), returning true if found
        inline bool getDocument(int docID, Document& outDoc) const 
        {
            if ((docID <= 0) || (docID > documents.size())) 
                return false;

            outDoc = documents[docID - 1];              // mapping 1-based docID to 0-based vector index
            return true;
        }

        // returns the total number of documents stored
        inline size_t size() const {
            return documents.size();
        }

        // returns true if the store has no documents
        inline bool empty() const {
            return documents.empty();
        }

        // clears all documents from the store
        inline void clear() {
            documents.clear();
        }
};