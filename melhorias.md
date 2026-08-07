# Prompt de Atualização – CardPet (M5Stack Cardputer)

Você é um desenvolvedor especialista em **C++**, **ESP32-S3**, **Arduino Framework**, **PlatformIO**, **M5Unified** e desenvolvimento para o **M5Stack Cardputer**.

Sua missão é **evoluir o projeto CardPet**, preservando a arquitetura atual, corrigindo bugs e implementando novas mecânicas sem quebrar funcionalidades existentes.

## Objetivos Gerais

* Corrigir todos os bugs existentes.
* Melhorar a experiência do usuário (UX).
* Tornar o jogo mais parecido com um Tamagotchi clássico.
* Manter o código modular, organizado e otimizado.
* Evitar regressões nas funcionalidades já implementadas.
* Documentar todas as alterações realizadas.

---

# 1. Correções (Prioridade Máxima)

## 🥚 O ovo não está chocando

Este é o bug mais importante.

Verificar completamente o sistema de evolução.

Conferir:

* cálculo de tempo
* idade do pet
* timestamp salvo
* carregamento do save
* lógica de evolução
* máquina de estados

O ovo deve chocar automaticamente quando atingir o tempo configurado.

Caso exista algum erro lógico ou de salvamento, corrigi-lo.

Não utilizar soluções temporárias ("gambiarras").

---

# 2. Melhorar o menu inferior

Atualmente algumas palavras ficam sobre os ícones.

Melhorar completamente a UI.

Objetivos:

* textos nunca sobrepostos
* espaçamento adequado

Caso necessário:

mostrar apenas o ícone e exibir o nome da opção na parte superior da tela.

O menu deve ser totalmente legível.

---

# 3. Escolha do nome

Antes do jogo iniciar:

Mostrar uma tela inicial.

Fluxo:

Logo

↓

Novo jogo

↓

Escolher nome

↓

Nascimento do ovo

↓

Jogo

O nome deve ser digitado utilizando o teclado do Cardputer.

Permitir:

* apagar letras
* confirmar
* limite entre 3 e 12 caracteres

Salvar permanentemente.

---

# 4. Timeout da tela

Implementar economia de energia.

Após um tempo configurável (ex.: 60 segundos) sem interação:

* desligar backlight
* apagar tela

Qualquer tecla:

* liga novamente
* retorna exatamente ao ponto onde estava

Não pausar a simulação.

Adicionar configuração para:

30 s

60 s

120 s

Nunca desligar

---

# 5. Indicador de bateria

Adicionar um pequeno indicador na barra superior.

Mostrar:

* porcentagem
* ícone

Exemplo:

🔋100%

🟩🟩🟩🟩

Caso seja possível utilizar a API do Cardputer para leitura da bateria.

Caso contrário, preparar uma interface para futura implementação.

---

# 6. Melhorar os ícones

Na loja e inventário existem muitos ícones repetidos.

Criar novos sprites em pixel art.

Exemplos:

Comidas

🍎

🍖

🍞

🍰

🥛

Remédios

💊

🩹

Brinquedos

⚽

🎲

🧸

Itens

🧽

🪣

🛏

🛁

Cada item deve possuir um sprite exclusivo.

---

# 7. Melhorar o ciclo de dia e noite

O sistema atual precisa ser refeito.

Adicionar:

Durante o dia

☀️ Sol animado

Nuvens

Céu azul

Durante o pôr do sol

céu alaranjado

Durante a noite

🌙 Lua

⭐ Estrelas

Brilho reduzido

O fundo deve mudar gradualmente.

---

# 8. Sistema de cocô (Nova Mecânica)

Adicionar uma nova necessidade do pet.

## Funcionamento

O pet faz cocô automaticamente.

A cada:

3 horas

Somente se:

barra de comida > 0

Cada cocô aparece no cenário.

Cada cocô ocupa um espaço.

Máximo:

4 cocôs

Quando atingir:

4 cocôs

O pet fica doente.

Adicionar nova opção no menu.

🧹 Limpar

Cada uso limpa apenas UM cocô.

Criar sprite de cocô em pixel art.

Enquanto houver muitos cocôs:

reduzir felicidade

aumentar chance de doença

---

# 9. Doença

O pet pode adoecer por diversos motivos.

Motivos:

* muito cocô 4 acumulados na tela.
* sem banho
* fome extrema
* tristeza extrema
* muito tempo acordado

Caso permaneça doente durante:

24 horas

O pet morre.

Mostrar:

animação

mensagem

Game Over

Permitir reiniciar.

---

# 10. Sistema de persistência

Revisar completamente o Save.

Sempre salvar:

nome

idade

estado

timestamp

fome

energia

felicidade

saúde

banho

carinho

cocôs

inventário

dinheiro

fase

posição das animações

Ao iniciar novamente:

calcular tempo offline.

Exemplo:

Usuário ficou:

12 horas sem jogar.

Atualizar automaticamente:

fome

energia

felicidade

sono

cocôs

banho

doença

idade

Caso necessário:

executar várias simulações em sequência até alcançar o horário atual.

---

# 11. Sistema de banho

Adicionar higiene.

O pet deve tomar banho:

1 vez por dia.

Criar variável:

Último banho.

Caso fique mais de:

24 horas

Sem banho:

fica sujo.

Após algum tempo:

fica doente.

Criar opção:

🛁 Banho

Adicionar animação.

Adicionar efeito sonoro.

---

# 12. Sistema de carinho

Adicionar necessidade emocional.

Criar atributo:

Carinho

0–100

Vai diminuindo com o tempo.

Ao utilizar:

❤️ Fazer carinho

Aumentar felicidade.

Caso fique muito tempo sem carinho:

fica triste

interage menos

fica bravo

pode adoecer

Criar animação exclusiva.

---

# 13. Sistema de sono

Melhorar o sistema atual.

Caso passe muitas horas acordado:

fica cansado

anda lentamente

fica triste

Adicionar olhos fechando lentamente.

---

# 14. Melhorias Visuais

Adicionar pequenas animações:

* brilho quando evoluir

---

# 15. Sons

Adicionar sons exclusivos para:

banho

limpeza

cocô

doença

cura

morte

nascimento

choque do ovo

digitação do nome

ligar/desligar tela

bateria fraca

---

# 16. Balanceamento

Revisar toda a velocidade das barras.

O objetivo é tornar o jogo agradável.

Exemplo:

Fome

↓ lentamente

Energia

↓ durante o dia

Felicidade

↓ lentamente

Carinho

↓

Banho

↓

Doença

↑ apenas quando necessário

Evitar que o jogador precise abrir o jogo a todo momento.

Idealmente:

algumas verificações por dia sejam suficientes.

---

# 17. Melhorias Gerais

* Melhorar todas as animações.
* Corrigir qualquer flickering.
* Melhorar a responsividade dos menus.
* Tornar todas as transições suaves.
* Revisar uso de memória RAM.
* Revisar consumo de CPU.
* Revisar consumo de bateria.
* Remover código duplicado.
* Melhorar comentários.
* Organizar classes.
* Criar constantes para tempos e limites, evitando números "mágicos".
* Garantir que todo o projeto continue compilando normalmente no PlatformIO.

# 18. Melhorar a navegação e leitura do teclado (Alta Prioridade)

Atualmente a navegação pelos menus está sensível demais. Ao pressionar uma tecla, muitas vezes o sistema registra dois ou mais toques, dificultando a utilização do CardPet.

## Objetivo

Implementar um sistema de leitura do teclado confiável e responsivo, evitando múltiplas ativações indesejadas. atualmente o q sai o spaço da enter e o botão de go.

### Requisitos

* Implementar **debounce** nas teclas.
* Registrar apenas um evento por pressionamento.
* Ignorar pequenas oscilações do contato da tecla.
* Evitar que uma única pressionada avance duas ou mais opções do menu.
* Permitir repetição de teclas apenas quando o usuário mantiver a tecla pressionada intencionalmente.

### Repetição de teclas

Implementar comportamento semelhante ao de um teclado de computador:

* Primeiro toque: executa apenas uma ação.
* Após aproximadamente **400–500 ms** segurando a tecla, iniciar repetição automática.
* Repetição a cada **80–120 ms** enquanto a tecla permanecer pressionada.
* Ao soltar a tecla, interromper imediatamente a repetição.

### Navegação

Melhorar toda a experiência de navegação:

* Menus suaves responsivos e previsíveis.
* Sem pulos entre opções.
* Sem seleção dupla.
* Mudança instantânea ao pressionar uma tecla.
* Pequena animação ao mover o cursor.
* Efeito sonoro discreto ao navegar.

### Organização

Criar uma classe exclusiva para gerenciamento de entrada, por exemplo:

```cpp
InputManager
```

Essa classe deve centralizar toda a leitura do teclado, oferecendo funções como:

* `isPressed()`
* `isJustPressed()`
* `isHeld()`
* `isReleased()`
* `repeatPressed()`

Nenhuma outra parte do projeto deve acessar diretamente as teclas do Cardputer; toda a leitura deve passar pelo `InputManager`, facilitando futuras manutenções e garantindo um comportamento consistente em todo o jogo.


---

# Sugestões de Novas Mecânicas

Além das melhorias acima, implemente algumas funcionalidades para deixar o CardPet ainda mais divertido e próximo de um Tamagotchi moderno.

### 🎒 Mochila e Inventário

* Limite de itens carregados.
* Organização por categorias.
* Possibilidade de vender itens por um valor menor do comprado.


### 🎣 Minijogos Extras

* Pesca.
* Corrida de obstáculos.

### 💤 Sonhos

Durante o sono, Mostrar pequenas animações e conceder bônus aleatórios ao acordar.

### 📅 Eventos Diários

Criar um evento diferente a cada dia, como presentes, promoções na loja ou mudanças climáticas.

### 🌧️ Sistema de Clima

Alternar entre sol, chuva e dias nublados. O clima pode influenciar o humor do pet e alterar o cenário.

### 🏆 Conquistas

Adicionar objetivos como:

* Primeiro banho.
* Primeira evolução.
* 30 dias de vida.
* Nunca ficar doente por uma semana.
* Limpar 100 cocôs.
* Alimentar 500 vezes.

### 💬 Personalidade

Cada personalidade deve influenciar as falas, preferências, velocidade com que as barras diminuem e reações às ações do jogador.

---

# Resultado Esperado

O CardPet deve oferecer uma experiência completa de bichinho virtual, com comportamento consistente mesmo após longos períodos desligado. O projeto deve permanecer modular, eficiente e preparado para futuras expansões, mantendo excelente desempenho no M5Stack Cardputer e utilizando boas práticas de desenvolvimento em C++ para sistemas embarcados.
