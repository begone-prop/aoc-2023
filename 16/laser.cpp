#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <map>
#include <utility>
#include <unistd.h>
#include <set>
#include <vector>

enum Direction {
    DIR_UP = 0,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    DIR_SIZE
};

Direction reverse_map[] = {
    [DIR_UP]    = DIR_DOWN,
    [DIR_RIGHT] = DIR_LEFT,
    [DIR_DOWN]  = DIR_UP,
    [DIR_LEFT]  = DIR_RIGHT
};

typedef struct Vec2 {
    long x, y;

    Vec2 operator+(const Vec2 &v) const { return {x + v.x, y + v.y}; }
    Vec2 operator-(const Vec2 &v) const { return {x - v.x, y - v.y}; }
    bool operator<(const Vec2 &o) const {if(x < o.x) return true; if(x > o.x) return false; if(y < o.y) return true; if(y > o.y) return false; return false;}
} Vec2;

Vec2 direction_map[] = {
    [DIR_UP]    = { 0, -1},
    [DIR_RIGHT] = { 1,  0},
    [DIR_DOWN]  = { 0,  1},
    [DIR_LEFT]  = {-1,  0},
};

typedef struct Arena {
    char *data;
    long width, height;

    Arena(const std::string&);
    ~Arena(void) {munmap(data, size());}
    size_t size(void) const {return width*height;}
    long index(const Vec2& v) const {return v.x + v.y * width;}
    char at(Vec2 v) const {return data[index(v)];}
    bool in_bounds(const Vec2& v) const {return v.x >= 0 && v.x < width - 1 && v.y >= 0 && v.y < height;}
} Arena;

Arena::Arena(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);

    struct stat sbuff;
    fstat(fd, &sbuff);
    char *data = (char*)mmap(NULL, sbuff.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    char *s = data;
    size_t height = 0;
    while(*s) { if(*s++ == '\n') height++; }

    s = data;
    while(*s++ != '\n') ;
    size_t width = s - data;

    this->data = data;
    this->width = width;
    this->height = height;
}

typedef struct Beam {
    Vec2 pos;
    Direction dir;
    bool operator<(const Beam &b) const { return this->pos < b.pos && this->dir < b.dir; }
} Beam;

#define IS_MIRROR(S) ((S) == '/' || (S) == '\\')

enum Mirror {
    MIRROR_FORWARD = 0,
    MIRROR_BACKWARD,
};

static std::map<char, Mirror> mirror_map {
    {'/',   MIRROR_FORWARD},
    {'\\',  MIRROR_BACKWARD},
};

static const Direction reflection_map[][DIR_SIZE] = {
    //                  DIR_UP      DIR_RIGHT   DIR_DOWN    DIR_LEFT
    [MIRROR_FORWARD]  = {DIR_RIGHT, DIR_UP,     DIR_LEFT,   DIR_DOWN},
    [MIRROR_BACKWARD] = {DIR_LEFT,  DIR_DOWN,   DIR_RIGHT,  DIR_UP},
};


#define IS_SPLITTER(S) ((S) == '|' || (S) == '-')

enum Splitter {
    SPLIT_PIPE = 0,
    SPLIT_DASH,
};

static std::map<char, Splitter> splitter_map {
    {'|', SPLIT_PIPE},
    {'-', SPLIT_DASH},
};

static const bool should_split[][4] = {
    //              DU      DR      DD      DL
    [SPLIT_PIPE] = {false,  true,   false,  true},
    [SPLIT_DASH] = {true,   false,  true,   false},
};

static const std::pair<Direction, Direction> split_map[] = {
    [SPLIT_PIPE] = {DIR_UP, DIR_DOWN},
    [SPLIT_DASH] = {DIR_RIGHT, DIR_LEFT},
};

long energize(const Beam& beam, const Arena& arena) {
    std::set<Vec2> visited;

    std::map<Vec2, std::set<Direction>> mirrors;

    std::vector<Beam> beams;
    beams.push_back(beam);

    while(!beams.empty()) {
        Beam current = beams.back();
        beams.pop_back();

        while(arena.in_bounds(current.pos) && !mirrors[current.pos].count(current.dir)) {
            if(IS_MIRROR(arena.at(current.pos))) {
                Mirror m = mirror_map[arena.at(current.pos)];
                mirrors[current.pos].insert(current.dir);
                current.dir = reflection_map[m][current.dir];
            }

            if(IS_SPLITTER(arena.at(current.pos))) {
                Splitter s = splitter_map[arena.at(current.pos)];
                if(should_split[s][current.dir]) {
                    current.dir = split_map[s].first;
                    beams.push_back((Beam){current.pos, split_map[s].second});
                }
            }

            visited.insert(current.pos);
            current.pos = current.pos + direction_map[current.dir];
        }
    }

    return visited.size();
}

int main(void) {
    Arena arena = Arena("./input");

    Beam beam = {{0, 0}, DIR_RIGHT};
    std::cout << "Part 1: " << energize(beam, arena) << '\n';

    std::vector<Beam> edge_beams;

    for(long x = 1; x < arena.width - 2; x++) {
        edge_beams.push_back((Beam){{x, 0}, DIR_DOWN});
        edge_beams.push_back((Beam){{x, arena.height - 1}, DIR_UP});
    }

    for(long y = 1; y < arena.height - 1; y++) {
        edge_beams.push_back((Beam){{0, y}, DIR_RIGHT});
        edge_beams.push_back((Beam){{arena.width - 2, y}, DIR_LEFT});
    }

    // UL
    edge_beams.push_back((Beam){{0, 0}, DIR_DOWN});
    edge_beams.push_back((Beam){{0, 0}, DIR_RIGHT});

    // UR
    edge_beams.push_back((Beam){{arena.width - 2, 0}, DIR_DOWN});
    edge_beams.push_back((Beam){{arena.width - 2, 0}, DIR_LEFT});

    // DL
    edge_beams.push_back((Beam){{0, arena.height - 1}, DIR_UP});
    edge_beams.push_back((Beam){{0, arena.height - 1}, DIR_RIGHT});

    // DR
    edge_beams.push_back((Beam){{arena.width - 2, arena.height - 1}, DIR_UP});
    edge_beams.push_back((Beam){{arena.width - 2, arena.height - 1}, DIR_LEFT});


    long max = 0;
    for(const Beam& beam : edge_beams) {
        long e = energize(beam, arena);
        if(e > max) max = e;
    }

    std::cout << "Part 2: " << max << '\n';
}
