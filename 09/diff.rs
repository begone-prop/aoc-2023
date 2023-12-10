use std::fs::read_to_string;

// really interesting video on this topic
// https://www.youtube.com/watch?v=4AuV93LOPcE

fn next_term(nums: &Vec<i64>) -> i64 {
    match nums.iter().all(|e| *e == 0) {
        true => 0,
        false => {
            nums.last().unwrap()
                + next_term(&nums.windows(2).map(|e| (e[1] - e[0])).collect::<Vec<_>>())
        }
    }
}

fn main() {
    let numbers: Vec<Vec<i64>> = read_to_string("./input")
        .unwrap()
        .lines()
        .map(|l| {
            l.trim()
                .split(" ")
                .map(|n| n.parse::<i64>().unwrap())
                .collect()
        })
        .collect();

    println!(
        "Part 1: {:?}",
        numbers.iter().map(|n| next_term(n)).sum::<i64>()
    );

    println!(
        "Part 2: {:?}",
        numbers
            .iter()
            .map(|n| next_term(&n.iter().rev().cloned().collect::<Vec<i64>>()))
            .sum::<i64>()
    );
}
