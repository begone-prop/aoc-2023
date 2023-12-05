#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>
#include <tuple>
#include <map>

std::tuple<int, int> read_number(std::string& str, size_t off) {
    std::string number{};
    size_t len = 0;

    while(isdigit(str[off])) {
        number.push_back(str[off++]);
        len++;
    }

    return {atoi(number.c_str()), len};
}


typedef struct Coord {
    int x, y;

    bool operator<(const Coord& o) const;
} Coord;

bool Coord::operator<(const Coord& o) const {
    if(x < o.x) return true;
    if(x > o.x) return false;
    if(y < o.y) return true;
    if(y > o.y) return false;
    return false;
}


typedef struct Part {
    Coord pos;
    int value;
    int size;

    std::vector<Coord> neighbours(void) const;
} Part;

std::vector<Coord> Part::neighbours(void) const {
    std::vector<Coord> neigh;

    for(int i = -1; i <= 1; i++) {
        neigh.push_back((Coord){this->pos.x - 1, this->pos.y + i});
        neigh.push_back((Coord){this->pos.x + this->size, this->pos.y + i});
    }

    for(int x = this->pos.x; x < this->pos.x + this->size; x++) {
        neigh.push_back((Coord){x, this->pos.y + 1});
        neigh.push_back((Coord){x, this->pos.y - 1});
    }

    return neigh;
}

int main(void) {
    std::ifstream input_data("./input");
    std::string line;
    std::vector<std::string> lines;

    std::vector<Part> parts;
    std::map<Coord, std::vector<Part>> gears;

    int y = 0;
    while(std::getline(input_data, line)) {
        for(int x = 0; (size_t)x < line.size(); x++) {
            if(isdigit(line[x])) {
                std::tuple<int, int> n = read_number(line, x);
                Part p = (Part){x, y, std::get<0>(n), std::get<1>(n)};
                parts.push_back(p);
                x += p.size;
            }

            if(line[x] == '*') gears[(Coord){x, y}] = {};
        }

        lines.push_back(line);
        y++;
    }

    int width = lines[0].size() - 1;
    int height = lines.size() - 1;


    int sum1 = 0;
    for(const Part& p : parts) {
        for(const Coord& c : p.neighbours()) {
            if(c.x < 0 || c.x > width || c.y < 0 || c.y > height)
                continue;

            char s = lines[c.y][c.x] ;

            if((s >= '0' && s <= '9') || s == '.')
                continue;

            sum1 += p.value;

            if(s == '*') {
                gears[(Coord){c.x, c.y}].push_back(p);
            }
        }
    }


    long sum2 = 0;
    for(auto const &v : gears) {
        if(v.second.size() != 2)
            continue;

        long prod = 1;
        for(auto const &x : v.second) {
            prod *= x.value;
        }

        sum2 += prod;
    }

    std::cout << "Part 1: " << sum1 << '\n';
    std::cout << "Part 2: " << sum2 << '\n';
    return 0;
}
