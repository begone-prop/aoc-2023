#include <iostream>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <map>
#include <set>

#define IS_PIPE(C) ((C) != '.')
#define START 'S'

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
    char at(Vec2& v) const {return data[index(v)];}
    bool in_bounds(const Vec2& v) const {return v.x >= 0 && v.x < width && v.y >= 0 && v.y < height;}
} Arena;

Arena::Arena(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);

    struct stat sbuff;
    fstat(fd, &sbuff);
    char *data = (char*)mmap(NULL, sbuff.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

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

std::map<char, std::pair<Direction, Direction>> side_map = {
	{'|', {DIR_UP, DIR_DOWN}},
	{'-', {DIR_RIGHT, DIR_LEFT}},
	{'L', {DIR_UP, DIR_RIGHT}},
	{'J', {DIR_UP, DIR_LEFT}},
	{'7', {DIR_DOWN, DIR_LEFT}},
	{'F', {DIR_RIGHT, DIR_DOWN}},
};

std::map<std::pair<Direction, Direction>, char> char_map = {
	{{DIR_UP, DIR_DOWN}, '|'},
	{{DIR_RIGHT, DIR_LEFT}, '-'},
	{{DIR_UP, DIR_RIGHT}, 'L'},
	{{DIR_UP, DIR_LEFT}, 'J'},
	{{DIR_DOWN, DIR_LEFT}, '7'},
	{{DIR_RIGHT, DIR_DOWN}, 'F'},
};

std::map<std::pair<char, Direction>, std::set<char>> connection_map = {
    {{'|', DIR_UP}, {'|', 'F', '7'}},
    {{'|', DIR_DOWN}, {'|', 'L', 'J'}},
    {{'-', DIR_RIGHT}, {'-', 'J', '7'}},
    {{'-', DIR_LEFT}, {'-', 'L', 'F'}},
    {{'L', DIR_UP}, {'|', 'F', '7'}},
    {{'L', DIR_RIGHT}, {'-', '7', 'J'}},
    {{'J', DIR_UP}, {'|', 'F', '7'}},
    {{'J', DIR_LEFT}, {'-', 'F', 'L'}},
    {{'7', DIR_DOWN}, {'|', 'L', 'J'}},
    {{'7', DIR_LEFT}, {'-', 'L', 'F'}},
    {{'F', DIR_RIGHT}, {'-', 'J', '7'}},
    {{'F', DIR_DOWN}, {'|', 'L', 'J'}},
};

typedef struct Pipe {
    char symbol;
    Vec2 position;
    struct Pipe *next = NULL;

    Pipe(char c, Vec2 p = {0, 0}) : symbol(c) , position(p) {}
    static bool connects(char s, char t, Direction d) { return !!connection_map[std::make_pair(s, d)].count(t); }
    bool connects(char t, Direction d) const { return Pipe::connects(symbol, t, d); }
    void parse(const Arena&);
} Pipe;

static std::set<Vec2> visited;

void Pipe::parse(const Arena& arena) {
    for(int d = 0; d < DIR_SIZE; d++) {
        Vec2 n = this->position + direction_map[d];
        if(!arena.in_bounds(n)) continue;
        if(!IS_PIPE(arena.at(n))) continue;
        if(!this->connects(arena.at(n), (Direction)d)) continue;
        if(visited.count(n)) continue;

        visited.insert(n);

        Pipe *p = new Pipe(arena.at(n), n);
        this->next = p;
        p->parse(arena);
        break;
    }
}

int main(void) {
    Arena arena = Arena("./input");

    long off = strchr(arena.data, 'S') - arena.data;
    Vec2 start_pos = {off % arena.width, off / arena.width};

    Direction sd[2];
    int s = 0;
    for(int d = 0; d < DIR_SIZE; d++) {
        Vec2 n = start_pos + direction_map[d];
        if(!arena.in_bounds(n)) continue;
        if(!IS_PIPE(arena.at(n))) continue;

        Direction r = reverse_map[d];
        std::pair<Direction, Direction> dirs = side_map[arena.at(n)];
        if(dirs.first == r || dirs.second == r) sd[s++] = (Direction)d;
    }

    Pipe start = Pipe(char_map[std::make_pair(sd[0], sd[1])], start_pos);
    start.parse(arena);
    std::cout << "Part 1: " << visited.size() / 2 + 1 << '\n';
}
