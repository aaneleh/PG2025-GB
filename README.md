# 🌸 Jogo Isométrico – Coletando Flores

## Objetivo
O jogador controla um personagem que se move por um mundo isométrico representado por tiles. O objetivo principal é coletar todas as flores espalhadas pelo mapa, evitando obstáculos como água e lava. Ao pisar em determinadas áreas, o terreno pode mudar de cor **(troca de tile).** Quando o jogador coletar todas as flores, ele vence o jogo.

## TODO
- ✅ 'Tabuleiro' de tilemap isometrico
- ✅ Personagem deve se mover com um sprite animado
- ✅ Personagem controlável para as 8 direções (sempre para o centro dos tiles)
- ✅ Lógica de tiles caminháveis e não-caminhável

#### Arquivo de configuração
- Codigicar o nome do arquivo de tiles, num de tiles, height, width, largura (15), altura (15), todo tile map, tiles caminháveis X não-caminháveis
- Mapa 15x15

#### Definição de tiles, itens e objetivo
- Sistema de coleta e evitação de itens
- Mecanismo de troca de tile (quando personagem pisa em cima de um tile, altera ele ou outros)
- Objetivo para vencer o jogo (coletar x itens ou alcançar tile)

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
