#!/usr/bin/env python

from math import floor

if __name__ == "__main__":
    with open("./input") as fp:
        data = [line.strip() for line in fp]

    answ1 = 0
    cards = [1] * len(data)

    for idx, line in enumerate(data):
        a, b = [
            set([int(v) for v in half.strip().split(" ") if v])
            for half in line.split(":")[1].split("|")
        ]

        points = len(a.intersection(b))
        answ1 += floor(pow(2, points - 1))

        for r in range(idx + 1, idx + points + 1):
            cards[r] += cards[idx]

    print(f"Part 1: {answ1}")
    print(f"Part 2: {sum(cards)}")
