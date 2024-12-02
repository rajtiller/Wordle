#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <set>
#include <map>

std::bitset<10> color_guess_1(char* &word, std::pair<std::unordered_map<char, std::vector<int>>, char*> &guess)
{
    std::bitset<10> ret; // Stores the result
    for (size_t word_inx = 0; word_inx < 5; word_inx++)
    {
        auto it = guess.first.find(word[word_inx]);
        if (it != guess.first.end())
        {
            if (word[word_inx] == guess.second[word_inx])
            {
                // Set ret for exact match
                ret |= std::bitset<10>(3 << 2 * word_inx);
            }
            else
            {
                for (int &inx : it->second)
                {
                    if ((ret >> (2 * inx) & std::bitset<10>(0b11)) == std::bitset<10>(0))
                    {
                        // Set ret for partial match
                        ret |= std::bitset<10>(1 << 2 * inx);
                        break;
                    }
                }
            }
        }
    }
    return ret; // Convert std::bitset to size_t before returning
}


size_t color_guess_2(char * &word, std::pair<std::unordered_map<char, std::vector<int>>, char *> &guess)
{
    // std::cout << "Word: "<< word << "\n" << "Guess: " << guess<<std::endl;
    uint16_t ret = 0; // consider using vector and see what happens
    for (size_t word_inx = 0; word_inx < 5; word_inx++)
    {
        auto it = guess.first.find(word[word_inx]);
        if (it != guess.first.end())
        {
            if (word[word_inx] == guess.second[word_inx])
            {
                ret |= (3 << 2*word_inx);
            } else {
                for (int & inx:it->second) {
                    if ((ret >> 2*inx & 0b11) == 0b00) {
                        ret |= (1 << 2*inx);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

size_t color_guess_3(std::string &word, std::string &guess_2, std::unordered_map<char, std::set<int>> &guess)
{
    size_t ret = 0;
    for (size_t i = 0; i < 5; i++)
    {
        if (guess.find(word[i]) != guess.end())
        {
            if (word[i] == guess_2[i])
            {
                ret += 2;
            }
            ret += 1;
        }
    }
    return ret;
}

size_t color_guess_4(char *&word, std::unordered_map<char, std::set<int>> &guess)
{
    size_t ret = 0;
    for (size_t i = 0; i < 5; i++)
    {
        auto it = guess.find(word[i]);
        if (it != guess.end())
        {
            ret += 1;
            if (it->second.find(i) != it->second.end())
            {
                ret += 2;
            }
        }
    }
    return ret;
}

int main()
{
   char* word = "ARRAY";
    std::pair<std::unordered_map<char, std::vector<int>>,char*> guess; // RADAR
    std::unordered_map<char, std::set<int>> guess_2;
    guess.second = "RADAR";
    guess.first['R'] = {0, 4};
    guess.first['A'] = {1, 3};
    guess.first['D'] = {2};
    guess_2['R'] = {0, 4};
    guess_2['A'] = {1, 3};
    guess_2['D'] = {2};
    for (int i = 0; i < 14855 * 2309; i++) // 14855 * 2309
    {                                      // 1000000
        color_guess_1(word, guess); // Should be 453 aka YGBYY
    }
    return 0;
}
