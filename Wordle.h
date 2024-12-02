#ifndef WORDLE_H
#define WORDLE_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <numeric> // for std::accumulate

typedef std::vector<char *> SetType;                         // make sure to initialize vectors with size 5 each, idk if char** is better                                                       // sort words ahead of time also
typedef std::unordered_map<char, std::vector<int>> R_STRING; // find a way to store this so I don't have to recalculate every time

SetType ANSWERS;
std::vector<std::pair<R_STRING, char *>> GUESSES;

static constexpr size_t MOST_VALID_GUESSES = 1;

static constexpr const char *ANSWERS_FILE = "AllValidAnswers.txt"; // ../ because address is relative to build folder
static constexpr const char *GUESSES_FILE = "AllValidGuesses.txt";

class Wordle
{
public:
    class Int
    {
    public:
        Int() : num(0) {
                };
        void operator++()
        {
            num++;
        }
        int num;
    };
    class Reverse
    {
    public:
        bool operator()(const int &int1, const int &int2)
        {
            return int1 > int2;
        }
    };
    class PQComp
    {
    public:
        bool operator()(const std::pair<double, R_STRING> &a, const std::pair<double, R_STRING> &b) const
        {
            return a.first < b.first;
        }
    };
    class SetHasher
    { // change so strings are indexed at some point (ie set type of a vector of size_t/int)
    };
    // Public Methods
    Wordle() {};
    void Wordle::print_best_word(std::pair<char *, double> &input, std::ostream &os);
    char *&to_upper_case(std::string &in);
    std::pair<R_STRING, char *> Wordle::to_R_STRING(std::string &in);
    size_t color_guess(char *&word, std::pair<R_STRING, char *> &guess);
    double naive_information_estimate(SetType &curr_words, std::pair<R_STRING, char *> &guess);
    void create_subgroups(SetType &curr_words, std::pair<R_STRING, char *> &guess, std::unordered_map<size_t, SetType> &possibilities);
    double exact_information_value(SetType &curr_words, std::pair<R_STRING, char *> &guess);
    double calculate_set_value(SetType &curr_word);
    std::pair<char *, double> find_best_word(SetType &curr_words);
    void prep_guesses_and_answers();

    std::unordered_map<SetType, std::pair<char *, double>, SetHasher> word_value_map;

private:
};

#endif // WORDLE_H
