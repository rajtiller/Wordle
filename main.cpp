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
    for (auto a : ANSWERS)
    {
        std::cout << a;
    }
    // std::pair<R_STRING,double> best_word = find_best_word(ANSWERS);
    // print_best_word(best_word,std::cout);
    return 0;
}
