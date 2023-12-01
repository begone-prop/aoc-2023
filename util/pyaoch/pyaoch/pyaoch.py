#!/usr/bin/env python3

import argparse
from datetime import datetime
from os import getenv
from sys import exit, stderr, stdout

import requests
from bs4 import BeautifulSoup as bs
from markdownify import MarkdownConverter


def main():
    parser = argparse.ArgumentParser(prog="aoch")
    now = datetime.now()

    parser.add_argument(
        "-I",
        "--input",
        action="store_true",
        default=False,
        help="Retrieve puzzle input not puzzle description",
    )

    parser.add_argument(
        "-d",
        "--day",
        type=int,
        required=True,
        metavar="<DAY>",
        help="Puzzle for specified day",
    )

    parser.add_argument(
        "-y",
        "--year",
        type=int,
        metavar="<YEAR>",
        default=now.year,
        help="Puzzles for specified year, default is current year",
    )

    parser.add_argument(
        "-c",
        "--cookie",
        type=str,
        metavar="<COOKIE>",
        default="",
        help="Cookie to use when making request",
    )

    parser.add_argument(
        "-o",
        "--output",
        type=str,
        metavar="<PATH>",
        help="Write output to specified file",
    )

    args = parser.parse_args()

    cookie = args.cookie
    if not cookie:
        cookie = getenv("AOC_COOKIE")

    if not cookie:
        print(f"No cookie provided either with -c or $AOC_COOKIE")
        parser.print_usage(file=stderr)
        exit(1)

    aoc_url = "https://adventofcode.com"

    output = stdout
    if args.output:
        try:
            output = open(args.output, mode="w+", encoding="utf-8")
        except Exception as e:
            print(f"Failed to open {args.output}: {e}", file=stderr)
            exit(1)

    head = {"Cookie": f"session={cookie}"}

    url = (
        f"{aoc_url}/{args.year}/day/{args.day}/input"
        if args.input
        else f"{aoc_url}/{args.year}/day/{args.day}"
    )

    response = requests.get(url, headers=head)

    if response.status_code != 200:
        print(f"Failed getting data from {url}: {response.status_code}")
        exit(1)

    data = response.content.decode("utf-8")

    if args.input:
        print(data, end="", file=output)
    else:
        html = bs(data, "html.parser")
        articles = html.find_all("article")

        opts = {
            "wrap": True,
            "wrap_width": 80,
            "bullets": "+",
        }

        for article in articles:
            markdown = MarkdownConverter(**opts).convert_soup(article)
            print(markdown, end="", file=output)


if __name__ == "__main__":
    main()
