#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#define ALPHA 26
#define NO_MATCH {0, 0}

using Match = std::tuple<size_t, int>;

struct Trie {
    int value = 0;
    bool end = false;
    Trie *children[ALPHA] = {nullptr};

    static size_t index(char c) {return c - 'a';};
    void insert(const std::string&, int value = 0);
    Match match(const std::string&) const;
    int digit(const std::string&, bool) const;
};

void Trie::insert(const std::string& str, int value) {
    Trie *head = this;

    for(const char& s : str) {
        size_t idx = this->index(s);
        if(!head->children[idx])
            head->children[idx] = new Trie;

        head = head->children[idx];
    }

    head->value = value;
    head->end = true;
}

Match Trie::match(const std::string& str) const {
    const Trie *head = this;

    size_t depth = 1;
    for(const char &s : str) {
        if(!std::isalpha(s))
            return NO_MATCH;

        size_t idx = this->index(s);
        if(!head->children[idx]) return NO_MATCH;

        if(head->children[idx]->end) {
            return {depth, head->children[idx]->value};
        }

        head = head->children[idx];
        depth++;
    }

    return NO_MATCH;
}

int Trie::digit(const std::string& str, bool convert = true) const {
    for(size_t start = 0; start < str.size(); start++) {
        if(std::isdigit(str[start]))
            return str[start] - '0';

        if(!convert) continue;

        Match m = this->match(str.substr(start));

        if(std::get<0>(m))
            return std::get<1>(m);
    }

    return 0;
}


int main(void) {
    std::vector<std::string> numbers = {
        "one", "two", "three",
        "four", "five", "six",
        "seven", "eight", "nine"
    };

    std::vector<std::string> srebmun = {
        "eno", "owt", "eerht",
        "ruof", "evif", "xis",
        "neves", "thgie", "enin"
    };

    Trie head = Trie();
    Trie deah = Trie();

    for(size_t idx = 0; idx < numbers.size(); idx++) {
        head.insert(numbers[idx], idx + 1);
        deah.insert(srebmun[idx], idx + 1);
    }

    std::ifstream input_data("./big_input");
    std::string line;

    int sum1 = 0;
    int sum2 = 0;

    while(std::getline(input_data, line)) {
        sum1 += head.digit(line, false) * 10;
        sum2 += head.digit(line) * 10;

        // would be faster to match the string backwards
        // reverse is slow ...
        std::reverse(line.begin(), line.end());

        sum1 += deah.digit(line, false);
        sum2 += deah.digit(line);
    }

    std::cout << "Part 1: " << sum1 << '\n';
    std::cout << "Part 2: " << sum2 << '\n';
    return 0;
}
