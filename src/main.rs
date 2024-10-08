use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::thread::sleep;
use std::time::Duration;
use std::process::Command;


fn carregar_labirinto(caminho_arquivo: &str) -> io::Result<(Vec<Vec<char>>, usize, usize, (usize, usize))> {
    let path = Path::new(caminho_arquivo);
    let arquivo = File::open(&path)?;
    let mut linhas = io::BufReader::new(arquivo).lines();
    
    let primeira_linha = linhas.next().unwrap()?;
    let dimensoes: Vec<usize> = primeira_linha
        .split_whitespace()
        .map(|s| s.parse().unwrap())
        .collect();
    let linhas_count = dimensoes[0];
    let colunas_count = dimensoes[1];
    
    let mut labirinto = Vec::with_capacity(linhas_count);
    let mut inicio = (0, 0);

    // Lê o labirinto linha por linha
    for (i, linha) in linhas.enumerate() {
        let linha = linha?;
        let linha_chars: Vec<char> = linha.chars().collect();
        if let Some(j) = linha_chars.iter().position(|&c| c == 'e') {
            inicio = (i, j);
        }
        labirinto.push(linha_chars);
    }

    Ok((labirinto, linhas_count, colunas_count, inicio))
}


fn imprimir_labirinto(labirinto: &Vec<Vec<char>>) {
    sleep(Duration::from_millis(100));
    
    if cfg!(target_os = "windows") {
        Command::new("cls").status().unwrap();
    } else {
        Command::new("clear").status().unwrap();
    }

    for linha in labirinto {
        for &celula in linha {
            print!("{}", celula);
        }
        println!();
    }
}

fn resolver_labirinto(labirinto: &mut Vec<Vec<char>>, linhas: usize, colunas: usize, inicio: (usize, usize)) -> bool {
    let mut pilha = vec![inicio];
    
    while let Some((x, y)) = pilha.pop() {
        // eh a saída
        if labirinto[x][y] == 's' {
            labirinto[x][y] = 'o';

            println!("Saída encontrada!\n");
            imprimir_labirinto(labirinto);
            return true;
        }

        if labirinto[x][y] == 'x' || labirinto[x][y] == 'e' {
            labirinto[x][y] = 'o';
        }
        
        imprimir_labirinto(labirinto);
        println!();

        labirinto[x][y] = '.';

        // proxima posição
        for (nx, ny) in [(x.wrapping_sub(1), y), (x + 1, y), (x, y.wrapping_sub(1)), (x, y + 1)] {
            if nx < linhas && ny < colunas && labirinto[nx][ny] != '#' && labirinto[nx][ny] != '.' {
                pilha.push((nx, ny));
            }
        }
    }

    println!("Nenhum caminho para a saída foi encontrado.");
    false
}

fn main() -> io::Result<()> {
    let (mut labirinto, linhas, colunas, inicio) = carregar_labirinto("../data/maze.txt")?;

    imprimir_labirinto(&labirinto);

    println!("Resolvendo labirinto...\n");

    resolver_labirinto(&mut labirinto, linhas, colunas, inicio);
    
    Ok(())
}
