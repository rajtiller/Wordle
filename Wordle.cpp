#include "Wordle.h"

size_t Wordle::set_hasher(const SetType &curr_words)
{
    std::hash<std::string> string_hasher; // Use a string hasher for consistency.
    size_t hash_value = 0;

    for (const char *str : curr_words)
    {
        if (str)
        {
            // Hash each string and combine using XOR and a prime multiplier
            hash_value ^= string_hasher(std::string(str)) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
        }
    }
    return hash_value;
}

char *Wordle::to_SetType(std::string &in)
{
    char *ret;
    for (char c : in)
    {
        ret += std::toupper(c);
    }
    return ret;
}

R_STRING Wordle::to_R_STRING(std::string &in)
{
    R_STRING ret;
    for (size_t i = 0; i < 5; i++)
    {
        ret[std::toupper(in[i])].insert(i);
    }
    return ret;
}

void Wordle::print_best_word(std::pair<R_STRING, double> &input, std::ostream &os)
{
    char word[5];
    for (auto r_string : input.first)
    {
        for (auto c : r_string.second)
        {
            word[c] = r_string.first;
        }
    }
    os << word << ": " << input.second << "\n";
}

size_t Wordle::color_guess(char *&word, R_STRING &guess) // reasonably tested
{                                                        // should return a value between 0 and 242
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

double Wordle::naive_information_estimate(SetType &curr_words, R_STRING guess)
{ //  could be beneficial to create and destory these sets immediately, saving memory
    // map not unordered_map bc I want to be able to iterate through quickly
    // even though vectors do perfect hash, it will leave a lot of empty spaces
    std::map<int, Int> seperated_words;
    for (char *&word : curr_words)
    {
        ++seperated_words[color_guess(word, guess)];
    }
    double ret = 0;
    for (std::pair<const int, Int> freq : seperated_words)
    {
        // ret += freq.second.num * freq.second.num; // geometric mean w/o normalization
        ret += std::log(freq.second.num) * freq.second.num; // logorithmix mean w/0 normalization
    }
    return ret;
}

void Wordle::create_subgroups(SetType &curr_words, R_STRING &guess, std::map<size_t, SetType> &possibilities)
{ // small return value means it's a good guess
    for (char *&word : curr_words)
    {
        possibilities[color_guess(word, guess)].push_back(word);
    }
}

double Wordle::exact_information_value(SetType &curr_words, R_STRING guess)
{
    double running_total = 0; // I don't think any sort of pruning is possible (maybe for small cases?)
    std::map<size_t, SetType> possibilities;
    for (std::pair<const size_t, SetType> &possibility : possibilities)
    {
        running_total += calculate_set_value(possibility.second) * possibility.second.size();
    }
    return running_total;
}

double Wordle::calculate_set_value(SetType &curr_words, std::optional<size_t> curr_hash_in) // add base cases at some point
{
    if (curr_words.size() < 3)
    {
        return 0.5+curr_words.size()/2;
    }
    size_t curr_hash = curr_hash_in.has_value() ? curr_hash_in.value() : set_hasher(curr_words);
    double ret = 10;
    auto it = word_value_map.find(curr_hash);
    if (it != word_value_map.end())
    {
        return it->second.second;
    }
    if (curr_words.size() < 100)
    {
        // call some ofther optimizer
    }
    // for guess in possible guesses, find how many words they'd lead to on average. Then take the best ones and recurse
    std::priority_queue<std::pair<double, R_STRING>,std::vector<std::pair<double, R_STRING>>,PQComp> heap;
    R_STRING worst_guess = {{'1', {0, 1, 2, 3, 4}}};
    double worst_guess_info = naive_information_estimate(curr_words, worst_guess);
    for (size_t guess_inx = 0; guess_inx < MOST_VALID_GUESSES; guess_inx++)
    {

        double curr_guess_info = naive_information_estimate(curr_words, GUESSES[guess_inx]);
        if (curr_guess_info + 0.1 < worst_guess_info)
        { // this 0.1 figure is arbitrary and can likely be increased
            // std::pair<double, R_STRING> to_add = {curr_guess_info,GUESSES[guess_inx]};
            heap.emplace(curr_guess_info,GUESSES[guess_inx]); // use emplace?
        }
    }
    for (size_t guess_inx = MOST_VALID_GUESSES; guess_inx < GUESSES.size(); guess_inx++)
    {
        double curr_guess_info = naive_information_estimate(curr_words, GUESSES[guess_inx]);
        if (curr_guess_info < heap.top().first)
        {
            heap.pop();
            heap.emplace(curr_guess_info, GUESSES[guess_inx]);
        }
    }
    std::deque<R_STRING> best_naive_guesses_in_order;
    while (!heap.empty())
    {
        best_naive_guesses_in_order.push_front(heap.top().second);
        heap.pop();
    }
    double best_value = 1000000; // IMPORTANT: To prevent excessive division, stores expected guesses * curr_words.size()
    R_STRING best_guess;
    for (R_STRING guess : best_naive_guesses_in_order)
    {
        double curr_value = exact_information_value(curr_words, guess);
        if (curr_value < best_value)
        {
            best_value = curr_value;
            best_guess = guess;
        }
    }
    word_value_map[curr_hash] = {best_guess, 1 + best_value / curr_words.size()}; // Idk if I should add the +1 here
    return word_value_map[curr_hash].second;
}

std::pair<R_STRING, double> Wordle::find_best_word(SetType &curr_words)
{
    if (curr_words.size() > 2)
    {
        size_t curr_hash = set_hasher(curr_words);
        auto it = word_value_map.find(curr_hash);
        if (it != word_value_map.end())
        {
            return it->second;
        }
        calculate_set_value(curr_words, curr_hash);
        return word_value_map[curr_hash];
    }
    std::string first_word(curr_words[0]);
    return {to_R_STRING(first_word),0.5+curr_words.size()/2};
}

void Wordle::prep_guesses_and_answers()
{
    std::ifstream answers_file(ANSWERS_FILE);
    std::string curr_word;
    while (std::getline(answers_file, curr_word))
    {
        ANSWERS.push_back(to_SetType(curr_word));
    }
    answers_file.close();
    std::ifstream guesses_file(GUESSES_FILE);
    while (std::getline(guesses_file, curr_word))
    {
        GUESSES.push_back(to_R_STRING(curr_word));
    }
    guesses_file.close();
}
