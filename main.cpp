#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <set>
#include <map>

const char **GUESSES;
const char **ANSWERS;
std::unordered_map<int, double> word_value_map;



typedef std::vector<char *> SetType; // make sure to initialize vectors with size 5 each, idk if char** is better
                                     // sort words ahead of time also
size_t set_hasher(SetType &s)
{
}

size_t color_guess(char *&word, std::unordered_map<char, std::set<int>> &guess)
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
            else if (ret >> 2 * *it->second.begin() & 0b11 == 0b00)
            {                                             // letter is first of its kind in word
                ret |= (0b01 << 2 * *it->second.begin()); // make letter yellow
            }
            else
            {
                if (it->second.size() > 1)
                { // check if letter appears twice in guess
                    if (ret >> 2 * *(++it->second.begin()) & 0b11 == 0b00)
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
}
std::vector<SetType> create_possibilities(SetType &curr_words, std::unordered_map<char, std::set<int>> &guess)
{                                        //  could be beneficial to create and destory these sets immediately, saving memory
    std::map<int, SetType> sorted_words; // map not unordered_map bc I want to be able to iterate through quickly
                                         // even though vectors do perfect hash, it will leave a lot of empty spaces
    for (char *&word : curr_words)
    {
        sorted_words[color_guess(word, guess)].push_back(word);
    }
    std::map<int, SetType> ordered_words;
    for (auto & it:sorted_words) {
        ordered_words.insert({it.second.size(),it.second});
    }

}

double calculate_word_value(SetType &curr_words, char *&curr_word)
{
    size_t curr_hash = set_hasher(curr_words);
    double ret = 10;
    auto it = word_value_map.find(curr_hash);
    if (it != word_value_map.end())
    {
        return it->second;
    }
}

char *find_best_word(SetType &curr_words)
{

    char *best_word = curr_words[0];
    double best_word_value = 10;
    for (auto curr_word = curr_words.begin(); curr_word != curr_words.end(); curr_word++)
    {
        double curr_word_value = calculate_word_value(curr_words, *curr_word);
        if (curr_word_value > best_word_value)
        {
            best_word = *curr_word;
            best_word_value = curr_word_value;
        }
    }
}

int main()
{
}
