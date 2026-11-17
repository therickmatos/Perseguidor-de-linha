/*
 * console.cpp - comandos pela serial (conversor USB ou Bluetooth, 115200)
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0
 * https://github.com/EXOTIC-TEAM-MX/EVA-RT
 *
 * Adaptado do firmware do seguidor RUSH - SATURN.
 *
 * So atende com o robo parado (regra 3.1.2 do Perseguidor: ajuste entre
 * partidas). Durante a partida nada e lido nem escrito na serial.
 *
 *   ?            ajuda e estado
 *   p            lista os parametros
 *   nome=valor   muda um parametro (vale ate desligar)
 *   salvar       grava os parametros na EEPROM
 *   padrao       volta aos valores de fabrica (nao grava)
 *   arena=n      grava o numero da arena do modulo de inicio (par, 0..126)
 *   ir           mostra os quadros RC5 recebidos (qualquer tecla para)
 *   s            sensores normalizados e erro (qualquer tecla para)
 *   r            leitura bruta dos 16 canais do mux, pino do IR e bateria
 *   res          resumo da ultima partida
 *   log          ultimos segundos da ultima partida, em CSV
 *   motores      teste das rodas (esq. frente/re, dir. frente/re)
 */
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "estado.h"
#include "hw.h"
#include "memoria.h"

// ---------------------------------------------------------------- tabela

enum { T_FLOAT, T_U8, T_U16 };

struct Descritor {
  char     nome[10];
  uint8_t  tipo;
  uint8_t  desloc;
  uint16_t minimo;
  uint16_t maximo;
};

#define PARAM(campo, tipo, mn, mx) \
  { #campo, tipo, (uint8_t)offsetof(Parametros, campo), mn, mx }

static const Descritor TABELA[] PROGMEM = {
  PARAM(kp, T_FLOAT, 0, 2),
  PARAM(kd, T_FLOAT, 0, 20),
  PARAM(fd, T_U8, 1, 100),
  PARAM(vel, T_U8, 0, 255),
  PARAM(reta, T_U8, 0, 200),
  PARAM(passo, T_U8, 0, 50),
  PARAM(vent, T_U8, 0, 255),
  PARAM(vpasso, T_U8, 0, 50),
  PARAM(nivel, T_U8, 0, 40),
  PARAM(ecurva, T_U16, 50, 5500),
  PARAM(treta, T_U16, 0, 5000),
  PARAM(rreta, T_U8, 0, 200),
  PARAM(rcurva, T_U8, 0, 200),
  PARAM(acel, T_U16, 10, 20000),
  PARAM(fcurva, T_U8, 0, 200),
  PARAM(tfcurva, T_U16, 1, 500),
  PARAM(re, T_U8, 0, 255),
  PARAM(perdido, T_U16, 1, 2000),
  PARAM(tmax, T_U16, 0, 600),
  PARAM(freio, T_U16, 0, 300),
  PARAM(tfreio, T_U16, 0, 1000),
  PARAM(comp, T_U8, 0, 1),
  PARAM(vref, T_U16, 6000, 9000),
  PARAM(batcal, T_U16, 300, 900),
  PARAM(batmin, T_U16, 0, 9000),
};
#define N_PARAMETROS  (sizeof(TABELA) / sizeof(TABELA[0]))

static void imprimirParametro(const Descritor &d) {
  const uint8_t *campo = (const uint8_t *)&P + d.desloc;
  Serial.print(d.nome);
  Serial.print('=');
  if (d.tipo == T_FLOAT) {
    float f;
    memcpy(&f, campo, sizeof(f));
    Serial.print(f, 3);
  } else if (d.tipo == T_U8) {
    Serial.print(*campo);
  } else {
    uint16_t v;
    memcpy(&v, campo, sizeof(v));
    Serial.print(v);
  }
}

static void listarParametros() {
  for (uint8_t i = 0; i < N_PARAMETROS; i++) {
    Descritor d;
    memcpy_P(&d, &TABELA[i], sizeof(d));
    imprimirParametro(d);
    Serial.print((i % 5 == 4 || i == N_PARAMETROS - 1) ? '\n' : '\t');
  }
}

static void definir(char *nome, const char *valor) {
  if (strcmp_P(nome, PSTR("arena")) == 0) {
    long v = atol(valor);
    if (v < 0 || v > 126 || (v & 1)) {
      Serial.println(F("arena: numero par de 0 a 126"));
      return;
    }
    arena = (uint8_t)v;
    arenaGravada = true;
    memSalvarArena(arena);
    consoleArena();
    return;
  }
  for (uint8_t i = 0; i < N_PARAMETROS; i++) {
    Descritor d;
    memcpy_P(&d, &TABELA[i], sizeof(d));
    if (strcmp(nome, d.nome) != 0) continue;

    uint8_t *campo = (uint8_t *)&P + d.desloc;
    if (d.tipo == T_FLOAT) {
      float f = (float)atof(valor);
      if (!(f >= (float)d.minimo && f <= (float)d.maximo)) break;
      memcpy(campo, &f, sizeof(f));
    } else {
      long v = atol(valor);
      if (v < (long)d.minimo || v > (long)d.maximo) break;
      if (d.tipo == T_U8) {
        *campo = (uint8_t)v;
      } else {
        uint16_t w = (uint16_t)v;
        memcpy(campo, &w, sizeof(w));
      }
    }
    if (strcmp_P(nome, PSTR("nivel")) == 0) nivelAtual = P.nivel;
    if (nivelAtual > nivelMaximo(P)) nivelAtual = nivelMaximo(P);
    imprimirParametro(d);
    Serial.println();
    return;
  }
  Serial.println(F("nome desconhecido ou valor fora da faixa ('p' lista)"));
}

// ---------------------------------------------------------------- relatorios

void consoleNivel() {
  Serial.print(F("nivel "));
  Serial.print(nivelAtual);
  Serial.print(F(": curva "));
  Serial.print(velCurvaDoNivel(P, nivelAtual));
  Serial.print(F(" reta "));
  Serial.print(velRetaDoNivel(P, nivelAtual));
  Serial.print(F(" ventoinha "));
  Serial.println(ventDoNivel(P, nivelAtual));
}

static void imprimirBateria() {
  for (uint8_t i = 0; i < 16; i++) hwBateriaAtualizar();
  uint16_t mv = hwBateriaMv(P);
  Serial.print(F("bateria "));
  Serial.print(mv);
  Serial.print(F(" mV, fator do PWM "));
  Serial.print(hwEscalaBateria(P) / 256.0f, 2);
  if (mv < P.batmin) Serial.print(F("  ** BAIXA **"));
  Serial.println();
}

void consoleArena() {
  Serial.print(F("modulo de inicio: arena "));
  Serial.print(arena);
  Serial.print(arenaGravada ? F("") : F(" (padrao, nunca programada)"));
  Serial.print(F(" -> partida = end "));
  Serial.print(IR_END_PARTIDA);
  Serial.print(F(" cmd "));
  Serial.print(arena | 1);
  Serial.print(F(", parada = end "));
  Serial.print(IR_END_PARTIDA);
  Serial.print(F(" cmd "));
  Serial.println(arena);
}

void consoleEstado() {
  Serial.println(F(FW_NOME " " FW_VERSAO " - baseado no EVA-RT (c) EXOTIC TEAM MX, AGPL-3.0"));
  Serial.print(F("calibracao: "));
  Serial.println(temCal ? F("ok") : F("falta"));
  consoleNivel();
  consoleArena();
  imprimirBateria();
}

static void ajuda() {
  consoleEstado();
  Serial.println(F("comandos: ?  p  nome=valor  salvar  padrao  arena=n  ir  s  r  res  log  motores"));
}

void consoleCalibracao(const Calibracao &c, uint16_t falha) {
  Serial.println(F("canal\tmin\tmax"));
  for (uint8_t i = 0; i < N_CANAIS; i++) {
    Serial.print('S');
    Serial.print(i);
    Serial.print('\t');
    Serial.print(c.mn[i]);
    Serial.print('\t');
    Serial.print(c.mx[i]);
    if (falha & ((uint16_t)1 << i)) Serial.print(F("\t<-- SEM CONTRASTE"));
    Serial.println();
  }
}

void consoleResumo() {
  const Resumo &r = corrida.resumo();
  Serial.print(F("fim: "));
  switch (r.motivo) {
    case FIM_PARADA:  Serial.print(F("COMANDO DE PARADA")); break;
    case FIM_PERDEU:  Serial.print(F("PERDEU A LINHA")); break;
    case FIM_BOTAO:   Serial.print(F("BOTAO")); break;
    case FIM_TEMPO:   Serial.print(F("TEMPO MAXIMO")); break;
    default:          Serial.println(F("nenhuma partida")); return;
  }
  Serial.print(F(" | andou "));
  Serial.print(r.totalMs / 1000.0f, 3);
  Serial.print(F(" s | linhas de partida "));
  Serial.print(r.faixas);
  Serial.print(F(" (~"));
  Serial.print(r.faixas / 2);
  Serial.println(F(" voltas)"));

  Serial.print(F("curva "));
  Serial.print(r.velCurva);
  Serial.print(F(" reta "));
  Serial.print(r.velReta);
  Serial.print(F(" ventoinha "));
  Serial.print(r.vent);
  Serial.print(F(" | em reta "));
  Serial.print(r.totalMs ? (r.retaMs * 100UL) / r.totalMs : 0);
  Serial.println(F(" % do tempo"));

  Serial.print(F("erro max "));
  Serial.print(r.erroMax);
  Serial.print(F(" | sem linha "));
  Serial.print(r.perdidoMs);
  Serial.println(F(" ms"));

  Serial.print(F("ciclo max "));
  Serial.print(cicloMaxUs);
  Serial.print(F(" us, atrasados "));
  Serial.println(ciclosAtrasados);
  imprimirBateria();
}

static void imprimirLog() {
  Serial.println(F("ms,erro,esq,dir,vista,faixa,reta"));
  uint8_t n = corrida.logQtd();
  for (uint8_t i = 0; i < n; i++) {
    const AmostraLog &a = corrida.logAmostra(i);
    Serial.print(-(int32_t)(n - 1 - i) * LOG_PERIODO_MS);   // 0 = ultima amostra
    Serial.print(',');
    Serial.print((int16_t)a.erro * 50);
    Serial.print(',');
    Serial.print((int16_t)a.esq * 2);
    Serial.print(',');
    Serial.print((int16_t)a.dir * 2);
    for (uint8_t b = LOG_VISTA; b <= LOG_RETA; b <<= 1) {
      Serial.print(',');
      Serial.print((a.bits & b) ? 1 : 0);
    }
    Serial.println();
  }
}

// ---------------------------------------------------------------- ao vivo

static bool interrompido() {
  return Serial.available() > 0 || hwBotao(1) || hwBotao(2);
}

static void esvaziarEntrada() {
  delay(20);
  while (Serial.available() > 0) Serial.read();
  while (hwBotao(1) || hwBotao(2)) {
  }
  delay(30);
}

// 12 sensores (0..255, 255 = linha), erro, L = linha vista, X = faixa atravessada
static void fluxoNormalizado() {
  if (!temCal) {
    Serial.println(F("calibre antes"));
    return;
  }
  corrida.preparar(cal);
  uint8_t bruto[N_CANAIS];
  while (!interrompido()) {
    for (uint8_t k = 0; k < 100 && !interrompido(); k++) {
      hwLerSensores(bruto);
      corrida.observar(bruto);
      delay(1);
    }
    for (uint8_t i = 0; i < N_SENSORES; i++) {
      Serial.print(corrida.linha().norm(i));
      Serial.print('\t');
    }
    Serial.print(F("erro "));
    Serial.print(corrida.linha().erro());
    Serial.print('\t');
    Serial.print(corrida.linha().vista() ? 'L' : '-');
    Serial.println(corrida.linha().cruzamento() ? 'X' : '-');
  }
  esvaziarEntrada();
}

// Bruto, sem depender do mapa: C0..C15 do mux, pino do IR e a bateria em mV
static void fluxoBruto() {
  Serial.println(F("C0..C15 | IR (1 = sem portadora) | bateria mV"));
  while (!interrompido()) {
    for (uint8_t c = 0; c < 16; c++) {
      Serial.print(hwLerCanalMux(c));
      Serial.print(c == 15 ? F("\t| ") : F("\t"));
    }
    Serial.print(hwIrNivel() ? 1 : 0);
    Serial.print(F("\t| "));
    hwBateriaAtualizar();
    Serial.println(hwBateriaMv(P));
    for (uint8_t k = 0; k < 20 && !interrompido(); k++) delay(10);
  }
  esvaziarEntrada();
}

// Quadros RC5 recebidos e o que significam para este robo
static void fluxoIR() {
  consoleArena();
  Serial.println(F("aperte os botoes do controle (qualquer tecla para)"));
  QuadroRC5 q;
  hwIrQuadro(q);   // descarta um quadro antigo
  while (!interrompido()) {
    if (!hwIrQuadro(q)) continue;
    Serial.print(F("end "));
    Serial.print(q.endereco);
    Serial.print(F(" cmd "));
    Serial.print(q.comando);
    Serial.print(F(" T "));
    Serial.print(q.toggle);
    switch (interpretarRC5(q, arena, IR_PARADA_QUALQUER_ID, IR_END_PROGRAMA, IR_END_PARTIDA)) {
      case IR_PARTIDA:  Serial.println(F("  = PARTIDA")); break;
      case IR_PARADA:   Serial.println(F("  = PARADA")); break;
      case IR_PROGRAMA: Serial.println(F("  = PROGRAMAR (aqui nao grava; use fora do 'ir')")); break;
      default:          Serial.println(F("  (outra arena ou outro aparelho)")); break;
    }
    hwLedC(true);
    delay(30);
    hwLedC(false);
  }
  esvaziarEntrada();
}

void testeMotores() {
  for (uint8_t k = 0; k < 4; k++) {
    bool esq = (k < 2);
    int16_t v = (k & 1) ? -30 : 30;
    Serial.print(esq ? F("roda ESQUERDA ") : F("roda DIREITA "));
    Serial.println(v > 0 ? F("para a frente") : F("para tras"));
    hwLedA(esq);
    hwLedB(!esq);
    hwLedC(v < 0);
    hwMotores(esq ? v : 0, esq ? 0 : v);
    delay(500);
    hwMotores(0, 0);
    hwLeds(false);
    delay(1000);
  }
}

// ---------------------------------------------------------------- comandos

static void executar(char *cmd) {
  char *igual = strchr(cmd, '=');
  if (igual) {
    *igual = 0;
    definir(cmd, igual + 1);
  } else if (!strcmp_P(cmd, PSTR("?"))) {
    ajuda();
  } else if (!strcmp_P(cmd, PSTR("p"))) {
    listarParametros();
  } else if (!strcmp_P(cmd, PSTR("salvar"))) {
    memSalvarParametros(P);
    Serial.println(F("parametros gravados"));
  } else if (!strcmp_P(cmd, PSTR("padrao"))) {
    parametrosPadrao(P);
    nivelAtual = P.nivel;
    listarParametros();
  } else if (!strcmp_P(cmd, PSTR("s"))) {
    fluxoNormalizado();
  } else if (!strcmp_P(cmd, PSTR("r"))) {
    fluxoBruto();
  } else if (!strcmp_P(cmd, PSTR("ir"))) {
    fluxoIR();
  } else if (!strcmp_P(cmd, PSTR("res"))) {
    consoleResumo();
  } else if (!strcmp_P(cmd, PSTR("log"))) {
    imprimirLog();
  } else if (!strcmp_P(cmd, PSTR("motores"))) {
    testeMotores();
  } else {
    Serial.println(F("? para ajuda"));
  }
}

void consoleProcessar() {
  static char linha[28];
  static uint8_t n = 0;
  static bool estourou = false;

  while (Serial.available() > 0) {
    char ch = (char)Serial.read();
    if (ch == '\r' || ch == '\n') {
      if (n > 0 && !estourou) {
        linha[n] = 0;
        executar(linha);
      }
      n = 0;
      estourou = false;
    } else if (ch == ' ') {
      // ignora espacos ("kp = 0.05")
    } else if (ch < ' ' || ch > '~') {
      estourou = true;   // lixo na linha (RX solto, baud errado): descarta
    } else if (n < sizeof(linha) - 1) {
      linha[n++] = (ch >= 'A' && ch <= 'Z') ? (char)(ch + 32) : ch;
    } else {
      estourou = true;
    }
  }
}
