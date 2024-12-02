#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include "Wordle.cpp"
// TODO: Create Overall Tests, Optimize?

int main()
{
    Wordle w;
    w.prep_guesses_and_answers();
    SetType sub_answers(ANSWERS.begin(),ANSWERS.begin()+100);
    std::vector<R_STRING> sub_guesses(GUESSES.begin(),GUESSES.begin()+100);
    GUESSES = sub_guesses;
    std::pair<R_STRING,double> best_word = w.find_best_word(sub_answers);
    w.print_best_word(best_word,std::cout);
    return 0;
}
