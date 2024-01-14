use rand;
use rand::seq::SliceRandom;
use std::collections::{HashMap, HashSet};
use std::fs::File;
use std::io::{self, BufRead, Error};
use std::vec::Vec;

struct Graph {
    name2ind: HashMap<String, usize>,
    edges: Vec<(usize, usize)>,
}

struct UnionFind {
    parents: Vec<usize>,
    ranks: Vec<usize>,
    cardinalities: Vec<usize>,
    num_roots: usize,
}

impl From<&Graph> for UnionFind {
    fn from(graph: &Graph) -> Self {
        let num_nodes = graph.name2ind.len();
        Self {
            parents: (0..num_nodes).collect(),
            ranks: vec![0; num_nodes],
            cardinalities: vec![1; num_nodes],
            num_roots: num_nodes,
        }
    }
}

impl UnionFind {
    fn find(&mut self, x: usize) -> usize {
        if self.parents[x] != x {
            self.parents[x] = self.find(self.parents[x])
        }
        self.parents[x]
    }

    fn union(&mut self, x: usize, y: usize) {
        let x_root = self.find(x);
        let y_root = self.find(y);

        if x_root == y_root {
            return;
        }

        if self.ranks[x_root] < self.ranks[y_root] {
            self.parents[x_root] = y_root;
            self.cardinalities[y_root] += self.cardinalities[x_root];
        } else if self.ranks[x_root] > self.ranks[y_root] {
            self.parents[y_root] = x_root;
            self.cardinalities[x_root] += self.cardinalities[y_root];
        } else {
            self.parents[y_root] = x_root;
            self.cardinalities[x_root] += self.cardinalities[y_root];
            self.ranks[x_root] += 1;
        };

        self.num_roots -= 1;
    }
}

fn read_graph<I: Iterator<Item = Result<String, Error>>>(lines: I) -> Result<Graph, Error> {
    let mut name2ind: HashMap<String, usize> = HashMap::new();
    let mut edges = vec![];

    for line_res in lines {
        let line = line_res?;
        let (source, targets) = line.split_once(": ").ok_or_else(|| {
            io::Error::new(
                io::ErrorKind::InvalidData,
                format!("Invalid line format: {}", line),
            )
        })?;

        let source_ind = {
            let curr_len = name2ind.len();
            *name2ind.entry(source.to_string()).or_insert(curr_len)
        };

        for target_str in targets.split_whitespace() {
            let curr_len = name2ind.len();
            let target_ind = *name2ind.entry(target_str.to_string()).or_insert(curr_len);
            edges.push((source_ind, target_ind));
        }
    }

    Ok(Graph { name2ind, edges })
}

fn karger(graph: &Graph) -> UnionFind {
    let mut union_find = UnionFind::from(graph);
    let mut rng = rand::thread_rng();

    while union_find.num_roots > 2 {
        let (x, y) = graph.edges.choose(&mut rng).unwrap();
        if x != y {
            union_find.union(*x, *y);
        }
    }

    union_find
}

fn find_solution(graph: &Graph) -> usize {
    loop {
        let mut union_find = karger(&graph);

        let num_cut_edges = graph
            .edges
            .iter()
            .filter(|(x, y)| union_find.find(*x) != union_find.find(*y))
            .count();

        if num_cut_edges == 3 {
            let roots: Vec<usize> = (0..graph.name2ind.len())
                .map(|v| union_find.find(v))
                .collect::<HashSet<usize>>()
                .into_iter()
                .collect();

            assert!(roots.len() == 2);
            let first_size = union_find.cardinalities[roots[0]];
            let second_size = union_find.cardinalities[roots[1]];
            assert!(first_size + second_size == graph.name2ind.len());

            return first_size * second_size;
        }
    }
}

fn main() -> std::io::Result<()> {
    let file_path = std::env::args().nth(1).unwrap();
    let file = File::open(file_path)?;

    let lines = io::BufReader::new(file).lines();
    let graph = read_graph(lines)?;

    let sol = find_solution(&graph);
    println!("Solution: {sol}");

    Ok(())
}
