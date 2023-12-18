use std::fs::read_to_string;

#[derive(Debug)]
enum Direction {
    Up,
    Right,
    Down,
    Left,
}

#[derive(Debug)]
struct Digs {
    dir: Direction,
    mag: i64,
    col: String,
}

// Our old friend appears again
// https://en.wikipedia.org/wiki/Shoelace_formula#Shoelace_formula
impl From<&str> for Digs {
    fn from(s: &str) -> Self {
        let d = s.trim().split_whitespace().collect::<Vec<_>>();

        let mag: i64 = d[1].parse().unwrap();

        let dir = match d[0] {
            "U" => Direction::Up,
            "R" => Direction::Right,
            "D" => Direction::Down,
            "L" => Direction::Left,
            _ => unreachable!("Got {}", d[0]),
        };

        let col = &d[2]
            .chars()
            .filter(|m| m.is_alphanumeric())
            .collect::<String>();

        Digs {
            dir,
            mag,
            col: col.to_string(),
        }
    }
}

fn main() {
    let data = read_to_string("./input").unwrap();

    let mut start = (0, 0);
    let mut holes: Vec<(i64, i64)> = Vec::from([start]);
    holes.push(start);

    let digs: Vec<Digs> = data.lines().map(|l| Digs::from(l)).collect::<Vec<_>>();

    for d in digs.iter() {
        let dig = match d.dir {
            Direction::Up => (start.0, start.1 - d.mag),
            Direction::Right => (start.0 + d.mag, start.1),
            Direction::Down => (start.0, start.1 + d.mag),
            Direction::Left => (start.0 - d.mag, start.1),
        };

        start = dig;
        holes.push(start);
    }

    println!(
        "Part 1: {:?}",
        1 + (holes
            .windows(2)
            .map(|h| h[0].0 * h[1].1 - h[1].0 * h[0].1)
            .sum::<i64>()
            .abs()
            + digs.iter().map(|d| d.mag).sum::<i64>())
            / 2
    );

    let newd = digs.iter().map(|d| {
        let mag = i64::from_str_radix(
            &d.col
                .chars()
                .take(d.col.len() - 1)
                .collect::<String>(),
            16
        ).unwrap();

        let dir = match d.col.chars().last().unwrap() {
            '0' => Direction::Right,
            '1' => Direction::Down,
            '2' => Direction::Left,
            '3' => Direction::Up,
            _ => unreachable!()
        };

        Digs {dir, mag, col: String::new()}
    }).collect::<Vec<_>>();

    let mut start = (0, 0);
    let mut holes: Vec<(i64, i64)> = Vec::from([start]);
    holes.push(start);

    for d in newd.iter() {
        let dig = match d.dir {
            Direction::Up => (start.0, start.1 - d.mag),
            Direction::Right => (start.0 + d.mag, start.1),
            Direction::Down => (start.0, start.1 + d.mag),
            Direction::Left => (start.0 - d.mag, start.1),
        };

        start = dig;
        holes.push(start);
    }

    println!(
        "Part 2: {:?}",
        1 + (holes
            .windows(2)
            .map(|h| h[0].0 * h[1].1 - h[1].0 * h[0].1)
            .sum::<i64>()
            .abs()
            + newd.iter().map(|d| d.mag).sum::<i64>())
            / 2
    );
}
