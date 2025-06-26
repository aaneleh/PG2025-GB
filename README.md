# 🍄 Jogo Isométrico – Coletando Cogumelos

## Objetivo
**coletar todos os itens espalhados pelo mapa e, depois, alcançar o tile de objetivo para vencer**.  
O cenário é um tabuleiro isométrico com obstáculos, tiles caminháveis, água, lava e diferentes tipos de terreno. Ao pisar em determinadas áreas, o terreno pode mudar de cor **(troca de tile).** Quando o jogador coletar todas os cogumelos, ele vence o jogo.

- **Só é possível caminhar em tiles considerados "caminháveis" (definidos no arquivo de configuração).**
- **Alguns tiles mudam de estado ao serem pisados (por exemplo, pedra vira lava).**
- **O jogo termina quando todos os itens são coletados e o personagem alcança o tile objetivo.**

## Como funciona a lógica do jogo

1. **Leitura do arquivo de configuração (`tilemap.txt`)**  
   O jogo carrega todas as informações sobre tiles, mapa, itens e regras a partir desse arquivo.  
   Isso inclui:
   - Nome do tileset, dimensões dos tiles e do mapa
   - Matriz do mapa (tipo de cada tile)
   - Quais tiles são caminháveis
   - Imagem e posições dos itens coletáveis

2. **Movimentação:**
   O Personagem se movimenta em 8 direções, controlado tanto pelas teclas de seta quanto pelas teclas WASD.
   
   **Movimentação Diagonal:** Use as teclas de seta para se mover nas direções diagonais:

* **`↑` (Seta para Cima):** Move o personagem para **Noroeste** (para cima e para a esquerda).
* **`→` (Seta para Direita):** Move o personagem para **Nordeste** (para cima e para a direita).
* **`↓` (Seta para Baixo):** Move o personagem para **Sudeste** (para baixo e para a direita).
* **`←` (Seta para Esquerda):** Move o personagem para **Sudoeste** (para baixo e para a esquerda).

  **Movimentação Reta (Teclas WASD)**
  Use as teclas WASD para se mover nas direções retas (Norte, Sul, Leste, Oeste):

* **`W`:** Move o personagem para **Norte** (para cima).
* **`S`:** Move o personagem para **Sul** (para baixo).
* **`A`:** Move o personagem para **Oeste** (para a esquerda).
* **`D`:** Move o personagem para **Leste** (para a direita).

3. **Coleta de itens:**  
   Ao entrar em um tile que contém um item, esse item é removido do mapa e incrementa o contador de itens coletados.  
   O item some visualmente, e o console exibe a mensagem de coleta.

4. **Troca de tile:**  
   Certos tiles mudam seu tipo ao serem pisados (por exemplo, pedra pode virar lava).  
   Essa lógica é definida no código e pode ser configurada para alterar apenas o tile atual ou outros ao redor.

5. **Objetivo de vitória:**  
   Após coletar todos os itens, o jogador deve alcançar o tile objetivo (definido no mapa) para vencer.  
   O jogo exibe uma mensagem de vitória e é encerrado.

## Exemplo de configuração (`tilemap.txt`)

```
TileSet.png 5 57 114
15 15
1 0 0 0 0 0 0 0 3 3 3 3 3 3 3
0 3 0 0 0 3 0 0 1 1 3 3 3 3 3
0 0 0 0 0 0 0 0 0 0 1 1 3 3 3
0 0 0 0 4 4 4 4 0 0 0 1 1 1 3
0 4 4 4 4 4 4 4 0 0 0 0 0 1 1
4 4 4 4 4 4 4 4 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
4 4 4 4 4 4 4 2 2 2 2 2 2 2 2
4 4 4 4 4 4 4 4 2 2 2 2 2 2 9
0 1 2 -1
cogumelo.png
0 0 
1 1 
2 2
3 3
```

- As linhas acima informam:
  - **Tileset:** nome, quantidade de tipos e dimensões
  - **Tamanho do mapa:** (15 x 15)
  - **Mapa:** matriz de tipos de tiles
  - **Tiles caminháveis:** 0, 1, 2
  - **Arquivo de item:** `cogumelo.png`
  - **Posições dos itens:** (0,0), (1,1), (2,2), (3,3)

---

## Complementando a lógica | 🍄 Jogo Isométrico – Coletando Cogumelos
 
  Basta alterar o mapa, adicionar/remover itens, mudar os tiles caminháveis ou até o tileset, sem tocar no código.
- **A lógica de movimentação impede sair do mapa ou andar em obstáculo.**
- **A coleta, troca de tile e objetivo são processados a cada movimento:**  
  Ao mover, o código checa se há item para coletar, se o tile precisa ser trocado, e se venceu.
- **Novos tipos de tile ou mecânicas** podem ser adicionados apenas alterando a matriz e as regras de troca/objetivo no código.

---

Desenvolvido para a disciplina de Processamento Gráfico | GrauB

Alunas: **Isadora Albano, Helena Kurz, Nathaly Loggiovini**

<!---
## Lógica do Jogo

### Mapa e Tiles
- O mapa deve ser composto por uma matriz `15x15` carregada do arquivo `map.txt`.
- Cada valor numérico representa um tipo de **tile**:
  - `0` – Grama (caminhável)
  - `1` – Água (não caminhável)
  - `2` – Flor (coletável)
  - `3` – Lava (não caminhável / morte)
  - `4` – Terra batida (pode mudar de aparência ao pisar)

### Tipos de tiles:
| Número | Tile         | Caminhável | Ação especial          |
|--------|--------------|------------|------------------------|
| 0      | Grama        |    Sim     | —                      |
| 1      | Água         |    Não     | Bloqueia movimento     |
| 2      | Flor         |    Sim     | Coleta item            |
| 3      | Lava         |    Não     | Reinicia personagem    |
| 4      | Terra batida |    Sim     | Muda de cor ao pisar   |
-->
