# Placa: RUSH - Dreamcast (perseguidor)

Placa encomendada na JLC em 15/11/2026. Baseada no EVA-RT (© EXOTIC TEAM MX, AGPL-3.0), pelo mesmo circuito do seguidor RUSH - SATURN.

## Arquivos
| Arquivo | O que é |
|---|---|
| `ProPrj_RUSH - Dreamcast - v4_2026-11-15.epro` | projeto EasyEDA Pro da placa encomendada, com esquemático e PCB |
| `gerber/Gerber_Final_Dreamcast_v4_2026-11-15.zip` | Gerber enviado à JLC |
| `BOM_v4.csv` | lista de componentes tirada da PCB do v4 (`python3 ferramentas/bom_epro.py <epro> <csv>`) |

## Pedido na JLC
- FR-4, 2 camadas, 1,6 mm, 99,8 × 58,4 mm (faixa de até 100 × 100 mm).
- HASL com chumbo, cobre de 1 oz.
- Vias tented, via mínima de 0,3/0,45 mm.
- Remove Mark, teste elétrico.

## Resumo da placa
- **Microcontrolador:** ATmega328P-AU com cristal de 16 MHz.
- **Motores:**
  - 2× DRV8874PWPR em modo PH/EN;
  - IPROPI de 910 Ω com VREF de 2,0 V, o que dá ITRIP ≈ 4,9 A.
- **Ventoinha:** AO3400A com 1N5819 em paralelo com o motor.
- **Alimentação:** LiPo 2S pelo XT30, 78M05, C1/C2 eletrolíticos SMD de 220 µF/16 V.
- **Conectores:**
  - J2, barra de sensores: 1×8 de 2,54 mm;
  - J3, gravação: 5 pads SMD embaixo, com +5V, GND, RX, TX e DTR;
  - J4, Bluetooth: barra fêmea 1×6 na ordem do HC-05;
  - J5, ICSP: 6 pads SMD embaixo;
  - J6, receptor IR KY-022: fêmea 1×3, com o sinal no pad quadrado, +5V no do meio e GND no outro. O sinal vai para o A5.
- Motores, ventoinha, J1 a J5 e R20 estão nas mesmas posições do Saturn.

## Conferência (15/11/2026)
- As 156 vias do `.epro` v4 batem com o arquivo de furação do Gerber final.
- O Gerber final é igual ao conferido antes do pedido.

## Depois da montagem
- **Sentido dos motores, testar com o robô no chão:**
  - se andar para trás, inverter os dois motores;
  - se girar no lugar, inverter um deles;
  - se a ventoinha soprar em vez de sugar, inverter a ventoinha.
- O furo do XT30 tem 2,54 mm e serve para pino de até ~2,3 mm.

## Histórico
| Versão | Data | O que mudou |
|---|---|---|
| v3 | 15/11 | sinais +/− e nomes do J5 na serigrafia (copiados depois para o Saturn) |
| v4 | 15/11 | J6 (KY-022) girado para a pinagem do módulo, sinal rerroteado até o A5; **encomendada** |
