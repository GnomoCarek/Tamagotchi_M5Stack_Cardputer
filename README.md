# 👾 CardPet Tamagotchi – M5Stack Cardputer Edition

![ESP32-S3](https://img.shields.io/badge/MCU-ESP32--S3-blue?style=flat-square&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange?style=flat-square&logo=platformio)
![Framework](https://img.shields.io/badge/Framework-Arduino-green?style=flat-square&logo=arduino)
![M5Cardputer](https://img.shields.io/badge/Hardware-M5Stack%20Cardputer-red?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-purple?style=flat-square)

Um jogo de bichinho virtual (**Tamagotchi**) moderno, completo e intuitivo, desenvolvido em **C++** com **PlatformIO** especialmente otimizado para o **M5Stack Cardputer**.

---

## 🌟 Destaques & Recursos

- 🐣 **Novo Fluxo de Inicialização**: Seleção de sexo (♂ Macho / ♀ Fêmea), digitação de nome (3 a 12 caracteres) e animação do nascimento do ovo chocando.
- 🌐 **Sincronização Wi-Fi & Hora Real (NTP)**: Conexão Wi-Fi com fuso horário oficial de Brasília (UTC-3). Ajuste de tempo real via internet e suporte offline via RTC interno do ESP32-S3.
- 🌅 **Ciclo Dia/Noite baseado em Horário Real**:
  - `06:00 – 11:59`: ☀️ Manhã (Céu azul claro, sol suave, nuvens)
  - `12:00 – 17:59`: 🌤 Tarde (Céu azul celeste, sol radiante)
  - `18:00 – 18:59`: 🌅 Pôr do Sol (Céu alaranjado/rosado, iluminação suave)
  - `19:00 – 05:59`: 🌙 Noite (Céu noturno escuro, lua, estrelas piscantes)
- ⚙️ **Modo de Desenvolvimento (Dev Mode)**: Ativável nas Configurações. Acelera a passagem do tempo de modo que **1 dia completo = 30 minutos reais** (multiplicador 48x), facilitando testes de evolução, ciclo dia/noite e doenças.
- 🎒 **Inventário & Loja por Categorias**: Organização em 6 abas intuitivas: *Alimentos, Medicamentos, Brinquedos, Higiene, Limpeza e Itens Especiais*.
- 🎮 **2 Minijogos Integrados**:
  1. 🌟 **Capturar Estrelas**: Arcade de esquiva e coleta de moedas.
  2. ⚡ **Teste de Reflexo**: Minijogo arcade de reflexo rápido no teclado do Cardputer com combos e pontuação progressiva!
- 🎨 **Interface Pixel Art 30 FPS**: Duplo buffering (`M5Canvas`) sem cintilação (*flickering*).
- 🔊 **Sintetizador de Áudio Customizado**: Efeitos sonoros retro para cada ação (comer, brincar, dormir, doente, cocô, banho, acertos, erros e boot).
- 💾 **Salva Automático NVS (Preferences)**: Armazenamento persistente de atributos, sexo, nome, relógio, credenciais Wi-Fi, inventário e conquistas.

---

## 🎮 Controles no M5Stack Cardputer

| Tecla / Comando | Função / Ação |
| :--- | :--- |
| **Setas `;` `,` `.` `/`** ou **`W` `A` `S` `D`** | Navegar entre opções, mover cursor, alternar categorias e mover o cesto no minijogo |
| **`ENTER` / `ESPAÇO` / Botão `G0`** | Confirmar seleção, usar item, interagir e selecionar |
| **`DEL` / `BACKSPACE`** | Apagar caractere digitado na entrada de texto ou Wi-Fi |
| **`TAB` / Tecla `Q` / `ESC`** | Voltar ao menu anterior / Fechar menu |
| **Teclas Alfanuméricas (`A-Z`, `0-9`)** | Digitação do nome do pet, credenciais Wi-Fi e resposta ao Teste de Reflexo |
| **Tecla `1`** | Atalho rápido durante o jogo: Fazer Carinho ❤️ |
| **Tecla `2`** | Atalho rápido durante o jogo: Limpar Cocô 🧹 |
| **Tecla `3`** | Atalho rápido durante o jogo: Dar Banho 🧼 |

---

## 📖 Como Jogar - Manual do Usuário

### 1. Criando o seu Pet
Ao iniciar o jogo pela primeira vez (ou selecionar **Novo Jogo** no menu inicial):
1. **Escolha o Sexo**: Selecione entre **♂ Macho** e **♀ Fêmea** utilizando as setas e pressione `ENTER`. O sexo é salvo permanentemente e exibido na tela de status.
2. **Digite o Nome**: Utilize o teclado do Cardputer para digitar um nome de **3 a 12 caracteres**. Pressione `DEL` para apagar e `ENTER` para confirmar.
3. **Nascimento do Ovo**: Assista à animação do ovo balançando e chocando ao som de comemoração!

### 2. Cuidados Diários do Pet
Mantenha a saúde e a felicidade do seu bichinho altas monitorando as estatísticas:
- 🍎 **Alimentação**: No menu principal, escolha **Alimentar** para abrir o inventário de alimentos (Maçã, Carne, Pão, Bolo, Leite, Hambúrguer, Salada). Consumir alimentos satisfaz a fome e ganha peso.
- 🧹 **Higiene e Limpeza**: Quando o pet fizer cocô ou ficar sujo, use a tecla de atalho `2` (Limpar) ou `3` (Banho), ou compre sabão/vassoura no inventário.
- ❤️ **Carinho**: Pressione a tecla `1` ou `ESPAÇO` para dar carinho no bichinho, aumentando seu afeto e felicidade.
- 💊 **Saúde & Medicamentos**: Se o pet ficar doente (devido ao acúmulo de sujeira ou fome), use o item **Xarope** ou **Injeção** para curá-lo antes que a saúde zere!
- 💤 **Sono**: Selecione **Dormir** no menu para fazer o pet descansar e recuperar energia.

### 3. Loja e Inventário Categorizado
Acesse a **Loja** ou o **Inventário** no menu principal:
- Use as setas **Esquerda/Direita** para alternar entre as 6 abas de categoria:
  - 🍎 **Alimentos**: Maçãs, Carnes, Pães, Bolos, Leites, etc.
  - 💊 **Medicamentos**: Vitaminas, Xaropes e Injeções.
  - ⚽ **Brinquedos**: Bolas e GameBoys para aumentar a felicidade.
  - 🧼 **Higiene**: Sabão e Shampoo para dar banho.
  - 🧹 **Limpeza**: Vassouras e Esponjas para manter o quarto limpo.
  - 🪴 **Especiais**: Decorações (Plantas, Tapetes, Quadros).

### 4. Minijogos & Ganhar Moedas
No menu, selecione **Brincar** para escolher um dos 2 minijogos:
1. 🌟 **Capturar Estrelas**: Mova o cesto com as setas para pegar estrelas douradas e evitar bombas.
2. ⚡ **Teste de Reflexo**: Uma tecla aparece em destaque na tela (ex: `TECLA A`, `TECLA 1`, `ENTER`, `ESPAÇO`). Pressione a tecla correspondente no teclado físico do Cardputer antes que a barra de tempo acabe! Acertos seguidos geram combos de moedas!

### 5. Configurações & Sincronização Wi-Fi (NTP)
No menu de **Configurações**:
- **Ajustes da Tela e Som**: Altere volume, brilho da tela e tempo de desligamento automático.
- **Configurar Wi-Fi**: Informe o SSID (nome da rede) e a senha utilizando o teclado do Cardputer. O jogo conectará automaticamente à internet e sincronizará o relógio oficial de Brasília via NTP!
- **Modo Dev (Desenvolvimento)**: Alterne o Modo Dev para `LIGADO` se desejar acelerar a simulação (1 dia virtual passa em 30 minutos reais), ideal para testar evoluções e ciclos de dia/noite.
- **Resetar Save**: Apaga o progresso atual para reiniciar o jogo.

---

## 📁 Estrutura do Repositório

```text
.
├── include/              # Arquivos de cabeçalho C++ (.h)
│   ├── Config.h          # Configurações globais, sexo, enums e paleta de cores
│   ├── Pet.h             # Lógica do bichinho virtual, sexo e atributos
│   ├── Clock.h           # Relógio NTP Wi-Fi, ciclo dia/noite e Modo Dev
│   ├── Animation.h       # Sprites em pixel art, expressões e partículas
│   ├── Input.h           # Leitura do teclado e botão G0 do Cardputer
│   ├── Sound.h           # Sintetizador de áudio e efeitos sonoros
│   ├── Storage.h         # Persistência NVS (Preferences), Wi-Fi e tempo offline
│   ├── Items.h           # Catálogo e categorias da Loja e Inventário
│   ├── Achievements.h    # Sistema de Conquistas e Troféus
│   ├── MiniGame.h        # Minijogos: Capturar Estrelas e Teste de Reflexo
│   ├── Events.h          # Eventos aleatórios e popups de notificação
│   ├── Menu.h            # Máquina de estados da interface, submenus e abas
│   ├── Renderer.h        # Renderizador 16-bit com M5Canvas
│   └── Game.h            # Coordenador principal do jogo
├── src/                  # Código fonte C++ (.cpp)
│   ├── main.cpp          # Ponto de entrada (setup e loop)
│   └── ...
├── platformio.ini        # Configurações de compilação do PlatformIO
├── Tamagotchi_Cardputer.bin # Firmware pré-compilado para MicroSD / M5Launcher
├── ajustes.md            # Especificação de melhorias do projeto
├── LICENSE               # Licença MIT
└── README.md             # Documentação oficial
```

---

## ⚡ Como Rodar no Cardputer

### Opção 1: Via Cartão MicroSD (Cardputer ADV / M5Launcher)
1. Baixe o arquivo pré-compilado **[`Tamagotchi_Cardputer.bin`](./Tamagotchi_Cardputer.bin)**.
2. Copie para o seu cartão MicroSD.
3. Insira no Cardputer, abra o **M5Launcher / Cardputer ADV** e execute o arquivo!

### Opção 2: Compilando via PlatformIO (VS Code)
1. Clone este repositório:
   ```bash
   git clone https://github.com/SeuUsuario/Tamagotchi_M5Stack_Cardputer.git
   ```
2. Abra a pasta no **VS Code** com a extensão **PlatformIO** instalada.
3. Conecte o Cardputer via USB-C.
4. Clique no ícone de **Upload** (`➔`) na barra inferior do PlatformIO.

---

## 📜 Licença

Este projeto é distribuído sob a licença [MIT](./LICENSE). Sinta-se à vontade para contribuir, modificar e compartilhar!
