#pragma once

#include <string>
#include <algorithm>

typedef long long size_t;

class EditDistance 
{
    public:
        static inline int levenshtein(const std::string& a, const std::string& b) 
        {
            size_t len_a = a.size();
            size_t len_b = b.size();

            if (len_a == 0) return static_cast<int>(len_b);
            if (len_b == 0) return static_cast<int>(len_a);

            int* prev = new int[len_b + 1];
            int* curr = new int[len_b + 1];

            for (size_t j = 0; j <= len_b; ++j) 
            {
                prev[j] = static_cast<int>(j);
            }

            for (size_t i = 1; i <= len_a; ++i) 
            {
                curr[0] = static_cast<int>(i);
                for (size_t j = 1; j <= len_b; ++j) 
                {
                    int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                    int deletion = prev[j] + 1;
                    int insertion = curr[j - 1] + 1;
                    int substitution = prev[j - 1] + cost;

                    int min_val = deletion;
                    if (insertion < min_val) min_val = insertion;
                    if (substitution < min_val) min_val = substitution;

                    curr[j] = min_val;
                }
                for (size_t j = 0; j <= len_b; ++j) 
                {
                    prev[j] = curr[j];
                }
            }

            int result = curr[len_b];
            delete[] prev;
            delete[] curr;
            return result;
        }
};