#pragma once


#include <string>                  // for string
using std::string;


// a utility class for computing edit distance (Levenshtein distance) between query and vocabulary terms
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

            if (!lenA) return lenB;
            if (!lenB) return lenA;

            auto prev {new int[lenB + 1]};
            auto curr {new int[lenB + 1]};

            for (auto j {0}; j <= lenB; ++j) 
                prev[j] = static_cast<int>(j);

            for (auto i {1}; i <= lenA; ++i) 
            {
                curr[0] = static_cast<int>(i);

                for (auto j {1}; j <= lenB; ++j) 
                {
                    auto cost         { (a[i - 1] == b[j - 1]) ? 0 : 1 };
                    auto deletion     { prev[j] + 1 };
                    auto insertion    { curr[j - 1] + 1 };
                    auto substitution { prev[j - 1] + cost };

                    auto minVal { deletion };
                    if (insertion    < minVal) minVal = insertion;
                    if (substitution < minVal) minVal = substitution;

                    curr[j] = minVal;
                }

                for (auto j {0}; j <= lenB; ++j) 
                    prev[j] = curr[j];
            }

            auto result {curr[lenB]};

            delete[] prev;
            delete[] curr;

            return result;
        }
};