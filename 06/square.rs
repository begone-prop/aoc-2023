use std::fs::read_to_string;

fn quad(t: f64, d: f64) -> i64 {
    ((t + (t * t - 4.0 * d).sqrt()) / 2.0).ceil() as i64
        - ((t - (t * t - 4.0 * d).sqrt()) / 2.0).floor() as i64
        - 1
}

fn main() {
    let td = read_to_string("./input")
        .unwrap()
        .lines()
        .map(|l| {
            l.trim()
                .split(":")
                .last()
                .unwrap()
                .split(" ")
                .filter(|e| *e != "")
                .map(|n| n.parse::<f64>().unwrap())
                .collect::<Vec<f64>>()
        })
        .collect::<Vec<Vec<f64>>>();

    let part1 = std::iter::zip(&td[0], &td[1])
        .map(|(t, d)| quad(*t, *d))
        .product::<i64>();

    println!("Part 1: {part1}");

    let td = &td
        .iter()
        .map(|e| {
            e.iter()
                .map(|n| n.to_string())
                .collect::<String>()
                .parse::<f64>()
                .unwrap()
        })
        .collect::<Vec<f64>>();

    let part2 = quad(td[0], td[1]);
    println!("Part 2: {part2}");
}
