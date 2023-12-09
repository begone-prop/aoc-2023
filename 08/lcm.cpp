#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <map>

typedef struct Node {
    std::string name;
    std::string name_left;
    std::string name_right;

    struct Node *left;
    struct Node *right;

    static Node from_string(const std::string&);
    Node *move(char) const;
    bool is_begin(void) const {return name[name.size() - 1] == 'A';};
    bool is_end(void) const {return name[name.size() - 1] == 'Z';};
    bool operator<(const Node&) const;
} Node;

bool Node::operator<(const Node &other) const {
    return this->name < other.name;
}

Node *Node::move(char c) const {
    if(c == 'R') return this->right;
    else return this->left;
}

Node Node::from_string(const std::string& s) {
    std::stringstream ss(s);
    std::string name, l, r, _;
    ss >> name >> _ >> l >> r;

    return (Node){
        .name = name,
        .name_left = l.substr(1, 3),
        .name_right = r.substr(0, 3),
        .left = NULL,
        .right = NULL,
    };
}


std::ostream &operator<<(std::ostream &os, Node const &n) {
    return os << "[" << n.name << "]: " << n.name_left << " " << n.name_right;
}

typedef struct Move {
    const std::string instructions;
    size_t current = 0;

    Move(const std::string& s) : instructions(s) {};
    char next(void);
    void reset(void) { current = 0; };
} Move;

char Move::next(void) {
    char c = this->instructions[current];
    this->current = (this->current + 1) % this->instructions.size();
    return c;
}

// https://en.wikipedia.org/wiki/Least_common_multiple
size_t lcm(const std::vector<size_t>& nums) {
    size_t x = nums[0];
    for(size_t idx = 1; idx < nums.size(); idx++) {
        // somehow std::gcd is a C++17 thinig
        x = x * nums[idx] / std::gcd(x, nums[idx]);
    }

    return x;
}

int main(void) {
    std::ifstream input_data("./input");
    std::string line;

    std::getline(input_data, line);
    Move moves = Move(line);
    std::getline(input_data, line);

    std::map<std::string, Node> nodes;

    while(std::getline(input_data, line)) {
        Node node = Node::from_string(line);
        nodes[node.name] = node;
    }

    for(const auto& entry : nodes) {
        Node *n = &nodes[entry.first];
        n->left = &nodes[n->name_left];
        n->right = &nodes[n->name_right];
    }

    Node *current = &nodes["AAA"];
    Node *end = &nodes["ZZZ"];
    size_t num_moves = 0;

    while(current != end) {
        current = current->move(moves.next());
        num_moves++;
    }

    std::cout << "Part 1: " << num_moves << '\n';

    std::vector<const Node*> start_nodes;

    for(const auto& entry : nodes) {
        if(entry.second.is_begin()) start_nodes.push_back(&entry.second);
    }

    std::vector<size_t> counts(start_nodes.size());
    for(size_t idx = 0; idx < counts.size(); idx++) {
        counts[idx] = 0;
    }

    moves.reset();
    bool done = false;

    while(!done) {
        char c = moves.next();
        done = true;
        for(size_t idx = 0; idx < start_nodes.size(); idx++) {
            if(start_nodes[idx]->is_end()) continue;

            start_nodes[idx] = start_nodes[idx]->move(c);
            counts[idx]++;
            done = false;
        }
    }

    std::cout << "Part 2: " << lcm(counts) << '\n';
}
