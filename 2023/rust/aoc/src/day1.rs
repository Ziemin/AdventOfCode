use std::fs::File;
use std::io::{self, BufRead};

fn part_1<'a, I: Iterator<Item = &'a [u8]>>(lines: I) -> u64 {
    lines
        .map(|text| {
            let l_ind = text.iter().position(|c: &u8| c.is_ascii_digit()).unwrap();
            let r_ind = text.iter().rposition(|c: &u8| c.is_ascii_digit()).unwrap();

            let l_digit = (text[l_ind] - b'0') as u64;
            let r_digit = (text[r_ind] - b'0') as u64;
            l_digit * 10 + r_digit
        })
        .sum()
}

const PATTERNS: [(&str, i64); 18] = [
    ("1", 1),
    ("2", 2),
    ("3", 3),
    ("4", 4),
    ("5", 5),
    ("6", 6),
    ("7", 7),
    ("8", 8),
    ("9", 9),
    ("one", 1),
    ("two", 2),
    ("three", 3),
    ("four", 4),
    ("five", 5),
    ("six", 6),
    ("seven", 7),
    ("eight", 8),
    ("nine", 9),
];

fn find_min_pattern<'a, I: Iterator<Item = (&'a str, i64)>>(
    text: &str,
    patterns: I,
) -> Option<i64> {
    patterns
        .filter_map(|(ptr, val)| text.find(ptr).map(|pos| (pos, val)))
        .min_by_key(|x| x.0)
        .map(|min_el| min_el.1)
}

fn part_2<'a, I: Iterator<Item = &'a [u8]>>(lines: I) -> u64 {
    let rev_patterns: [(String, i64); 18] =
        PATTERNS.map(|(ptr, val)| (ptr.chars().rev().collect::<String>(), val));

    lines
        .map(|text| {
            let text_str = std::str::from_utf8(text).unwrap();
            let l_digit = find_min_pattern(text_str, PATTERNS.iter().cloned()).unwrap();
            let r_digit =
                find_min_pattern(text_str, rev_patterns.iter().map(|(s, v)| (s.as_str(), *v)))
                    .unwrap();

            (l_digit as u64) * 10 + (r_digit as u64)
        })
        .sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_1() {
        let input = vec!["1abc2", "pqr3stu8vwx", "a1b2c3d4e5f", "treb7uchet"];
        assert_eq!(part_1(input.iter().map(|x| x.as_bytes())), 142);
    }

    #[test]
    fn test_2() {
        let input = vec![
            "two1nine",
            "eightwothree",
            "abcone2threexyz",
            "xtwone3four",
            "4nineeightseven2",
            "zoneight234",
            "7pqrstsixteen",
        ];
        assert_eq!(part_2(input.iter().map(|x| x.as_bytes())), 281);
    }
}

fn read_input(file: &File) -> Result<Vec<String>, std::io::Error> {
    io::BufReader::new(file).lines().collect()
}

fn main() -> std::io::Result<()> {
    let file_path = std::env::args().nth(1).unwrap();
    let file = File::open(file_path)?;

    let lines = read_input(&file)?;

    println!(
        "Part 1 {}",
        part_1(
            lines
                .iter()
                .map(AsRef::as_ref)
                .map(|text: &str| text.as_bytes())
        )
    );

    println!(
        "Part 2 {}",
        part_2(
            lines
                .iter()
                .map(AsRef::as_ref)
                .map(|text: &str| text.as_bytes())
        )
    );

    Ok(())
}
