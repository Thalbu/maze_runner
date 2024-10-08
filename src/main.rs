use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

#[derive(Clone)]
struct Maze {
    grid: Vec<Vec<char>>,
    rows: usize,
    cols: usize,
    start: (usize, usize),
}

impl Maze {
    fn new(filename: &str) -> io::Result<Self> {
        let path = Path::new(filename);
        let file = File::open(&path)?;
        let mut lines = io::BufReader::new(file).lines();
        
        // Lê a primeira linha para obter o número de linhas e colunas
        let first_line = lines.next().unwrap()?;
        let dims: Vec<usize> = first_line
            .split_whitespace()
            .map(|s| s.parse().unwrap())
            .collect();
        let rows = dims[0];
        let cols = dims[1];
        
        let mut grid = Vec::with_capacity(rows);
        let mut start = (0, 0);

        // Lê o labirinto linha por linha
        for (i, line) in lines.enumerate() {
            let line = line?;
            let row: Vec<char> = line.chars().collect();
            if let Some(j) = row.iter().position(|&c| c == 'e') {
                start = (i, j);
            }
            grid.push(row);
        }

        Ok(Maze { grid, rows, cols, start })
    }

    fn print_maze(&self) {
        for row in &self.grid {
            for &cell in row {
                print!("{}", cell);
            }
            println!();
        }
    }

    fn solve(&mut self) -> bool {
        let mut stack = vec![self.start];
        
        while let Some((x, y)) = stack.pop() {
            // Verifica se a posição atual é a saída
            if self.grid[x][y] == 's' {
                println!("Saída encontrada!");
                self.grid[x][y] = 'o';
                self.print_maze();
                return true;
            }
            
            // Marca a posição como explorada
            if self.grid[x][y] == 'x' || self.grid[x][y] == 'e' {
                self.grid[x][y] = 'o';
            }
            

            // Adiciona as posições adjacentes à pilha
            for (nx, ny) in [(x.wrapping_sub(1), y), (x + 1, y), (x, y.wrapping_sub(1)), (x, y + 1)] {
                if nx < self.rows && ny < self.cols && self.grid[nx][ny] != '#' && self.grid[nx][ny] != 'o' {
                    stack.push((nx, ny));
                }
            }
        }

        println!("Nenhum caminho para a saída foi encontrado.");
        false
    }
}

fn main() -> io::Result<()> {
    let mut maze = Maze::new("../data/maze.txt")?;
    maze.print_maze();
    println!("Resolving maze...\n");
    maze.solve();
    Ok(())
}
