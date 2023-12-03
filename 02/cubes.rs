use std::fs::read_to_string;

#[derive(Debug)]
struct Hand {
    red: u32,
    green: u32,
    blue: u32,
}

impl Hand {
    fn from_vec(data: &Vec<&str>) -> Self {
        let mut hand = Hand {
            red: 0,
            green: 0,
            blue: 0,
        };

        for e in data.iter().map(|e| e.split(" ").collect::<Vec<_>>()) {
            match e[1] {
                "red" => hand.red = e[0].parse::<u32>().unwrap(),
                "green" => hand.green = e[0].parse::<u32>().unwrap(),
                "blue" => hand.blue = e[0].parse::<u32>().unwrap(),
                &_ => unreachable!(),
            }
        }

        return hand;
    }
}

fn main() {
    let data = read_to_string("./big_input").unwrap();

    let avail = Hand {
        red: 12,
        green: 13,
        blue: 14,
    };

    let mut sum1 = 0;
    let mut sum2 = 0;
    for (game, line) in data.lines().enumerate() {
        let hands = line
            .split(":")
            .last()
            .unwrap()
            .split(";")
            .map(|h| h.split(",").map(|e| e.trim()).collect::<Vec<_>>())
            .into_iter()
            .map(|f| Hand::from_vec(&f))
            .collect::<Vec<_>>();

        let mut ok = true;
        let mut max_hand = Hand {
            red: 0,
            green: 0,
            blue: 0,
        };

        for hand in hands {
            if hand.red > avail.red || hand.green > avail.green || hand.blue > avail.blue {
                ok = false;
            }

            max_hand.red = std::cmp::max(max_hand.red, hand.red);
            max_hand.green = std::cmp::max(max_hand.green, hand.green);
            max_hand.blue = std::cmp::max(max_hand.blue, hand.blue);

        }

        sum2 += max_hand.red * max_hand.green * max_hand.blue;

        if ok {
            sum1 += game + 1;
        }
    }

    println!("Part 1: {}", sum1);
    println!("Part 2: {}", sum2);
}
