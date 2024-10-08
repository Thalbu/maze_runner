"use strict";

const fs = require("fs");

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(() => {
      resolve(ms);
    }, ms);
  });
}

function carregaArquivo(filename) {
  const conteudoLabirinto = fs.readFileSync(filename, "utf-8");
  const arrayLinhas = conteudoLabirinto.split("\n");

  const [tamanhoLinhas, tamanhoColunas] = arrayLinhas[0].split(" ").map(Number);

  const matrixLabirinto = [];
  let entradaLabirinto = null;

  // encontrar entrada
  for (let i = 1; i <= tamanhoLinhas; i++) {
    const linha = arrayLinhas[i].split("");
    matrixLabirinto.push(linha);

    if (!entradaLabirinto) {
      const posicaoEntradaLabirinto = linha.indexOf("e");
      if (posicaoEntradaLabirinto !== -1) {
        entradaLabirinto = [i - 1, posicaoEntradaLabirinto];
      }
    }
  }

  return { matrixLabirinto, tamanhoLinhas, tamanhoColunas, entradaLabirinto };
}

async function printMatriz(matrixLabirinto) {
  await sleep(100);
  console.clear();
  matrixLabirinto.forEach((linha) => console.log(linha.join("")));
}

async function pesquisaSaida({
  matrixLabirinto,
  tamanhoLinhas,
  tamanhoColunas,
  entradaLabirinto,
}) {
  const posicaoAtual = [entradaLabirinto];

  while (posicaoAtual.length > 0) {
    const [x, y] = posicaoAtual.pop();

    if (matrixLabirinto[x][y] === "s") {
      console.log("Saída encontrada!");
      console.log("Posição da saida:", x, y, "\n");

      matrixLabirinto[x][y] = "o";
      await printMatriz(matrixLabirinto);

      return true;
    }

    // Marca a posição atual como visitada
    if (matrixLabirinto[x][y] === "x" || matrixLabirinto[x][y] === "e") {
      matrixLabirinto[x][y] = "o";
    }

    await printMatriz(matrixLabirinto);

    const direcoes = [
      [x - 1, y], // cima
      [x + 1, y], // baixo
      [x, y - 1], // esquerda
      [x, y + 1], // direita
    ];

    for (const [nx, ny] of direcoes) {
      if (
        nx >= 0 &&
        ny >= 0 &&
        nx < tamanhoLinhas &&
        ny < tamanhoColunas &&
        (matrixLabirinto[nx][ny] === "x" || matrixLabirinto[nx][ny] === "s")
      ) {
        matrixLabirinto[x][y] = ".";
        posicaoAtual.push([nx, ny]);
      }
    }
  }

  console.log("Nenhum caminho para a saída foi encontrado.");

  return false;
}

async function main() {
  const matriz = carregaArquivo("../data/maze.txt");
  await printMatriz(matriz.matrixLabirinto);

  console.log("Resolvendo labirinto...\n");

  await pesquisaSaida(matriz);
}

(async () => {
  await main();
})();
