# Prompt – Desenvolvimento de um Tamagotchi para M5Stack Cardputer

Você é um desenvolvedor especialista em sistemas embarcados, Arduino, ESP32, C++, PlatformIO e dispositivos da M5Stack.

Sua tarefa é criar um projeto completo de um **Tamagotchi (bichinho virtual)** otimizado para o **M5Stack Cardputer**, utilizando **PlatformIO** no **Visual Studio Code**.

## Objetivo

Desenvolver um jogo de bichinho virtual, bonito e responsivo, aproveitando todos os recursos disponíveis do Cardputer.

---

# Ambiente de desenvolvimento

* IDE: Visual Studio Code
* Extensão: PlatformIO
* Linguagem: C++
* Framework: Arduino
* Hardware: M5Stack Cardputer
* Microcontrolador: ESP32-S3
* Biblioteca principal:

  * M5Unified
* Utilizar somente bibliotecas compatíveis com PlatformIO.

Organize corretamente os arquivos:

```
src/
include/
lib/
assets/
platformio.ini
README.md
```

---

# Interface

Criar uma interface inspirada nos Tamagotchis clássicos, porém moderna.

A tela deve conter:

* Sprite do personagem centralizado
* Nome do bichinho
* Relógio interno
* Dia do jogo
* Ícones simples desenhados em pixel art
status com:
* Barra de vida
* Barra de fome
* Barra de felicidade
* Barra de energia

A interface deve atualizar sem cintilação (flickering).

Utilizar Sprite Buffer quando possível.

---

# Personagem

Criar um personagem em pixel art de aproximadamente:

```
48x48 pixels
```

ou

```
64x64 pixels
```

Com animações.

Exemplo:

Idle

(^_^)

Piscar

(-_-)

Dormindo

(-.-) zZ

Feliz

(≧▽≦)

Triste

(T_T)

Doente

(@_@)

Bravo

(>_<)

Morto

(X_X)

As animações devem alternar automaticamente.

---

# Estados do Tamagotchi

Implementar um sistema de atributos.

## Fome

0–100

Diminui lentamente com o tempo.

---

## Felicidade

0–100

Aumenta brincando.

Diminui quando ignorado.

Diminue quando está com fome.

---

## Energia

0–100

Diminui durante o dia.

Recupera dormindo.

---

## Saúde

0–100

Diminui se:

* ficar com fome
* dormir pouco
* ficar triste
* ficar doente

Pode ser recuperada com remédio.

---

## Idade

Contada em dias.

---

## Peso

Opcional.

Pode aumentar ao comer demais.

---

# Ações do jogador

Criar um menu simples navegável pelo teclado.

Opções:

🍖 Alimentar

🎮 Brincar

💊 Dar remédio

💤 Dormir

🛁 Dar banho

❤️ Fazer carinho

📊 Status

💾 Salvar

⚙ Configurações

---

# Sistema de tempo

O tempo deve continuar mesmo após desligar o aparelho.

Salvar timestamp.

Ao ligar novamente:

Calcular quanto tempo passou.

Atualizar automaticamente:

* fome
* energia
* felicidade
* saúde

---

# Sistema de save

Salvar na memória Flash usando:

Preferences

ou

LittleFS

Salvar:

* nome
* idade
* barras
* dinheiro
* itens
* personalidade
* timestamp

Carregar automaticamente ao iniciar.

---

# Sistema de evolução

Após alguns dias o personagem evolui.

Exemplo:

Ovo

↓

Filhote

↓

Juvenil

↓

Adulto

↓

Velho

Cada fase possui sprite próprio.

---

# Sistema de personalidade

O bichinho pode nascer:

* Alegre
* Bravo
* Dorminhoco
* Guloso
* Curioso
* Calmo

Cada personalidade altera o comportamento.

---

# Sons

Utilizar o buzzer interno do Cardputer.

Criar pequenos efeitos sonoros.

Exemplos:

Alimentar

♪ bip bip

Brincar

♪ pipipi

Dormir

♪ zzz zzz zzz

Doente

♪ biiiiip

Cagando

♪ poin poin poin

Evolução

♪ pequena melodia

Morrer

♪ melodia triste

Inicialização

♪ som clássico de Tamagotchi

Criar uma função:

```
playSound(evento)
```

que reproduza automaticamente cada efeito.

---

# Minijogo

Criar um minijogo simples.

Exemplo:

Capturar estrelas.

Objetivo:

Mover um cursor e pegar estrelas que aparecem aleatoriamente.

Recompensa:

Moedas.

---

# Loja

Permitir comprar:

* comida
* remédio
* brinquedos
* decoração

Sistema de moedas.

---

# Sistema de eventos aleatórios

Algumas vezes:

O bichinho:

* fica doente (se não limpar o coco dele)
* encontra dinheiro
* ganha presente
* fica triste
* pede atenção

Mostrar pequenas mensagens.

---

# Sistema de notificações

Exemplos:

"Estou com fome!"

"Vamos brincar?"

"Estou cansado."

"Obrigado!"

"Estou feliz!"

---

# Estatísticas

Mostrar:

Dias vividos

Tempo jogado

Comidas consumidas

Banhos

Brincadeiras

Remédios

Horas dormidas

---

# Configurações

Permitir:

Volume

Brilho

Resetar save

Idioma (PT-BR)

Velocidade do jogo

---

# Organização do código

Separar em classes.

Exemplo:

```
Pet
Game
Renderer
Input
Sound
Storage
Animation
UI
Menu
MiniGame
Items
Events
Clock
```

Cada classe em arquivos separados.

Evitar código gigante no `main.cpp`.

---

# Arquitetura

Utilizar orientação a objetos.

Evitar variáveis globais.

Criar máquinas de estados para:

Menu

Jogo

MiniGames

Dormindo

Tela inicial

Game Over

---

# Performance

Priorizar:

* baixo uso de RAM
* baixo uso de Flash
* animações suaves
* Fluidez
* atualização parcial da tela
* consumo reduzido de CPU

---

# README

Gerar documentação completa contendo:

* Instalação
* Configuração do PlatformIO
* Bibliotecas necessárias
* Compilação
* Upload
* Estrutura do projeto
* Funcionamento
* Controles

---

# platformio.ini

Gerar um arquivo totalmente configurado para o M5Stack Cardputer.

Incluir todas as dependências necessárias.

---

# Qualidade do código

O projeto deve:

* Compilar sem erros.
* Não conter código incompleto.
* Possuir comentários úteis.
* Ser modular.
* Ser fácil de expandir.
* Seguir boas práticas de C++ moderno.

---

# Diferenciais desejados

Se houver recursos disponíveis no Cardputer, implementar também tem um cartão de 16GB nele:

* Sistema de dia e noite com mudança gradual das cores da interface.
* Expressões faciais dinâmicas baseadas no estado do pet.
* Pequenas partículas (corações, estrelas e notas musicais) durante interações.
* Animações suaves usando interpolação quando possível.
* Tela inicial com logotipo em pixel art e animação.
* Música de fundo simples usando o buzzer, com opção para ativar ou desativar.
* Sistema de conquistas (Achievements).
* Inventário de itens.
* Decorações para o ambiente do bichinho.
* Múltiplos pets desbloqueáveis.
* Código preparado para futuras atualizações sem necessidade de grandes refatorações.

O resultado final deve ser um projeto completo, organizado, profissional e pronto para abrir no Visual Studio Code com PlatformIO, proporcionando uma experiência semelhante aos Tamagotchis clássicos, mas adaptada às limitações e recursos do M5Stack Cardputer.
