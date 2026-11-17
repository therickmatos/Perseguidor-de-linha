# CLAUDE.md — contexto para o Claude Code

## Como trabalhar
- Responder em **português**, de forma direta e concisa. O Erick (estudante de eletrônica, IFSP) revisa e corrige o que recebe.
- Antes de afirmar algo sobre a placa, ler `hardware/placa_base/README.md`.
- Unidades em mm. Origem no centro da ventoinha, +y para a frente, o mesmo referencial do seguidor.
- Antes de mudar algo de que o Erick gostou, explicar o que seria alterado. Não decidir pelo Erick.

## O projeto
Perseguidor **RUSH - Dreamcast**. É a placa irmã do seguidor RUSH - SATURN (repositório `therickmatos/Seguidor-de-linha`) e foi encomendada junto com ele na JLC em 15/11/2026.
- O circuito é o do Saturn, sem as abas dos 4 sensores de marca, mais o receptor IR KY-022 (J6) no A5.
- O circuito é baseado no EVA-RT (EXOTIC TEAM MX, AGPL-3.0). Manter o crédito e a licença em qualquer arquivo derivado.
- Fluxo: EasyEDA Pro → Gerber → JLC.
- Regulamento: Perseguidor de Linha da RoboCore (12/12/2023): largada e parada pelo módulo de início do juiz, sem tempo de segurança; sem marcas de curva; duas linhas de partida de 200 mm em retas.

## Arquivos
| Pasta | Conteúdo |
|---|---|
| `hardware/placa_base/` | `.epro` do v4 (encomendado), Gerber final, BOM |
| `hardware/barra_v1/` | Gerber da barra V1 (a mesma do seguidor) |
| `firmware/perseguidor/` | firmware (Arduino, ATmega328P), adaptado de `firmware/seguidor/` do repositório do seguidor |
| `firmware/testes/` | `sim.cpp` (controle no PC) e `teste_ir.cpp` (decodificador RC5) |
| `mecânica/` | STL, parafusos, montagem e conferência 3D |
| `ferramentas/bom_epro.py` | BOM a partir do `.epro` |
| `ferramentas/pecas_3d/` | gera `mecânica/stl/` e faz a conferência 3D (lê o `.epro` e o Gerber da V1) |

## Firmware (17/11/2026)
- Mesmo controle do seguidor (EVA-RT 1.2.2, mesmo mapa de pinos, conferido na netlist do `.epro`); sem sensores de marca; A5 = receptor IR por interrupção (PCINT13), A6 sem uso.
- Módulo de início no padrão do startmodule.com (RC5: programar 0x0B, partida/parada 0x07, comando = arena + 1 / arena). Não testado com módulo de verdade: conferir no briefing.
- Escrito e conferido só por simulação e compilação com `avr-gcc`; **ainda não rodou no robô**.
- `linha.cpp`, `corrida.cpp` e `ir_rc5.cpp` não dependem do Arduino e são os mesmos no robô e nos testes: depois de mexer neles, rodar `sim` e `teste_ir` (os dois têm de terminar com "tudo certo").
- O ciclo de controle é de 1 ms e o ATmega gasta ~0,6 ms nele (simavr): nada de `float` nem divisão de 32 bits dentro de `Corrida::passo()`.

## Mecânica (17/11/2026)
- Base de **1,6 mm** (confirmado pelo Erick). Presilha nova sem o braço de baixo (`presilha_vareta_base1.6.stl`): com braço, a cabeça do parafuso ficaria abaixo da pista.
- Patim, distância de 63,14 mm entre furos (L = 107,1 mm) e demais peças iguais às do seguidor. Espessura da V1 ainda por medir (há patim para 1,0 e 1,6 mm).

## Formato .epro (para os scripts)
- É um zip de linhas JSON, com coordenadas em mil (× 0,0254 = mm).
- Serigrafia STRING: camada 3 = cima, 4 = baixo.
