#pragma once


#include <string>                                       // for string
#include "../../1_ds/1_vector.hpp"                      // for MyVector
using std::string;


// a tokenizer class to clean, lowercase, and split raw text into word tokens
class Tokenizer 
{
    public:
        Tokenizer() 
            {}

        // takes raw text and returns a custom MyVector of lowercased, punctuation-free word tokens
        inline static MyVector<string> tokenize(const string& text) 
        {
            MyVector<string> tokens;
            string currToken {""};

            for (auto ch : text) 
            {
                if (isalnum(ch)) 
                    currToken += static_cast<char>(tolower(ch));
                
                else {
                    if (currToken.size() >= 2)
                    {
                        tokens.push_back(currToken);
                        currToken = "";
                    }
                }
            }

            // push any remaining, valid token at the end of the text stream
            if (currToken.size() >= 2)
                tokens.push_back(currToken);

            return tokens;
        }
};