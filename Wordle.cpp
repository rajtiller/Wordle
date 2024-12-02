#include "Wordle.h"

std::string & Wordle::to_upper_case(std::string & in) {
    for (char & c:in) {
        c = std::toupper(c);
    }
    return in;
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
    std::string word(5,'X');
    for (auto r_string : input.first)
    {
        for (auto c : r_string.second)
        {
            word[c] = r_string.first;
        }
    }
    os << word << ": " << input.second << "\n";
}

size_t Wordle::color_guess(std::string &word, R_STRING &guess) // reasonably tested
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
    for (std::string &word : curr_words)
    {
        ++seperated_words[color_guess(word, guess)];
    }
    double ret = 0;
    for (std::pair<const int, Int> freq : seperated_words)
    {
        // ret += freq.second.num * freq.second.num; // geometric mean w/o normalization
        ret += std::log(freq.second.num) * freq.second.num; // logorithmix mean w/o normalization
    }
    return ret;
}

void Wordle::create_subgroups(SetType &curr_words, R_STRING &guess, std::map<size_t, SetType> &possibilities)
{ // small return value means it's a good guess
    for (std::string &word : curr_words)
    {
        possibilities[color_guess(word, guess)].push_back(word);
    }
    possibilities.erase(255); // cause when the guess equals the word, we don't want that to add to the total
}

double Wordle::exact_information_value(SetType &curr_words, R_STRING guess)
{
    double running_total = 0; // I don't think any sort of pruning is possible (maybe for small cases?)
    std::map<size_t, SetType> possibilities;
    create_subgroups(curr_words,guess,possibilities);
    for (std::pair<const size_t, SetType> &possibility : possibilities)
    {
        running_total += calculate_set_value(possibility.second) * possibility.second.size(); // not doing division until end
    }
    return curr_words.size() + running_total;
}

double Wordle::calculate_set_value(SetType &curr_words) // add base cases at some point
{
    if (curr_words.size() < 3)
    {
        return (1+curr_words.size())/2.0;
    }
    auto it = word_value_map.find(curr_words);
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
    word_value_map[curr_words] = {best_guess, best_value / curr_words.size()}; // Idk if I should add the +1 here
    return best_value / curr_words.size();
}

std::pair<R_STRING, double> Wordle::find_best_word(SetType &curr_words)
{
    if (curr_words.size() > 2)
    {
        auto it = word_value_map.find(curr_words);
        if (it != word_value_map.end())
        {
            return it->second;
        }
        calculate_set_value(curr_words);
        return word_value_map[curr_words];
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
        ANSWERS.push_back(to_upper_case(curr_word));
    }
    answers_file.close();
    std::ifstream guesses_file(GUESSES_FILE);
    while (std::getline(guesses_file, curr_word))
    {
        GUESSES.push_back(to_R_STRING(curr_word));
    }
    guesses_file.close();
}
