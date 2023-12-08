use std::fs::read_to_string;

#[derive(Debug)]
#[allow(dead_code)]
struct Range {
    destination: u64,
    source: u64,
    offset: u64,
}

impl Range {
    fn map_value(self: &Self, value: &u64) -> Option<u64> {
        match (self.source..=self.source + self.offset).contains(&value) {
            true => Some(self.destination + value - self.source),
            false => None,
        }
    }
}

impl From<&str> for Range {
    fn from(s: &str) -> Self {
        let x = s
            .split(" ")
            .map(|n| n.parse::<u64>().unwrap())
            .collect::<Vec<u64>>();
        Range {
            destination: x[0],
            source: x[1],
            offset: x[2],
        }
    }
}

fn main() {
    let data = read_to_string("./input").unwrap();

    let seeds: Vec<u64> = data
        .split("\n\n")
        .take(1)
        .next()
        .unwrap()
        .split(":")
        .last()
        .unwrap()
        .trim()
        .split(" ")
        .map(|n| n.parse::<u64>().unwrap())
        .collect();

    let blocks = data
        .split("\n\n")
        .skip(1)
        .map(|b| {
            b.split(":")
                .last()
                .unwrap()
                .split("\n")
                .filter(|e| *e != "")
                .map(|s| Range::from(s))
                .collect::<Vec<_>>()
        })
        .collect::<Vec<Vec<Range>>>();

    let mut min = std::u64::MAX;
    for seed in &seeds {
        let mut location = *seed;
        for block in &blocks {
            for table in block {
                if let Some(v) = table.map_value(&location) {
                    location = v;
                    break;
                }
            }
        }
        min = std::cmp::min(min, location);
    }

    println!("Part 1: {min}");

    // this will finish, JUST LET IT COOK !!!
    let mut min = std::u64::MAX;
    let mut new_min = std::u64::MAX;
    for chunk in seeds.chunks(2) {
        for seed in chunk[0]..=chunk[0]+chunk[1] {
            let mut location = seed;
            for block in &blocks {
                for table in block {
                    if let Some(v) = table.map_value(&location) {
                        location = v;
                        break;
                    }
                }
            }
            min = std::cmp::min(min, location);
        }
        new_min = std::cmp::min(new_min, min);
    }

    println!("Part 2: {new_min}");
}
