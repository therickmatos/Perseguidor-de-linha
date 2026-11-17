/*
 * config.h - configuracao de compilacao do perseguidor (pinos, barra, IR, limiares)
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0
 * https://github.com/EXOTIC-TEAM-MX/EVA-RT
 * Adaptado do firmware do seguidor RUSH - SATURN (therickmatos/Seguidor-de-linha).
 *
 * O que muda de robo para robo ou de pista para pista fica aqui.
 * O que se ajusta entre partidas (ganhos, velocidades) fica em params.h
 * e pode ser alterado pelo console serial, sem regravar.
 */
#pragma once

#define FW_NOME    "perseguidor"
#define FW_VERSAO  "0.1.0"

// ------------------------------------------------------------------ pista
#define LINHA_BRANCA  1          // RoboCore/TRIF: linha branca em manta preta

// ------------------------------------------------------------------ pinos
// Placa RUSH - Dreamcast v4 (conferido na netlist do .epro): o mapa do
// EVA-RT 1.2.2, sem os sensores de marca e com o receptor IR no A5.
// So documentacao: o hw.cpp acessa os registradores direto.
//   D2  PD2  LED A                       D3  PD3  sentido motor A (esq.)
//   D7  PD7  LED B                       D4  PD4  sentido motor B (dir.)
//   D13 PB5  LED C                       D5  PD5  PWM motor A (OC0B)
//   D11 PB3  botao 1                     D6  PD6  PWM motor B (OC0A)
//   D12 PB4  botao 2                     D10 PB2  PWM ventoinha (OC1B)
//   A0..A3 PC0..PC3  S0..S3 do mux       A4  ADC4 saida comum do mux
//   A5  PC5  receptor IR KY-022 (J6), entrada digital, PCINT13
//   A6  sem ligacao                      A7  ADC7 bateria (divisor 20k/10k)
#define ADC_MUX       4
#define ADC_BATERIA   7

// ------------------------------------------------------------------ barra
#define N_SENSORES    12

// Canal do 74HC4067 de cada sensor, do S0 (esquerda do robo, olhando para a
// frente) ao S11 (direita). E o mapa do teste de bancada da barra V1
// (firmware/teste_barra/barra.h do repositorio do seguidor).
//
// ATENCAO: no Gerber de hardware/barra_v1 os sensores estao nos canais 2 a
// 13 (0, 1, 14 e 15 no GND), como na barra do EVA-RT. Se a calibracao
// rejeitar sempre os mesmos sensores da ponta, veja com o comando 'r' quais
// canais respondem e troque para a linha de baixo. (Mesma pendencia do seguidor.)
#define MAPA_MUX  {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
// #define MAPA_MUX  {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}

// 1 = a barra esta montada ao contrario (S0 do mapa ficou na direita)
#define BARRA_ESPELHADA      0

// Na barra V1 o preto da leitura alta (saida no coletor)
#define BARRA_ALTO_NO_PRETO  1

// Espera depois de trocar o canal do mux, antes da conversao (us)
#define ACOMODACAO_US        12

// Limiares na escala normalizada 0..255 (255 = em cima da linha)
#define LIMIAR_RUIDO         13   // abaixo disso o sensor nao entra na media
#define LIMIAR_LINHA         77   // algum sensor acima disso = linha vista

// Contraste minimo (bruto, 8 bits) aceito na calibracao
#define DELTA_MINIMO         40

// A linha de 19 mm cobre 2 a 3 sensores (passo de 7,8 mm), 4 se o robo
// estiver um pouco torto. LARGURA_FAIXA ou mais sensores vizinhos ativos
// e uma faixa larga; LARGURA_CRUZAMENTO ou mais e uma faixa atravessada.
// No perseguidor a unica faixa atravessada e a linha de partida (regra 4.8:
// reta de 200 mm cruzando a pista, com 250 mm de reta antes e depois), que
// cobre a barra inteira.
//
// Numa curva fechada a barra, que vai uns 100 mm a frente do eixo, fica
// quase paralela a linha e tambem ve uma faixa larga. Por isso a faixa so
// e tratada como atravessada (erro mantido) se o robo nao estiver virando:
// comando medio de direcao abaixo de CRUZ_COMANDO_MAX % da velocidade de
// curva. Com LARGURA_TOTAL sensores e faixa atravessada em qualquer situacao
// (nas pistas simuladas a linha em curva nao passou de 6).
#define LARGURA_FAIXA         5
#define LARGURA_CRUZAMENTO    8
#define LARGURA_TOTAL        10
#define CRUZ_COMANDO_MAX     30
#define FAIXA_MAX_MS         80   // erro mantido no maximo por este tempo

// Num trecho reto a linha nao anda mais que isto de um ciclo para o outro
// (400 = 3 mm em 1 ms). Um salto maior so e aceito se durar SALTO_CICLOS:
// da tempo de a faixa atravessada, que entra por um lado da barra, ser
// reconhecida.
#define SALTO_MAX           400
#define SALTO_CICLOS          3

// Perdeu a linha com o erro alem disso (1000 = um sensor): ela saiu pela
// lateral e o erro vai para o extremo. Senao (falha no meio) o erro e mantido.
#define LIMIAR_LATERAL     2000

// ------------------------------------------------------------------ modulo de inicio (IR)
// Regra 3.1.1 e 6.4 do Perseguidor (RoboCore, 12/12/2023): o robo liga e
// para pelo modulo de partida do juiz, sem tempo de seguranca. Item 8.2.3:
// vale um receptor IR proprio que siga o padrao dos modulos recomendados
// (JSumo MicroStart, Ja-Bots), que e o do startmodule.com: RC5, com
//   programar: endereco 0x0B, comando = numero da arena ("dohyo")
//   partida:   endereco 0x07, comando = numero da arena + 1
//   parada:    endereco 0x07, comando = numero da arena
// O numero e par e fica gravado na EEPROM, como no modulo de verdade.
// CONFERIR no briefing qual controle o juiz usa (comando 'ir' do console
// mostra o endereco e o comando de cada quadro recebido).
#define IR_END_PROGRAMA    0x0B
#define IR_END_PARTIDA     0x07
// 1 = para com o comando de parada de qualquer arena (mais seguro contra a
// regra 6.5, mas um juiz de outra arena por perto tambem para o robo).
// 0 = so com o numero gravado, como os modulos do padrao.
#define IR_PARADA_QUALQUER_ID  0
//
// Para testar na bancada com o controle NEC que vem com o KY-022: console
// 'nec=1' (teclas em 'necpart' e 'necpara', ver params.h). Desligar antes de
// competir: outro controle igual por perto tambem largaria e pararia o robo.

// ------------------------------------------------------------------ motores
// Motor A = esquerdo, motor B = direito. Se uma roda girar para tras no
// teste de motores, mude aqui (ou inverta os fios).
#define INVERTER_MOTOR_ESQ   0
#define INVERTER_MOTOR_DIR   0
#define TROCAR_MOTORES       0    // 1 = o motor A esta na direita

// ------------------------------------------------------------------ ciclo
// Periodo fixo do controle. Os tempos em ms dos modulos sao contados em
// ciclos, entao mudar isto muda a escala do kd (ver params.h).
#define PERIODO_US         1000

// ------------------------------------------------------------------ largada e parada
// Pelo modulo de inicio a saida e imediata (regra 3.1.1). Pelo botao 1, so
// para teste, o robo espera LARGADA_BOTAO_MS para a mao sair.
#define LARGADA_BOTAO_MS   1000
#define VENT_SUBIDA_MS      200   // rampa da ventoinha (as rodas saem junto, em rampa de 'acel')
#define VENT_POS_PARADA_MS  500   // ventoinha ligada depois de frear
#define VENT_MAXIMO         245   // como no EVA-RT (MAX_IMP_SPEED)
#define VEL_MAXIMA          140   // teto do PWM base ao subir de nivel (EVA-RT)

// ------------------------------------------------------------------ registro
#define LOG_TAMANHO         120   // amostras guardadas (as ultimas)
#define LOG_PERIODO_MS       20   // 120 x 20 ms = ultimos 2,4 s
