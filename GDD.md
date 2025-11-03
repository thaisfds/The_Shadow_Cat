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

![](media/image1.png){width="6.5in" height="2.2916666666666665in"}

As fases do jogo são um conjunto de salas, e acaba quando o jogador mata
o chefe. A estrutura das salas é feita à mão, mas os inimigos,
obstáculos e outros elementos são colocados aleatoriamente.

O reino está em conflito e o jogador passa cada fase em uma região
diferente do reino, batalhando diferentes gatos. Os chefes das fases são
os bípedes, gatos de maior status que conseguem andar em duas patas.

O jogador começa com 7 vidas, perdendo uma vida a cada ataque, morrendo
quando as vidas se esgotarem (finalizando a *run*). Itens e alguns
elementos no mapa são capazes de recuperar as vidas do jogador, mas são
um recurso mais escasso.

A progressão do personagem acontece por meio de itens e habilidades.
Inimigos podem deixar itens no chão quando morrem, esses itens podem ser
ativos (efeitos diversos quando utilizados) ou passivos (melhoria de
atributos do jogador). Habilidades podem ser aprimoradas de diversas
maneiras, a depender do tipo (uma habilidade de atirar um projétil pode
ter o alcance melhorado ou aumentar a velocidade do projétil, por
exemplo).

### 2.2. Controles & Heads-up Display (HUD)

Esse jogo é desenvolvido para PC, e por isso os controles são baseados
nos jogos clássicos. O seguinte mapeamento será utilizado para teclado e
controle de Xbox, respectivamente:

**Interação:** F / Y (Xbox)

**Movimentação:**

-   **Cima:** W / Joystick para cima

-   **Baixo:** S / Joystick para baixo

-   **Esquerda:** A / Joystick para esquerda

-   **Direita:** D / Joystick para direita

**Ataque básico:** Botão esquerdo / X (Xbox)

**Ataque carregado:** Botão esquerdo segurado / X (Xbox)

**Ataque especial:** Botão direito / B (Xbox)

**Dodge:** Shift / RT (Xbox)

**Pause:** Esc / Start (Xbox)


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

![](media/image3.jpg){width="2.09375in"
height="2.7333333333333334in"}![](media/image5.png){width="2.0in"
height="2.7333333333333334in"}![](media/image4.png){width="2.0619477252843397in"
height="2.7290485564304463in"}

A ambientação das fases representa a segregação entre os povos felinos e
como isso afetou o equilíbrio do reino. As regiões são:

**Floresta:** lar dos gatos pretos, um ambiente mítico e conectado à luz
da lua

**Cidade:** o centro do poder felino, onde os os gatos laranjas dominam
com orgulho e rigidez

**Arredores:** território dos gatos livres, uma zona de transição
habitada por gatos brancos, tricolores e frajolas.

![](media/image2.jpg){width="1.1166272965879265in"
height="1.0290485564304461in"}

O protagonista será um jovem e pequeno gato preto, ágil e curioso, que
parte em uma jornada para desafiar a tirania e restaurar o equilíbrio
entre os reinos feridos, a comando de sua rainha. Sua aparência e
animações refletem leveza, determinação e um toque de inocência.

As artes serão formadas de uma mistura entre as disponíveis em *open
source* e desenvolvimento próprio através do Aseprite[^4].


## 4. Música e Efeitos Sonoros

A música do jogo é inspirada em jogos clássicos 16-bit, tendo
características e instrumentação simples, mas com qualidades modernas,
como a usada em *Enter The Gungeon*[^5]*,* por exemplo. A ambientação
desejada para o jogador é usar uma melodia com batidas suaves e ritmo
lento, para transmitir o aspecto fofo do jogo em cenários tranquilos e
exposição de *lore*. Em batalhas intensas, a melodia se torna mais
errática com um ritmo acelerado, representando a intensidade da batalha
e o lado feroz dos felinos.

De maneira similar, os efeitos sonoros vão ter ritmo e melodia alinhados
com o item ou ação em questão, sendo tranquilos para ações inofensivas,
e agressivos para ações violentas, como infligir ou receber dano.

## 5. Tecnologias

### 5.1. Game Engine

O jogo será implementado em C++, utilizando a biblioteca SDL para
gerenciamento de janela, I/O, sons e gráficos. Adicionalmente, a
biblioteca GLEW vai ser utilizada para compilar shaders OpenGL no
formato .glsl, permitindo a criação de um renderizador customizado para
implementação dos efeitos e visuais requeridos.

Com essas tecnologias, o jogo será desenvolvido para duas plataformas:
Windows e Linux.

### 5.2. Áudio e Arte

Por questões de tempo, a trilha sonora e quaisquer efeitos sonoros serão
provenientes de fontes *copyright free* e sintetizadores gratuitos como
Bfxr[^6]. Se necessário, *remixes* e edições leves podem ser feitas com
o Audacity[^7].

A arte do jogo será criada no Aseprite, como mencionado em seções
anteriores. Os sprites de personagens vão ter leves animações, o
background será estático e animações podem ser adicionadas ao longo do
andamento do projeto se possível.

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
