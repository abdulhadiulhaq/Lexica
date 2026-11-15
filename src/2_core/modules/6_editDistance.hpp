#pragma once


#include <string>                  // for string
#include <algorithm>               // for min
using std::min, std::string;

/*
Levenshtein Distance
    => it measures the minimum number of single-character operations needed to transform one string (a) into another string (b). 
    
    The allowed operations are:

        1. Insertion (adding a character)
        2. Deletion (removing a character)
        3. Substitution (swapping one character for another)
*/

// a utility class for computing edit distance (Levenshtein distance) between query and vocabulary terms using DP
class EditDistance 
{
    public:
        EditDistance() 
            {}

        // calculates the minimum number of single-character edits to transform string a into string b
        inline static int levenshtein(const string& a, const string& b) 
        {
            auto lenA {a.size()};
            auto lenB {b.size()};

            // edge case handling
                if (!lenA) return lenB;
                if (!lenB) return lenA;

            // allocating the rolling rows
                auto prev {new int[lenB + 1]};
                auto curr {new int[lenB + 1]};

            // initializing the base-row
                for (auto j {0}; j <= lenB; ++j) 
                    prev[j] = j;

            // the main DP loops
                for (auto i {1}; i <= lenA; ++i) 
                {
                    curr[0] = i;

                    for (auto j {1}; j <= lenB; ++j) 
                    {
                        auto cost { (a[i - 1] == b[j - 1])? 0 : 1 };

                        // costs of the 3 possible choices
                            auto deletion     { prev[j]     + 1    };
                            auto insertion    { curr[j - 1] + 1    };
                            auto substitution { prev[j - 1] + cost };

                        curr[j] = min({deletion, insertion, substitution});
                    }

                    // the row-shifting loop
                        for (auto j {0}; j <= lenB; ++j)                                // copying all the values from the current row (curr) into the previous row (prev) after the inner loop completes
                            prev[j] = curr[j];
                }

            auto result {curr[lenB]};

            delete[] prev;
            delete[] curr;

            return result;
        }
};