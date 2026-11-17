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
- Regulamento: TRIF 2026, que anexa o Perseguidor de Linha da RoboCore (12/12/2023) na íntegra (`referencias/TRIF 2026 - Perseguidor de Linha.pdf`): largada e parada pelo módulo de partida, sem contato e sem tempo de segurança; receptor IR próprio só se seguir o padrão dos módulos JSumo/Já-Bots (8.2.3); bluetooth só se não der (8.3); sem marcas de curva; duas linhas de partida de 200 mm em retas.

## Arquivos
O `README.md` da raiz tem a tabela "Onde encontrar" por tarefa. Manter atualizada quando algo mudar de lugar.
Branch principal: `main`. Os branches antigos e o que cada um tem estão listados em `historico/README.md`.

| Pasta | Conteúdo |
|---|---|
| `hardware/placa_base/` | `.epro` do v4 (encomendado), Gerber final, BOM |
| `hardware/barra_v1/` | Gerber da barra V1 (a mesma do seguidor) |
| `firmware/perseguidor/` | firmware (Arduino, ATmega328P), adaptado de `firmware/seguidor/` do repositório do seguidor |
| `firmware/testes/` | `sim.cpp` (controle no PC) e `teste_ir.cpp` (decodificadores RC5 e NEC) |
| `mecânica/` | STL para imprimir (`mecânica/stl/PETG/` e `mecânica/stl/PLA/`, por filamento), guia de montagem, figuras e relatório da conferência 3D |
| `ferramentas/bom_epro.py` | BOM a partir do `.epro` |
| `ferramentas/pecas_3d/` | gera `mecânica/stl/` e faz a conferência 3D (lê o `.epro` e o Gerber da V1) |
| `referencias/` | regulamento do TRIF 2026 e a parte do EVA-RT que as ferramentas leem |
| `historico/` | branches antigos no GitHub e o que cada um tem |

## Firmware (17/11/2026)
- Mesmo controle do seguidor (EVA-RT 1.2.2, mesmo mapa de pinos, conferido na netlist do `.epro`); sem sensores de marca; A5 = receptor IR por interrupção (PCINT13), A6 sem uso.
- **Controle:** no TRIF o controle fica com o participante e o NEC do kit do KY-022 foi aceito em 2024 e 2025 (Erick, 17/11/2026). O edital não cita protocolo. O NEC é o principal e vem ligado de fábrica (`p.nec = 1`, teclas 0x45/0x46, ajustáveis no console). Risco: no TRIF 2024/25 a maioria das equipes usava esse mesmo controle (endereço 0); recomendado ao Erick usar outro controle NEC com outro endereço (só configurar `necend`/`necpart`/`necpara`), ou teclas menos óbvias.
- Também lê RC5 no padrão do startmodule.com (programar 0x0B, partida/parada 0x07, comando = arena + 1 / arena), conferido no código oficial (github.com/p1rse/robot-sumo-start-module). Sony (JSumo MicroStart) não é decodificado.
- Escrito e conferido só por simulação e compilação com `avr-gcc`; **ainda não rodou no robô**.
- `linha.cpp`, `corrida.cpp` e `ir.cpp` não dependem do Arduino e são os mesmos no robô e nos testes: depois de mexer neles, rodar `sim` e `teste_ir` (os dois têm de terminar com "tudo certo").
- O ciclo de controle é de 1 ms e o ATmega gasta ~0,6 ms nele (simavr): nada de `float` nem divisão de 32 bits dentro de `Corrida::passo()`.

## Mecânica (17/11/2026)
- Base de **1,6 mm** (confirmado pelo Erick) e barra V1 de **1,6 mm** (medida pelo Erick). Guia em `mecânica/README.md`, na ordem de uso; saída da conferência em `mecânica/conferencia/relatorio.txt`.
- STL gerados por `ferramentas/pecas_3d/gerar_pecas.py`; conferência 3D com o `.epro` do v4 em `ferramentas/pecas_3d/conferir_montagem.py`. Do seguidor vieram o patim, o assento da presilha, a placa de sucção com rebaixo e a separação PLA/PETG. A presilha sem braço de baixo é daqui (o seguidor usa a mesma atrás).
- Peças adaptadas:
  - presilha sem braço de baixo (com braço, a cabeça do parafuso ficaria abaixo da pista), a mesma nas 4 posições, com a parede na borda x = ±50 (entra 0,05 mm, como no EVA-RT) e assento: o furo da vareta é fechado embaixo, por ser impressa em PLA/PETG. `presilha(..., assento=False)` continua reconstruindo o `barilla_clamp` original;
  - patim da V1 para barra de 1,6 mm (igual ao do seguidor);
  - placa de sucção com rebaixo de Ø4,4 para as cabeças dos pés (0,6 mm da pista). O fundo de 0,2 mm isola as vias PWMA_D5/PWMB_D6/GND dos pés de trás: não trocar por furo passante com parafuso de aço.
- Sem asa: o bloco da transversal é o `barilla_connection` como está (não encosta na placa).
- O Erick tem só PLA e PETG (sem filamento com fibra de carbono). PETG em tudo, menos aro, apoio da bateria e bucha (PLA). Quem decide a pasta é o conjunto `PLA` em `gerar_pecas.py`.
- Barra a 63,14 mm entre furos (L = 107,1 mm), ajustável na vareta. Barra a 3,4 mm da pista; cabeças de parafuso a 0,6–0,8 mm (pneu de ~22 mm, não medido).
- J3 (gravação, embaixo): barra de pinos reta deitada, parte preta encostada na borda de trás (y = −30), ~0,8 mm da pista.
- Conferência: `gerar_pecas.py` tem de dar "reconstrucao confere"; no `conferir_montagem.py` a única interferência aceita é a da parede da presilha na borda (0,05 mm).
- Nada foi impresso nem testado fisicamente.

## Formato .epro (para os scripts)
- É um zip de linhas JSON, com coordenadas em mil (× 0,0254 = mm).
- VIA: r[3] rede, r[5]/r[6] posição, r[7] furo, r[8] diâmetro do anel.
- Serigrafia STRING: camada 3 = cima, 4 = baixo.

## Cuidados
- Manter o crédito e a licença AGPL-3.0 do EVA-RT em qualquer arquivo derivado.
- Nos `.md`, nenhum link ou caminho entre crases pode apontar para algo que não existe.
- Não decidir pelo Erick.
