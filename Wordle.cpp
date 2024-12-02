#include "Wordle.h"

char *&Wordle::to_upper_case(std::string &in)
{
    char *ret;
    for (size_t i = 0; i < 5; i++)
    {
        ret += std::toupper(in[i]);
    }
    return ret;
}

std::pair<R_STRING, char *> Wordle::to_R_STRING(std::string &in)
{
    std::pair<R_STRING, char *> ret;
    for (size_t i = 0; i < 5; i++)
    {
        ret.first[std::toupper(in[i])].push_back(i);
        ret.second += in[i];
    }
    return ret;
}

void Wordle::print_best_word(std::pair<char *, double> &input, std::ostream &os)
{
    std::string word(5, 'X');
    os << input.first << ": " << input.second << "\n";
}

size_t Wordle::color_guess(char *&word, std::pair<R_STRING, char *> &guess)
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
                ret |= (3 << 2 * word_inx);
            }
            else
            {
                for (int &inx : it->second)
                {
                    if ((ret >> 2 * inx & 0b11) == 0b00)
                    {
                        ret |= (1 << 2 * inx);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}


double Wordle::naive_information_estimate(SetType &curr_words, std::pair<R_STRING, char *> &guess)
{ //  could be beneficial to create and destory these sets immediately, saving memory
    // map not unordered_map bc I want to be able to iterate through quickly
    // even though vectors do perfect hash, it will leave a lot of empty spaces
    std::unordered_map<int, Int> seperated_words;
    for (char *&word : curr_words)
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

void Wordle::create_subgroups(SetType &curr_words, std::pair<R_STRING, char *> &guess, std::unordered_map<size_t, SetType> &possibilities)
{ // small return value means it's a good guess
    for (char *&word : curr_words)
    {
        possibilities[color_guess(word, guess)].push_back(word);
    }
    possibilities.erase(255); // cause when the guess equals the word, we don't want that to add to the total
}

double Wordle::exact_information_value(SetType &curr_words, std::pair<R_STRING, char *> &guess)
{
    double running_total = 0; // I don't think any sort of pruning is possible (maybe for small cases?)
    std::unordered_map<size_t, SetType> possibilities;
    create_subgroups(curr_words, guess, possibilities);
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
        return (1 + curr_words.size()) / 2.0;
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
    std::priority_queue<std::pair<double, std::pair<R_STRING, char *>>, std::vector<std::pair<double, std::pair<R_STRING, char *>>>, PQComp> heap;
    std::pair<R_STRING, char *> worst_guess = {{{'1', {0, 1, 2, 3, 4}}}, "11111"};
    double worst_guess_info = naive_information_estimate(curr_words, worst_guess);
    heap.emplace(worst_guess_info, worst_guess);
    for (size_t guess_inx = 0; guess_inx < MOST_VALID_GUESSES; guess_inx++)
    {
        // IDEA: Make guesses passed (not by reference) and delete obviously bad guesses as you go
        double curr_guess_info = naive_information_estimate(curr_words, GUESSES[guess_inx]);
        if (curr_guess_info + 0.1 < worst_guess_info)
        { // this 0.1 figure is arbitrary and can likely be increased
            // std::pair<double, R_STRING> to_add = {curr_guess_info,GUESSES[guess_inx]};
            heap.emplace(curr_guess_info, GUESSES[guess_inx]); // use emplace?
        }
        else
        {
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
    std::deque<std::pair<R_STRING, char *>> best_naive_guesses_in_order;
    while (!heap.empty())
    {
        best_naive_guesses_in_order.push_front(heap.top().second);
        heap.pop();
    }
    double best_value = 1000000; // IMPORTANT: To prevent excessive division, stores expected guesses * curr_words.size()
    std::pair<R_STRING, char *> best_guess;
    for (std::pair<R_STRING, char *> &guess : best_naive_guesses_in_order)
    {
        double curr_value = exact_information_value(curr_words, guess);
        if (curr_value < best_value)
        {
            best_value = curr_value;
            best_guess = guess;
        }
    }
    word_value_map[curr_words] = {best_guess.second, best_value / curr_words.size()}; // Idk if I should add the +1 here
    return best_value / curr_words.size();
}

std::pair<char *, double> Wordle::find_best_word(SetType &curr_words)
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
    return {curr_words[0], (1 + curr_words.size()) / 2.0};
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
