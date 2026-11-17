# Firmware

Adaptado do firmware do seguidor RUSH - SATURN ([`therickmatos/Seguidor-de-linha`](https://github.com/therickmatos/Seguidor-de-linha), `firmware/seguidor/`), que é baseado no [EVA-RT 1.2.2](https://github.com/EXOTIC-TEAM-MX/EVA-RT) (© 2025 EXOTIC TEAM MX, AGPL-3.0).
O controle é o mesmo do seguidor. Saem os sensores de marca e a contagem de partida/chegada; entra o módulo de início por IR, que o regulamento do Perseguidor exige.

| Pasta | Conteúdo |
|---|---|
| `perseguidor/` | firmware do robô (ATmega328P-AU, 16 MHz). Abrir `perseguidor.ino` na IDE do Arduino |
| `testes/` | simulador do controle no PC (`sim.cpp`) e teste dos decodificadores IR (`teste_ir.cpp`) |

**Estado: compila sem avisos e passa no simulador e no teste do IR; ainda não rodou no robô.** Os ganhos e o freio só se acertam na pista.

## Regulamento usado
Perseguidor de Linha da RoboCore, revisão de 12/12/2023 (o TRIF usa as regras da RoboCore no seguidor; conferir no edital se vale o mesmo no perseguidor). O que pesa no firmware:
- **3.1.1 e 6.4:** o robô liga e para pelo módulo de partida do juiz, sem contato e **sem tempo de segurança**.
- **6.5:** robô que não para com o comando de parada é **eliminado**.
- **8.2.3:** vale receptor IR próprio, desde que siga o padrão dos módulos recomendados (JSumo MicroStart, Já-Bots).
- **4.8–4.10:** não há marcas de curva nem cruzamentos; as únicas faixas atravessadas são as duas linhas de partida (200 mm, em reta, com 250 mm de reta antes e depois).
- **6.3:** partida de 2 min; o juiz pode estender.

## Gravar
IDE do Arduino → abrir `perseguidor/perseguidor.ino` → placa "Arduino Nano" (ATmega328P; "Old Bootloader" se o chip veio de um Nano antigo) ou "Arduino Uno", conforme o chip doador → gravar pelo J3 (5 pads embaixo). Serial e Bluetooth em 115200. Ocupa 20,8 kB de flash e 1015 B de RAM.

## Uso
| Situação | LEDs | Controle do juiz | Botão 1 | Botão 2 |
|---|---|---|---|---|
| Ligar com um botão apertado | piscam 2× | — | teste das rodas | liga/desliga a ventoinha |
| Esperando | C pisca devagar | programar grava a arena | calibrar | usar a calibração gravada |
| Calibrando | C pisca rápido | — | terminar | — |
| Pronto | A = linha à esquerda, B = à direita, A+B = no centro; C = linha vista | **partida: sai na hora**; programar grava a arena | sai em 1 s, **só para teste** (segurar: calibrar de novo) | sobe um nível (segurar: nível 0) |
| Correndo | A/B = lado da linha; C = acima da velocidade de curva | **parada: freia e volta sozinho para "pronto"** | desliga | desliga |
| Fim sem o comando de parada | C piscando = perdeu a linha; os três piscando = botão ou tempo | parada → "pronto"; partida → sai de novo | volta para "pronto" | volta para "pronto" |

- **Calibração:** passar a barra inteira pela linha. É rejeitada se faltar contraste em algum sensor (LED C pisca). Fica gravada na EEPROM: nas partidas seguintes basta o botão 2.
- **Nível:** cada toque soma 5 no PWM das rodas e 14 no da ventoinha, como no EVA-RT.
- **Largada:** pelo controle, as rodas saem no primeiro ciclo depois do comando, em rampa (`acel`), e a ventoinha sobe junto em 200 ms. O robô tem de estar com a barra sobre a linha (se não vir a linha, nem sai). Em cima da própria linha de partida também serve.
- **Rounds:** depois do comando de parada o robô volta sozinho para "pronto", com a mesma calibração e o mesmo nível: os rounds seguidos não precisam de botão (regra 6.13, sem manutenção entre rounds).

## Módulo de início (IR)
O KY-022 (J6, no A5) só demodula o infravermelho; quem decodifica é o firmware (`ir.*`), por interrupção. Para a competição vale o padrão do startmodule.com, que é o dos módulos recomendados (protocolo RC5):

| Comando | RC5: endereço | RC5: comando | O que o robô faz |
|---|---|---|---|
| Programar | 0x0B (11) | número da arena (par) | grava o número na EEPROM e pisca os 3 LEDs 3× |
| Partida | 0x07 (7) | arena + 1 | sai na hora (só em "pronto" ou "fim") |
| Parada | 0x07 (7) | arena | freia (`freio`, `tfreio`) e desliga; com o robô parado, volta para "pronto" |

- O número da arena começa em 0 (partida = comando 1, parada = comando 0) até o juiz programar. O console mostra o número atual (`?`) e permite gravar outro (`arena=n`).
- Quadro que não é para este robô (outra arena, outro aparelho): o console mostra e o LED C pisca 2×.
- Repetição do mesmo aperto (o controle repete o quadro a cada 114 ms) é ignorada.
- `IR_PARADA_QUALQUER_ID` em `config.h`: 0 (padrão) para só com o número gravado, como o módulo de verdade; 1 para com a parada de qualquer arena (mais seguro contra a regra 6.5, mas o juiz de uma arena vizinha também para o robô).
- O KY-022 é de 38 kHz e o RC5 usa 36 kHz: funciona, com um pouco menos de alcance.
- **Conferir no briefing** qual controle o juiz usa. Se não for RC5 no padrão acima, o comando `ir` mostra o que chega e o mapeamento muda no `config.h`.

### Testar com o controle que vem com o KY-022
Esse controle usa outro protocolo (NEC) e não manda os comandos do juiz. O firmware também decodifica NEC, mas só aceita quando ligado no console:

```
nec=1
salvar
```

- Partida e parada ficam nas **duas primeiras teclas da linha de cima**: "1" e "2" no controle de 17 teclas (setas e OK), "CH−" e "CH" no de 21 teclas (o "Car MP3"). Os dois mandam endereço 0 e comandos 0x45 (69) e 0x46 (70).
- Se for outro controle ou outra tecla: `ir`, apertar a tecla, ver o `NEC end` e o `cmd`, e acertar `necend`, `necpart` e `necpara`.
- Ligado, o `?` e a tela de "pronto" avisam. **Desligar antes de competir** (`nec=0`, `salvar`): o mesmo controle é comum, e o de outra equipe por perto largaria ou pararia o robô.
- Repetição de tecla segurada é ignorada. Programar a arena só pelo RC5.

## Conferir na bancada, nesta ordem
1. **Canais da barra:** console `r`. Passando a mão, devem variar 12 dos 16 canais; conferir com o `MAPA_MUX` do `config.h` (o teste de bancada da barra usou 4–15, o Gerber da V1 indica 2–13; mesma pendência do seguidor).
2. **Rodas:** ligar com um botão apertado, botão 1. A sequência é esquerda para a frente, esquerda para trás, direita para a frente, direita para trás (LED A = esquerda, LED B = direita, LED C = para trás). Se não bater, mudar `INVERTER_MOTOR_*` / `TROCAR_MOTORES` no `config.h` ou inverter os fios.
3. **Lado da barra:** calibrar e usar o console `s`. Com a linha sob a ponta **esquerda** do robô, quem acende é a primeira coluna e o erro é **negativo**; em "pronto" acende só o LED A. Se estiver ao contrário, `BARRA_ESPELHADA 1`.
4. **IR:** console `ir` e apertar os botões do controle. Cada quadro aparece com o protocolo (RC5 ou NEC), endereço, comando e o que significa. Com o controle do kit: `nec=1` (seção acima). Para testar o RC5 do juiz sem o módulo: controle remoto Philips, outro Arduino com a biblioteca IRremote mandando RC5, ou o controle do JSumo/Já-Bots.
5. **Parada:** em "pronto", com o robô suspenso (rodas no ar), mandar partida e parada pelo controle. Depois, no chão.
6. **Bateria:** console `?` mostra a tensão. Ajustar `batcal` até bater com o multímetro (`batcal` novo = `batcal` × tensão real / tensão mostrada).
7. Primeira partida no nível 0, com a mão perto do robô.

## Console (serial ou Bluetooth, só com o robô parado)
`?` estado · `p` lista · `nome=valor` muda · `salvar` grava na EEPROM · `padrao` valores de fábrica · `arena=n` grava o número da arena · `ir` quadros IR (RC5 e NEC) ao vivo · `s` sensores ao vivo · `r` canais brutos e pino do IR · `res` resumo da última partida · `log` últimos 2,4 s em CSV · `motores` teste das rodas

| Parâmetro | Padrão | O que é |
|---|---|---|
| `kp`, `kd` | 0,06 / 0,40 | PD. Erro em milésimos de sensor (1000 = 7,8 mm); `kd` por ms |
| `fd` | 50 | filtro da derivada (% da leitura nova; 100 = sem filtro, como no EVA-RT) |
| `vel` | 40 | PWM nas curvas fechadas, nível 0 |
| `reta` | 10 | PWM a mais nas retas (0 = velocidade única, como no EVA-RT) |
| `passo`, `vent`, `vpasso`, `nivel` | 5 / 180 / 14 / 0 | aumento por nível, ventoinha, nível ao ligar |
| `ecurva` | 600 | erro que indica entrada de curva: volta para `vel` na hora |
| `treta` | 60 | ms com o erro quieto para poder passar de `vel` |
| `rreta`, `rcurva` | 15 / 40 | curvatura (diferença entre as rodas em % do PWM): até `rreta` anda em `vel + reta`, de `rcurva` em diante em `vel`, no meio proporcional |
| `acel` | 300 | PWM por segundo ao acelerar (também é a rampa da largada) |
| `fcurva`, `tfcurva` | 100 / 40 | freada ao baixar a velocidade: % da queda aplicada a mais, por uns `tfcurva` ms |
| `re` | 255 | ré máxima da roda de dentro |
| `perdido` | 100 | ms sem ver a linha até desligar (EVA-RT: 10) |
| `tmax` | 180 | s até desligar sozinho (0 = sem limite). A partida tem 2 min mais o acréscimo do juiz |
| `freio`, `tfreio` | 100 / 75 | ré no comando de parada: % do PWM em que vinha, por `tfreio` ms (EVA-RT: −SPEED por 75 ms) |
| `nec`, `necend`, `necpart`, `necpara` | 0 / 0 / 69 / 70 | controle NEC do kit, só para teste: liga, endereço, tecla de partida, tecla de parada |
| `comp`, `vref`, `batcal`, `batmin` | 1 / 7600 / 588 / 7000 | compensação do PWM pela bateria; tensão de referência (mV); ajuste do divisor; aviso de bateria baixa |

Saíram em relação ao seguidor: `cruz`, `nmarcas`, `tminvolta` (eram dos sensores de marca).

## Ajuste na pista
1. `reta=0`, nível 0: acertar `kp`/`kd` até seguir sem balançar (balança rápido → menos `kp` ou mais `kd`; demora a entrar na curva → mais `kp`).
2. **Parada:** mandar parada pelo controle numa reta. Se o robô voltar para trás, diminuir `tfreio`; se demorar a parar, aumentar. Conferir de novo ao subir de nível. No perseguidor, voltar para trás é pior que no seguidor: o adversário pode vir logo atrás.
3. Subir o nível (botão 2) até o limite das curvas fechadas.
4. Subir `reta` aos poucos. O resumo (`res`) mostra a % do tempo acima da velocidade de curva; se ficar perto de 0, aumentar `ecurva`.
5. `salvar`.

O resumo também conta as linhas de partida que a barra cruzou: são 2 por volta, então dá o número de voltas da partida.

## O que muda em relação ao seguidor
| Seguidor RUSH - SATURN | Perseguidor | Por quê |
|---|---|---|
| 12 sensores da barra + 2 de marca (A5, A6) | só a barra; A5 é o receptor IR, A6 sem uso | placa Dreamcast, sem as abas |
| larga pelo botão 1, sai em 1 s | larga pelo controle, sai na hora (o botão ficou só para teste) | regra 3.1.1 |
| para na 2ª marca da direita (partida, chegada) | para pelo comando de parada do controle (com a mesma ré curta) | não há marcas; regra 6.4 |
| ventoinha sobe em 400 ms antes de sair | rodas e ventoinha saem juntas; ventoinha em 200 ms | sem tempo de segurança |
| fim: espera um botão | parada pelo juiz: volta sozinho para "pronto" | rounds seguidos sem tocar no robô |
| LEDs A/B = sensores de marca | LEDs A/B = lado da linha na barra | ajuda a posicionar o robô |
| faixa atravessada = cruzamento, descartada nas marcas | faixa atravessada = linha de partida, só segura o erro | regra 4.8 |
| `tmax` 120 s | 180 s | partida de 2 min + acréscimo |
| EEPROM: parâmetros e calibração | + número da arena; magias diferentes das do seguidor | módulo de início |
| — | controle NEC do kit do KY-022 opcional (`nec=1`) | testar na bancada |

Igual ao seguidor: ciclo fixo de 1 ms em ponto fixo, posição pelo grupo de sensores em cima da linha, velocidade de reta e de curva com freada na entrada de curva, roda que satura tira das duas, compensação pela bateria, linha perdida continua virando para o lado em que saiu, console e EEPROM, mapa de pinos do EVA-RT (conferido na netlist do `.epro` do Dreamcast v4).

## Testes no PC (`testes/`)
```
cd firmware/testes
g++ -O2 -std=c++11 -I../perseguidor sim.cpp ../perseguidor/linha.cpp ../perseguidor/corrida.cpp -o sim && ./sim
g++ -O2 -std=c++11 -I../perseguidor teste_ir.cpp ../perseguidor/ir.cpp -o teste_ir && ./teste_ir
```
Os dois terminam com "tudo certo" ou listam o que falhou.

**`sim.cpp`** roda `linha.cpp` e `corrida.cpp` (os mesmos arquivos do robô) com um robô simulado: barra em arco a 107,1 mm do eixo (63,14 mm entre furos, como na `mecânica/`). Pistas:
- ensaio aberto: largada pelo controle e pelo botão, parada pelo juiz, curvas R100–R300, linha de partida reta e a 85°, níveis 4–12, largada torta, linha interrompida, ganho baixo, largada fora da linha, `tmax`, parada antes de sair;
- circuito de perseguidor **inventado** dentro do regulamento (7,44 m, simétrico, 2 linhas de partida, cabe em 2,5 × 1,5 m): 3 voltas largando com a barra sobre a linha, 40 mm antes e 30 mm depois, e da outra linha de partida; três modelos de motor; níveis 4–12; sensores desiguais;
- as pistas TRIF 2025 e 2024 do seguidor, sem as marcas, como teste mais difícil (cruzamentos de verdade e curvas de R100).

No circuito, a velocidade de reta (`reta=10`) baixa o tempo de 3 voltas em 7 a 12 %, conforme o motor. O modelo dos motores é inventado (velocidade proporcional ao PWM, atraso de primeira ordem, sem escorregar): serve para conferir a lógica, **não** para escolher ganhos nem prever tempo de volta. Com o motor mais rápido (60 ms), a ré padrão faz o robô voltar ~75 mm depois de parar; com o mais lento (200 ms), ele anda ~270 mm depois do comando. O simulador só avisa: isso é o ajuste de `tfreio` no robô.

**`teste_ir.cpp`** gera a saída do receptor para quadros RC5 e NEC, com a distorção do receptor (−200 a +300 µs nos pulsos), o atraso da interrupção e a resolução de 4 µs do `micros()`, e passa as bordas pelos dois decodificadores, como no robô. RC5: todos os endereços e comandos (6144 quadros), botão segurado, volta do `micros()`, quadro com pico de ruído (pode se perder, mas nunca vira outro comando) e quadro cortado. NEC: 4608 quadros, endereço estendido, tecla segurada (as repetições são ignoradas) e comando com o inverso errado (descartado). RC5 e NEC alternados não se atrapalham. Em 2 × 10 min de pulsos aleatórios apareceu 1 quadro válido e nenhuma partida, parada ou programação, mesmo com o NEC ligado.

**Tempo de ciclo** medido no `simavr` (ATmega328P, 16 MHz): leitura dos 12 sensores 320 µs + controle 247 µs (máx. 300) + saídas 22 µs = 590 µs em média, 640 µs no pior caso. Cada borda do IR passa pelos dois decodificadores em 21–28 µs (no máximo 2 bordas por ciclo). O console `res` mostra o maior ciclo e quantos passaram de 1 ms no robô.

## Pendências
- Tudo acima precisa ser confirmado no robô: mapa do mux, sentido das rodas, lado da barra, IR, ganhos, freio.
- O protocolo do controle do juiz (RC5 no padrão startmodule.com) vem do regulamento; não foi testado com um módulo de verdade.
- A barra fica ~107 mm à frente do eixo. Em curva de R100 ela fica quase paralela à linha e vê uma faixa larga; o firmware trata isso, mas a leitura da posição perde resolução ali. Com a barra mais perto do eixo o efeito diminui (é só deslizar o patim na vareta).
- PWM dos motores em 976 Hz, como no EVA-RT e no seguidor.
- Partida por Bluetooth (regra 8.3, só se não der para usar o IR): não implementada.
