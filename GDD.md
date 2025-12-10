# UNIVERSIDADE FEDERAL DE MINAS GERAIS

FILIPE MENDES (2021031920), GABRIEL PAINS (2021096887), JUAN BRAGA (2021423489),  THAÍS FERREIRA (2021092571)


## 1. Declaração Artística

Este documento apresenta o design do *The Shadow Cat*, um *action
roguelike* de gatinhos, com um visual fofo e sistema de progressão de
itens e habilidades para o jogador. O foco aqui não é ter um jogo muito
inovador, reinventando o gênero, mas sim fazer bem o gênero para os
jogadores já experientes: uma experiência que desafia o jogador através
de mecânicas de combate, incentivando o uso criativo dos sistemas e
habilidades para derrotar inimigos. E tudo isso, é claro, aliado à
punição de recomeçar o jogo a cada morte.

A emoção principal a ser explorada no jogo é entregar uma sensação de
que é possível derrotar os inimigos através do aprimoramento das
habilidades, mas sem tirar a tensão e o risco constante de perder o
progresso a cada morte. O jogador deve sentir que, mesmo diante de
derrotas repetidas, está aprendendo padrões, dominando mecânicas e se
tornando mais habilidoso.


## 2. Gameplay

Da primeira vez que o jogo é iniciado, o jogador é mandado ao tutorial,
para entender as mecânicas principais do jogo. Nas próximas, o jogador
começa sempre na primeira fase, como em vários roguelikes, e vai
progredindo até a última fase em uma *run*. A *run* acaba quando o
jogador morre, recomeçando da primeira fase, com a oportunidade de
conseguir itens e habilidades diferentes na próxima (nenhuma run é igual
à outra).

As fases do jogo são um conjunto de salas, e acaba quando o jogador mata
o chefe. A estrutura das salas é feita à mão, mas os inimigos,
obstáculos e outros elementos são colocados aleatoriamente.

O reino está em conflito e o jogador passa cada fase em uma região
diferente do reino, batalhando diferentes gatos. Os chefes das fases são
os bípedes, gatos de maior status que conseguem andar em duas patas.

O jogador possui um sistema de pontos de vida (HP), perdendo HP ao
receber ataques. Quando o HP chega a zero, o jogador morre e a *run*
se encerra. O HUD exibe a vida do jogador através de uma barra visual.

A progressão do personagem acontece principalmente através do sistema de
habilidades. O jogador possui cinco habilidades principais:

-   **Ataque Básico (Basic Attack):** Ataque corpo-a-corpo rápido

-   **Claw Attack:** Ataque especial corpo-a-corpo mais poderoso

-   **Furball:** Projétil de longo alcance

-   **Stomp:** Ataque de área ao redor do jogador

-   **Dash:** Movimento rápido evasivo

Habilidades podem ser aprimoradas de diversas maneiras através de
upgrades. Por exemplo, uma habilidade de projétil pode ter o alcance,
dano, velocidade ou cooldown melhorados.

### 2.2. Controles & Heads-up Display (HUD)

Esse jogo é desenvolvido para PC, utilizando teclado e mouse. O seguinte mapeamento de controles foi implementado:

**Movimentação:**

-   **Cima:** W

-   **Baixo:** S

-   **Esquerda:** A

-   **Direita:** D

**Ataque básico:** Botão esquerdo do mouse (LMB)

**Ataque especial (Claw Attack):** Botão direito do mouse (RMB)

**Habilidades:**

-   **Furball (projétil):** E

-   **Stomp (área):** Q

-   **Dash (movimento rápido):** Shift

**Tutorial:** H (mostrar/esconder controles)

**Debug:** F1 (desenvolvedores)


## 3. Arte

O estilo artístico de *The Shadow Cat* segue uma estética 2D top-down em
pixel art, com foco em cores suaves e ambientes visualmente
aconchegantes, em contraste com o tema central da história: a guerra
entre os felinos. Essa diferença busca criar um mundo encantado e mágico
onde cada ambiente reflita um pouco da história e da identidade de cada
grupo de gatos.

Visualmente o jogo se inspira na mistura entre fofo e sombrio de *Cult
Of The Lamb*[^1], na pixel art viva e colorida de *Stardew Valley*[^2],
e na ambientação marcada pelo uso expressivo de luz e sombra de *Hollow
Knight*[^3].

A ambientação das fases representa a segregação entre os povos felinos e
como isso afetou o equilíbrio do reino. As regiões são:

**Floresta:** lar dos gatos pretos, um ambiente mítico e conectado à luz
da lua

**Cidade:** o centro do poder felino, onde os os gatos laranjas dominam
com orgulho e rigidez

**Arredores:** território dos gatos livres, uma zona de transição
habitada por gatos brancos, tricolores e frajolas.

O protagonista será um jovem e pequeno gato preto, ágil e curioso, que
parte em uma jornada para desafiar a tirania e restaurar o equilíbrio
entre os reinos feridos, a comando de sua rainha. Sua aparência e
animações refletem leveza, determinação e um toque de inocência.

As artes serão formadas de uma mistura entre as disponíveis em *open
source* e desenvolvimento próprio através do Aseprite[^4].


## 4. Música e Efeitos Sonoros

O jogo possui um sistema de áudio completo implementado com SDL_mixer,
contando com músicas dinâmicas e efeitos sonoros contextuais.

### 4.1. Músicas

Cada cena do jogo possui trilha sonora exclusiva:

-   **Menu Principal:** "Sweet Dreams" (8bit Doves) - melodia relaxante

-   **Lobby/Tutorial:** "House" (Zelda: Ocarina of Time) - tema acolhedor

-   **Level 1:** "Kokiri Forest" (Zelda: Ocarina of Time) - ambiente tranquilo

-   **Level 2:** "Opening Scene" (Zelda: Majora's Mask) - transição para intensidade

-   **Level 3:** "A Bitter Cold" (Return of the Obra Dinn) - atmosfera tensa

-   **Boss 1:** "Nightmare" (8bit Doves) - batalha intensa

-   **Boss 2:** "Dinosaur Boss Battle" (Zelda: Ocarina of Time) - combate épico

-   **Boss 3:** "Boss Battle" (Zelda: Ocarina of Time) - confronto final

-   **Vitória:** "8-bit Game" (Pixabay) - celebração

-   **Game Over:** "Boss Clear" (Zelda: Ocarina of Time) - encerramento

### 4.2. Efeitos Sonoros

Os efeitos sonoros são contextuais e incluem:

-   **Habilidades:** Cada habilidade (Basic Attack, Claw Attack, Stomp,
    Furball, Dash) possui sons únicos criados com bfxr.net

-   **Passos:** Sons de passos variam conforme o terreno (grama, tijolo,
    pedra), com alternância para maior realismo

-   **Ambiente:** Som de transição ao mudar de sala

-   **Interface:** Sons de menu e interações (Zelda: Breath of the Wild)

Todos os volumes foram balanceados individualmente para garantir uma
experiência auditiva agradável.

## 5. Tecnologias

### 5.1. Game Engine

O jogo foi implementado em C++, utilizando as seguintes bibliotecas:

-   **SDL2:** Gerenciamento de janela, input e renderização base

-   **SDL_mixer:** Sistema de áudio (músicas e efeitos sonoros)

-   **SDL_image:** Carregamento de texturas

-   **SDL_ttf:** Renderização de fontes

-   **OpenGL com GLEW:** Compilação de shaders customizados (.glsl) para
    efeitos visuais avançados

Com essas tecnologias, o jogo foi desenvolvido para duas plataformas:
Windows e Linux.

### 5.2. Áudio e Arte

A trilha sonora utiliza músicas de *The Legend of Zelda: Ocarina of Time*,
*Majora's Mask*, *Return of the Obra Dinn* e *8bit Doves*. Os efeitos sonoros foram criados com bfxr.net[^6] e
alguns provenientes de *The Legend of Zelda: Breath of the Wild*.

A arte do jogo foi criada no Aseprite[^4], como mencionado em seções
anteriores. Os sprites de personagens possuem animações suaves para
movimento, ataques e habilidades. Os backgrounds e tilesets são estáticos
com alguns elementos animados.

### 5.3. Declaração de Uso de IA

Para ajudar na geração de ideias relacionadas a história do mundo e bons
nomes relacionados aos ambientes e personagens, fizemos a utilização do
*ChatGPT*. Ele forneceu ideias para o ambiente político de guerra ,
nomes para ambientes e reinos. Com base nesse *brainstorm* fomos capazes
de definir melhor o que iríamos fazer, e em qual direção nossa história
iria seguir.

Fora essa utilização o *ChatGPT* não foi mais utilizado.


## 6. Marketing

O público alvo do jogo são jogadores que gostam de desafios crescentes e
levemente punitivos, mas que recompensam habilidade e aprendizado, ou
seja, jogadores que gostam de ação e *roguelikes*. Ainda, como o jogo
tem um apelo artístico fofo, o público alvo também inclui jogadores que
gostam de temas relacionados à gatos e do contraste entre os visuais
fofos e a ação explosiva, assim como em *Cult of the Lamb,* mencionado
anteriormente*.*


## 7. Cronograma

O cronograma tentativo para o desenvolvimento do jogo está descrito na
tabela abaixo:

  ---------------------------------------------------------------------
  **Semana**     **Objetivo**
  -------------- ------------------------------------------------------
  29/09 - 12/10  Game Design Document

  14/10 - 19/10  Criação dos assets gráficos básicos, além de músicas e
                 efeitos sonoros

  20/10 - 26/10  Começo da prototipação

  27/10 - 02/11  Cont. prototipação

  03/11 - 09/11  Cont. prototipação

  10/11 - 16/11  Playtest interno, correção de bugs e implementações
                 finais de detalhes de gameplay

  17/11 - 23/11  Melhoria de assets gráficos e detalhes extras de
                 gameplay

  24/11 - 30/11  Playtest, QA e polimento final

  01/12 - 07/12  Apresentação e entrega final
  ---------------------------------------------------------------------

Devido à natureza do desenvolvimento de jogos, muitas etapas podem ser
sobrepostas ou executadas em ordens levemente diferentes. Por exemplo,
efeitos sonoros podem ser mudados nos testes de gameplay, o mesmo se
aplica à arte.

[^1]: [[https://www.cultofthelamb.com/]{.underline}](https://www.cultofthelamb.com/)

[^2]: [[https://www.stardewvalley.net/]{.underline}](https://www.stardewvalley.net/)

[^3]: [[https://www.hollowknight.com/]{.underline}](https://www.hollowknight.com/)

[^4]: [[https://www.aseprite.org/]{.underline}](https://www.aseprite.org/)

[^5]: [[https://enterthegungeon.com/]{.underline}](https://enterthegungeon.com/)

[^6]: [[https://www.bfxr.net/]{.underline}](https://www.bfxr.net/)

[^7]: [[https://www.audacityteam.org/]{.underline}](https://www.audacityteam.org/)
