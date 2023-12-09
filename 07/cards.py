#!/usr/bin/env python

from dataclasses import dataclass
from enum import Enum
from typing import Self


class Rank(Enum):
    High = 1
    Pair = 2
    TwoPair = 3
    ThreeOfAKind = 4
    FullHouse = 5
    FourOfAkind = 6
    FiveOfAkind = 7


card_lookup = {
    "2": 2,
    "3": 3,
    "4": 4,
    "5": 5,
    "6": 6,
    "7": 7,
    "8": 8,
    "9": 9,
    "T": 10,
    "J": 11,
    "Q": 12,
    "K": 13,
    "A": 14,
}


@dataclass
class Hand:
    cards: str
    rank: Rank
    bid: int

    def __lt__(self, o: Self) -> bool:
        if self.rank == o.rank:
            for a, b in zip(self.cards, o.cards):
                if a == b:
                    continue

                return card_lookup[a] < card_lookup[b]

        return False

    def use_joker(self) -> Rank:
        return Rank.High


if __name__ == "__main__":
    with open("./input") as fp:
        data = [line.strip() for line in fp]

    hands = []
    for line in data:
        cards = line.split(" ")[0]
        bid = int(line.split(" ")[1])
        counts = {c: cards.count(c) for c in set(cards)}
        kmax = max(counts.values())
        clen = len(counts)

        if clen == 5:
            rank = Rank.High
        elif clen == 4:
            rank = Rank.Pair
        elif clen == 3:
            if kmax == 3:
                rank = Rank.ThreeOfAKind
            else:
                rank = Rank.TwoPair
        elif clen == 2:
            if kmax == 4:
                rank = Rank.FourOfAkind
            else:
                rank = Rank.FullHouse
        elif clen == 1:
            rank = Rank.FiveOfAkind

        hands.append(Hand(cards, rank, bid))  # pyright: ignore

    hands = sorted(hands, key=lambda h: h.rank.value)
    part1 = sum(i * h.bid for i, h in enumerate(sorted(hands), start=1))
    print(f"Part 1: {part1}")

    card_lookup["J"] = 1

    for hand in hands:
        jokers = hand.cards.count("J")

        if jokers == 1:
            if hand.rank == Rank.High:
                hand.rank = Rank.Pair

            elif hand.rank == Rank.Pair:
                hand.rank = Rank.ThreeOfAKind

            elif hand.rank == Rank.TwoPair:
                hand.rank = Rank.FullHouse

            elif hand.rank == Rank.ThreeOfAKind:
                hand.rank = Rank.FourOfAkind

            elif hand.rank == Rank.FourOfAkind:
                hand.rank = Rank.FiveOfAkind

        elif jokers == 2:
            if hand.rank == Rank.Pair:
                hand.rank = Rank.ThreeOfAKind
            elif hand.rank == Rank.TwoPair:
                hand.rank = Rank.FourOfAkind
            elif hand.rank == Rank.FullHouse:
                hand.rank = Rank.FiveOfAkind

        elif jokers == 3:
            if hand.rank == Rank.FullHouse:
                hand.rank = Rank.FiveOfAkind

            elif hand.rank == Rank.ThreeOfAKind:
                hand.rank = Rank.FourOfAkind

        elif jokers == 4:
            hand.rank = Rank.FiveOfAkind

    hands = sorted(hands, key=lambda h: h.rank.value)
    part2 = sum(i * h.bid for i, h in enumerate(sorted(hands), start=1))
    print(f"Part 2: {part2}")
