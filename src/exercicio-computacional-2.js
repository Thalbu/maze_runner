"use strict";

const fs = require("fs");
const {
  Worker,
  isMainThread,
  parentPort,
  workerData,
} = require("worker_threads");

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

  const labirinto = [];
  let entradaLabirinto = null;

  // Encontrar entrada
  for (let i = 1; i <= tamanhoLinhas; i++) {
    const linha = arrayLinhas[i].split("");
    labirinto.push(linha);

    if (!entradaLabirinto) {
      const posicaoEntradaLabirinto = linha.indexOf("e");
      if (posicaoEntradaLabirinto !== -1) {
        entradaLabirinto = [i - 1, posicaoEntradaLabirinto];
      }
    }
  }

  return { labirinto, tamanhoLinhas, tamanhoColunas, entradaLabirinto };
}

async function printMatriz(labirinto) {
  await sleep(300);
  console.clear();
  labirinto.forEach((linha) => console.log(linha.join("")));
}

async function pesquisaSaida({
  labirinto,
  tamanhoLinhas,
  tamanhoColunas,
  entradaLabirinto,
}) {
  const posicaoAtual = [entradaLabirinto];

  while (posicaoAtual.length > 0) {
    const [x, y] = posicaoAtual.pop();

    if (labirinto[x][y] === "s") {
      console.log("Saída encontrada!");
      console.log("Posição da saida:", x, y, "\n");

      labirinto[x][y] = "o";
      await printMatriz(labirinto);

      return true;
    }

    // Marca a posição atual como visitada
    if (labirinto[x][y] === "x" || labirinto[x][y] === "e") {
      labirinto[x][y] = "o";
    }

    await printMatriz(labirinto);

    const direcoes = [
      [x - 1, y], // cima
      [x + 1, y], // baixo
      [x, y - 1], // esquerda
      [x, y + 1], // direita
    ];

    let caminhosPossiveis = [];
    for (const [nx, ny] of direcoes) {
      if (
        nx >= 0 &&
        ny >= 0 &&
        nx < tamanhoLinhas &&
        ny < tamanhoColunas &&
        (labirinto[nx][ny] === "x" || labirinto[nx][ny] === "s")
      ) {
        labirinto[x][y] = ".";
        caminhosPossiveis.push([nx, ny]);
      }
    }

    if (caminhosPossiveis.length > 1) {
      for (let i = 1; i < caminhosPossiveis.length; i++) {
        criarNovaThreadParaCaminho(
          labirinto,
          tamanhoLinhas,
          tamanhoColunas,
          caminhosPossiveis[i]
        );
      }
      posicaoAtual.push(caminhosPossiveis[0]);
    } else if (caminhosPossiveis.length === 1) {
      posicaoAtual.push(caminhosPossiveis[0]);
    }
  }

  console.log("Nenhum caminho para a saída foi encontrado.");
  return false;
}

function criarNovaThreadParaCaminho(
  labirinto,
  tamanhoLinhas,
  tamanhoColunas,
  inicio
) {
  const novoLabirinto = labirinto.map((linha) => [...linha]); // Faz uma cópia do labirinto
  const worker = new Worker(__filename, {
    workerData: {
      labirinto: novoLabirinto,
      tamanhoLinhas,
      tamanhoColunas,
      inicio,
    },
  });

  worker.on("message", (msg) => console.log(msg));
  worker.on("error", (err) => console.error(err));
  worker.on("exit", (code) => {
    if (code !== 0) {
      console.error(`Thread finalizada com código: ${code}`);
    }
  });
}

if (isMainThread) {
  // Função principal
  async function main() {
    const informacoesMatrizLabirinto = carregaArquivo("../data/maze2.txt");

    await printMatriz(informacoesMatrizLabirinto.labirinto);

    console.log("Resolvendo labirinto com multithreads...\n");

    await pesquisaSaida(informacoesMatrizLabirinto);
  }

  (async () => {
    await main();
  })();
} else {
  // Código executado nas threads
  const { labirinto, tamanhoLinhas, tamanhoColunas, inicio } = workerData;
  (async () => {
    await pesquisaSaida({
      labirinto,
      tamanhoLinhas,
      tamanhoColunas,
      entradaLabirinto: inicio,
    });
    parentPort.postMessage("Thread finalizada com sucesso.");
  })();
}
