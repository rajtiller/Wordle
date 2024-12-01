#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <set>
#include <map>

size_t color_guess_1(char *&word, std::unordered_map<char, std::set<int>> &guess)
{ // should return a value between 0 and 242
    // yellow iff char is
    size_t ret = 0b0000000000;     // Unnecesariily complicated, but it should work. Green is 11 and yellow is 01
    for (size_t i = 0; i < 5; i++) // for char in word, give letter to 'deserving' letter in guess (if any exists)
    {
        auto it = guess.find(word[i]);
        if (it != guess.end())
        {
            // std::set<int> & res = it->second;
            if (it->second.find(i) != it->second.end())
            {                           // if the letter should be green in guess
                ret |= (0b11 << 2 * i); // could overwrite the letter, but that's fine
            }
            else if ((ret >> 2 * *it->second.begin() & 0b11) == 0b00)
            {                                             // letter is first of its kind in word
                ret |= (0b01 << 2 * *it->second.begin()); // make letter yellow
            }
            else
            {
                if (it->second.size() > 1)
                { // check if letter appears twice in guess
                    if ((ret >> 2 * *(++it->second.begin()) & 0b11) == 0b00)
                    {                                                 // letter is second of its kind
                        ret |= (0b01 << 2 * *(++it->second.begin())); // make letter yellow
                    }
                    else
                    { // letter is third of its kind
                        if (it->second.size() > 2)
                        { // check if letter appears thrice in guess
                            ret |= (0b01 << 2 * *(++ ++it->second.begin()));
                            // no letter appears four times in a word, so we can simply set the last letter to be yellow
                        }
                    }
                }
            }
        }
    }
    return ret;
}

size_t color_guess_2(char *&word, char *&guess)
{
    // std::cout << "Word: "<< word << "\n" << "Guess: " << guess<<std::endl;
    std::vector<int> to_ret(5, 0);
    std::vector<char> unused_chars;
    for (size_t i = 0; i < 5; i++)
    { // for char in word, assign green letters
        if (word[i] == guess[i])
        {
            to_ret[i] = 3;
        }
        else
        {
            unused_chars.push_back(word[i]);
        }
    }
    for (char c : unused_chars)
    {
        // std::cout<<c<<"\n";
        for (size_t i = 0; i < 5; i++)
        {
            if (!to_ret[i] && guess[i] == c)
            {
                to_ret[i] = 1;
                // std:: cout << "char, inx: " << guess[i] << i<<"\n";
                break;
            }
        }
    }
    return (to_ret[4] << 8) + (to_ret[3] << 6) + (to_ret[2] << 4) + (to_ret[1] << 2) + (to_ret[0] << 0);
}
int main()
{
    return 0;
}

// char* word = "ARRAY";
// std::unordered_map<char, std::set<int>> guess; // RADAR
// char * guess_2 = "RADAR";
// guess['R'] = {0,4};
// guess['A'] = {1,3};
// guess['D'] = {2};
// for (int i = 0; i < 1; i++) { // 1000000
//     std::cout<<color_guess_1(word,guess);
// }