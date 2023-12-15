#include <fcntl.h>
#include <iostream>
#include <map>
#include <openssl/evp.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


// compile this program with -lcrypto

#define OPENSSL_ENGINE NULL

enum Direction {
    DIR_UP = 0,
    DIR_LEFT,
    DIR_DOWN,
    DIR_RIGHT,
    DIR_SIZE
};

Direction reverse_map[] = {
    [DIR_UP]    = DIR_DOWN,
    [DIR_LEFT]  = DIR_RIGHT,
    [DIR_DOWN]  = DIR_UP,
    [DIR_RIGHT] = DIR_LEFT,
};

typedef struct Vec2 {
    long x, y;

    Vec2 operator+(const Vec2 &v) const { return {x + v.x, y + v.y}; }
    Vec2 operator-(const Vec2 &v) const { return {x - v.x, y - v.y}; }
} Vec2;

Vec2 direction_map[] = {
    [DIR_UP]    = { 0, -1},
    [DIR_LEFT]  = {-1,  0},
    [DIR_DOWN]  = { 0,  1},
    [DIR_RIGHT] = { 1,  0},
};

typedef struct Arena {
    char *data;
    long width, height;

    Arena(const std::string&);
    ~Arena(void) {munmap(data, size());}
    size_t size(void) const {return width*height;}
    long index(const Vec2& v) const {return v.x + v.y * width;}
    char at(Vec2 v) const {return data[index(v)];}
    void at(Vec2 v, char c) { data[index(v)] = c;}
    bool in_bounds(const Vec2& v) const {return v.x >= 0 && v.x < width -1 && v.y >= 0 && v.y < height;}
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

Vec2 pos_from_dir(const Arena& ar, long v, Direction d) {
    std::map<Direction, Vec2> loop_map = {
        {DIR_UP, {v, 0}},
        {DIR_LEFT, {0, v}},
        {DIR_DOWN, {v, ar.height - 1}},
        {DIR_RIGHT, {ar.width - 2, v}},
    };

    return loop_map[d];
}

void tilt(Arena& ar, Direction dir) {
    for(long idx = 0; idx < ar.height; idx++) {
        Vec2 pos = pos_from_dir(ar, idx, dir);

        for(; ar.in_bounds(pos); pos = pos + direction_map[reverse_map[dir]]) {
            if(ar.at(pos) != 'O') continue;
            Vec2 next = pos + direction_map[dir];

            while(ar.in_bounds(next) && ar.at(next) == '.') {
                next = next + direction_map[dir];
            }

            ar.at(pos, '.');
            ar.at(next + direction_map[reverse_map[dir]], 'O');
        }
    }
}

long load(const Arena &arena) {
    int sum = 0;
    for(long y = 0; y < arena.height; y++) {
        for(long x = 0; x < arena.width - 1; x++) {
            Vec2 pos = {x, y};
            if(arena.at(pos) == 'O') sum += arena.height - y;
        }
    }

    return sum;
}

std::string sha256sum(const char *string) {
    EVP_MD_CTX *mdCtx = EVP_MD_CTX_new();
    unsigned char mdVal[EVP_MAX_MD_SIZE];
    unsigned int mdLen;

    EVP_DigestInit_ex(mdCtx, EVP_sha256(), OPENSSL_ENGINE);
    EVP_DigestUpdate(mdCtx, string, strlen((const char *)string));

    EVP_DigestFinal_ex(mdCtx, mdVal, &mdLen);
    EVP_MD_CTX_free(mdCtx);

    return std::string((char*)mdVal);
}

int main(void) {
    Arena arena = Arena("./input");

    tilt(arena, DIR_UP);
    std::cout << "Part 1: " << load(arena) << '\n';

    long rounds = 1000000000;
    std::map<std::string, long> hashes;

    for(long round = 1; round <= rounds; round++) {
        for(int d = DIR_UP; d < DIR_SIZE; d++) {
            tilt(arena, (Direction)d);
        }

        std::string sha = sha256sum(arena.data);
        if(!hashes.count(sha)) {
            hashes[sha] = round;
            continue;
        }

        long cycle = (rounds - round) % (round - hashes[sha]);

        for(long idx = 0; idx < cycle; idx++) {
            for(int d = DIR_UP; d < DIR_SIZE; d++) {
                tilt(arena, (Direction)d);
            }
        }

        break;
    }

    std::cout << "Part 2: " << load(arena) << '\n';
    return 0;
}
