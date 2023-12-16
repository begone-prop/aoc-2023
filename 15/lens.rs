use std::collections::{BTreeMap, HashMap};
use std::fs::read_to_string;

fn hash(s: &str) -> u32 {
    s.trim().chars().fold(0, |a, c| ((a + c as u32) * 17) % 256)
}

#[derive(Debug)]
enum Op {
    Del,
    Add(u32),
}

#[derive(Debug)]
struct Instruction {
    label: String,
    op: Op,
}

impl Instruction {
    fn from_string(s: &str) -> Self {
        Self {
            label: s
                .chars()
                .take_while(|c| c.is_alphabetic())
                .collect::<String>(),
            op: match s.rfind("=") {
                Some(_) => Op::Add(s.split("=").last().unwrap().trim().parse().unwrap()),
                None => Op::Del,
            },
        }
    }
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
struct Entry {
    index: u32,
    value: u32,
}

fn main() {
    let data = read_to_string("./input").unwrap();

    let sum: u32 = data.split(",").map(|s| hash(s)).sum();

    println!("Part 1: {sum}");

    let mut boxes: HashMap<u32, BTreeMap<String, Entry>> = HashMap::new();

    for (idx, tok) in data
        .lines()
        .take(1)
        .map(|t| t.clone())
        .collect::<String>()
        .split(",")
        .enumerate()
    {
        let inst = Instruction::from_string(tok);
        let h = hash(&inst.label);

        match boxes.get_mut(&h) {
            Some(b) => match inst.op {
                Op::Add(v) => {
                    if b.get(&inst.label).is_some() {
                        let i = b.get(&inst.label).unwrap();
                        b.insert(
                            inst.label,
                            Entry {
                                index: i.index,
                                value: v,
                            },
                        );
                    } else {
                        b.insert(
                            inst.label,
                            Entry {
                                index: idx as u32,
                                value: v,
                            },
                        );
                    }
                }
                Op::Del => {
                    b.remove(&inst.label);
                }
            },
            None => match inst.op {
                Op::Add(v) => {
                    let bts = BTreeMap::from([(inst.label, Entry { index: 0, value: v })]);
                    boxes.insert(h, bts);
                }
                Op::Del => (),
            },
        }
    }

    // terrible solution, need to store and sort the index of insertion
    // since none of rust containers from std::collections keep track of it
    let mut sum: u32 = 0;
    for idx in 0..=255 {
        if boxes.get(&idx).is_some_and(|m| m.len() > 0) {
            let mut v = boxes
                .get(&idx)
                .unwrap()
                .iter()
                .collect::<Vec<_>>();

            v.sort_by_key(|(_, k)| k.index);
            sum += v.iter().enumerate().map(|(i, (_, e))| (idx + 1) * (i + 1) as u32 * e.value).sum::<u32>();
        }
    }

    println!("Part 2: {sum}");
}
