## 🚀 Projeto: Controle de LEDs e Interrupções no RP2040  

### 📌 Descrição  
Este projeto implementa o controle de LEDs e botões utilizando interrupções no **microcontrolador RP2040**, com a placa **BitDogLab**. O projeto inclui uma **matriz 5x5 de LEDs WS2812**, um **LED RGB** e dois **botões**, aplicando conceitos como **debouncing via software** e **interrupções (IRQ)** para capturar eventos dos botões.  

### 🎯 Objetivos  
- Compreender o funcionamento e a aplicação de **interrupções** em microcontroladores.  
- Implementar **debouncing via software** para evitar leituras incorretas dos botões.  
- Manipular e controlar **LEDs comuns e LEDs WS2812**.  
- Explorar o uso de **resistores de pull-up internos**.  
- Desenvolver um **sistema funcional** combinando hardware e software.  

### 🛠 Componentes Utilizados  
- **Microcontrolador RP2040** (Placa BitDogLab)  
- **Matriz 5x5 de LEDs WS2812** (GPIO 7)  
- **LED RGB** (GPIOs 11, 12, 13)  
- **Botão A** (GPIO 5)  
- **Botão B** (GPIO 6)  

### 🔧 Funcionalidades  
1. O **LED vermelho** do **LED RGB** pisca **5 vezes por segundo**.  
2. O **Botão A** incrementa o número exibido na **matriz de LEDs**.  
3. O **Botão B** decrementa o número exibido na **matriz de LEDs**.  
4. A **matriz WS2812** exibe números de **0 a 9**

### 📽 Demonstração  
[Clique aqui para conferir a demonstração do projeto](https://youtu.be/5amEdhSQfOc)

### 🚀 Como Executar  
1. Clone este repositório:  
   ```bash
   git clone https://github.com/KaianS/U4C4OT12
   ```
2. Acesse a pasta do projeto:  
   ```bash
   cd U4C4OT12
   ```
3. Compile e execute o código

📌 **Obs:** Certifique-se de que sua placa está conectada e configurada corretamente.  

