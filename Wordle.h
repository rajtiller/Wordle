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

typedef std::vector<char *> SetType;                      // make sure to initialize vectors with size 5 each, idk if char** is better                                                       // sort words ahead of time also
typedef std::unordered_map<char, std::set<int>> R_STRING; // find a way to store this so I don't have to recalculate every time

SetType ANSWERS;
std::vector<R_STRING> GUESSES;

static constexpr size_t MOST_VALID_GUESSES = 100;

static constexpr const char *ANSWERS_FILE = "../AllValidAnswers.txt"; // ../ because address is relative to build folder
static constexpr const char *GUESSES_FILE = "../AllValidGuesses.txt";

std::map<size_t, std::pair<R_STRING, double>> word_value_map;

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
    // Public Methods
    Wordle() {};
    size_t set_hasher(const SetType &curr_words);
    char *to_SetType(std::string &in);
    void print_best_word(std::pair<R_STRING, double> &input, std::ostream &os);
    R_STRING to_R_STRING(std::string &in);
    size_t color_guess(char *&word, R_STRING &guess);
    double naive_information_estimate(SetType &curr_words, R_STRING guess);
    void create_subgroups(SetType &curr_words, R_STRING &guess, std::map<size_t, SetType> &possibilities);
    double exact_information_value(SetType &curr_words, R_STRING guess);
    double calculate_set_value(SetType &curr_words, std::optional<size_t> curr_hash_in = std::nullopt);
    std::pair<R_STRING, double> find_best_word(SetType &curr_words);
    void prep_guesses_and_answers();

private:
};

#endif // WORDLE_H
