# 👾 Tamagotchi M5Stack Cardputer Edition

![ESP32-S3](https://img.shields.io/badge/MCU-ESP32--S3-blue?style=flat-square&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange?style=flat-square&logo=platformio)
![Framework](https://img.shields.io/badge/Framework-Arduino-green?style=flat-square&logo=arduino)
![M5Cardputer](https://img.shields.io/badge/Hardware-M5Stack%20Cardputer-red?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-purple?style=flat-square)

Um jogo de bichinho virtual (**Tamagotchi**) moderno e completo, desenvolvido em **C++** com **PlatformIO** especialmente otimizado para o **M5Stack Cardputer**.

---

## 🌟 Destaques & Recursos

- 🎨 **Interface Pixel Art Fluida**: Duplo buffering (`M5Canvas`) a 30 FPS **sem cintilação (flickering)**.
- 🌅 **Ciclo Dia/Noite Dinâmico**: Transição suave de cores do céu (Alvorecer, Dia, Crepúsculo e Noite).
- 🐣 **Evolução em 5 Estágios**: *Ovo ➔ Filhote ➔ Juvenil ➔ Adulto ➔ Senior*.
- 🎭 **Personalidades Únicas**: O bichinho nasce com personalidade própria (*Alegre, Bravo, Dorminhoco, Guloso, Curioso, Calmo*) que impacta o apetite, fadiga e reações.
- 🔊 **Sintetizador de Áudio**: Efeitos sonoros customizados para cada ação (comer, brincar, dormir, doente, cocô, evolução, morte e boot).
- ⏰ **Tempo Offline Real**: Salva o timestamp ao desligar e calcula a passagem do tempo e decaimento dos atributos ao religar.
- 💾 **Salva Automático (NVS Flash)**: Armazenamento persistente de atributos, idade, moedas, inventário e conquistas.
- 🎮 **Minijogo "Capturar Estrelas"**: Arcade integrado para ganhar moedas e melhorar o humor do pet.
- 🏪 **Loja & Inventário**: Compre e use alimentos, remédios, brinquedos e itens decorativos.
- 🏆 **Sistema de Conquistas (Achievements)**: 10 troféus desbloqueáveis com recompensas em moedas.
- 💬 **Notificações Dinâmicas**: Popups animados de aviso ("Estou com fome!", "Vamos brincar?", "Encontrei moedas!").

---

## 🎮 Controles no M5Stack Cardputer

| Tecla / Ação | Função |
| :--- | :--- |
| **Setas / W, A, S, D / ;, ,, ., /** | Mover cursor / Navegar nos Menus e Minijogo |
| **Enter / Espaço / Tecla E / Botão G0** | Confirmar / Selecionar / Interagir |
| **Del / Backspace / Tab / Tecla Q** | Voltar / Fechar Menu |
| **Tecla 1** | Atalho rápido: Carinho ❤️ |
| **Tecla 2** | Atalho rápido: Limpar Cocô 🛁 |

---

## 📁 Estrutura do Repositório

```text
.
├── include/              # Arquivos de cabeçalho C++ (.h)
│   ├── Config.h          # Configurações globais e paleta de cores
│   ├── Pet.h             # Lógica do bichinho virtual e estatísticas
│   ├── Clock.h           # Relógio interno e ciclo dia/noite
│   ├── Animation.h       # Sprites em pixel art, expressões e partículas
│   ├── Input.h           # Leitura do teclado e botão G0 do Cardputer
│   ├── Sound.h           # Sintetizador de áudio e efeitos sonoros
│   ├── Storage.h         # Persistência NVS (Preferences) e tempo offline
│   ├── Items.h           # Catálogo da Loja e Inventário
│   ├── Achievements.h    # Sistema de Conquistas e Troféus
│   ├── MiniGame.h        # Minijogo de capturar estrelas
│   ├── Events.h          # Eventos aleatórios e popups de notificação
│   ├── Menu.h            # Máquina de estados da interface e submenus
│   ├── Renderer.h        # Renderizador 16-bit com M5Canvas
│   └── Game.h            # Coordenador principal do jogo
├── src/                  # Código fonte C++ (.cpp)
│   ├── main.cpp          # Ponto de entrada (setup e loop)
│   └── ...
├── platformio.ini        # Configurações de compilação do PlatformIO
├── Tamagotchi_Cardputer.bin # Firmware pré-compilado para MicroSD / M5Launcher
├── Plano.md              # Requisitos do projeto
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
