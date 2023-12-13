#!/usr/bin/env python

from collections import namedtuple
from itertools import chain

Vec2 = namedtuple("Pos", ["x", "y"])


def travel(gs, cols, rows, exp):
    dist = 0
    gs = gs.copy()
    while gs:
        current = gs.pop()

        for gal in gs:
            start_x, end_x = sorted((current.x, gal.x))
            start_y, end_y = sorted((current.y, gal.y))

            inter_x = set(range(start_x + 1, end_x + 1))
            inter_y = set(range(start_y + 1, end_y + 1))

            dist += (
                len(inter_x - cols)
                + len(inter_y - rows)
                + len(inter_x & cols) * exp
                + len(inter_y & rows) * exp
            )

    return dist


if __name__ == "__main__":
    with open("./input") as fp:
        data = [line.strip() for line in fp]

    galaxies = [
        Vec2(x % len(data), x // len(data))
        for x, c in enumerate(chain(*data))
        if c == "#"
    ]

    erows = {idx for idx, line in enumerate(data) if all(c == "." for c in line)}
    ecols = {
        c
        for c in range(len(data))
        if all(c == "." for c in "".join(line[c] for line in data))
    }

    print(f"Part 1: {travel(galaxies, ecols, erows, 2)}")
    print(f"Part 2: {travel(galaxies, ecols, erows, 1000000)}")
