/*
 * teste_ir.cpp - confere o decodificador RC5 (ir_rc5.cpp) no PC
 *
 * Parte do firmware do perseguidor RUSH - Dreamcast, baseado no EVA-RT 1.2.2
 * (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0.
 *
 * Gera a saida do receptor (alto = sem portadora) para quadros RC5, com o
 * que o receptor e a interrupcao fazem com eles: pulsos com portadora mais
 * longos ou mais curtos, atraso variavel da interrupcao e a resolucao de 4 us
 * do micros(). Confere tambem quadros estragados, ruido e o significado de
 * cada quadro para o modulo de inicio.
 *
 * Rodar nesta pasta:
 *   g++ -O2 -std=c++11 -I../perseguidor teste_ir.cpp ../perseguidor/ir_rc5.cpp -o teste_ir && ./teste_ir
 */
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "ir_rc5.h"

static int falhas = 0;
static void conferir(bool ok, const char *o_que) {
  if (!ok) { falhas++; std::printf("    FALHOU: %s\n", o_que); }
}

static unsigned semente = 2024;
static double aleatorio() {   // 0..1
  semente = semente * 1103515245u + 12345u;
  return ((semente >> 16) & 0x7FFF) / 32767.0;
}

struct Borda { double t; bool nivel; };

// Bordas de um quadro a partir de t0 (inicio do S1). distorcao: quanto o
// receptor alarga (+) ou encurta (-) cada pulso com portadora, em us.
static void quadro(std::vector<Borda> &b, double t0, uint8_t end, uint8_t cmd, uint8_t tog,
                   double distorcao) {
  const double MEIO = 889.0;
  uint16_t bits = (uint16_t)(1u << 13) | (uint16_t)(((cmd & 0x40) ? 0 : 1) << 12) |
                  (uint16_t)((tog & 1) << 11) | (uint16_t)((end & 0x1F) << 6) | (cmd & 0x3F);
  const size_t antes = b.size();
  bool nivel = true;   // linha parada (sem portadora)
  for (int i = 13; i >= 0; i--) {
    bool um = (bits >> i) & 1;
    bool primeira = um;          // "1": sem portadora e depois com
    double tb = t0 + (13 - i) * 2 * MEIO;
    if (primeira != nivel) { b.push_back({tb, primeira}); nivel = primeira; }
    b.push_back({tb + MEIO, !primeira});
    nivel = !primeira;
  }
  if (!nivel) b.push_back({t0 + 14 * 2 * MEIO, true});
  // o receptor atrasa a subida (fim da portadora) em relacao a descida
  for (size_t i = antes; i < b.size(); i++) if (b[i].nivel) b[i].t += distorcao;
}

// Passa as bordas pelo decodificador como a interrupcao faria e devolve os quadros
static std::vector<QuadroRC5> decodificar(std::vector<Borda> b, double atrasoMax, uint32_t base = 0) {
  std::vector<QuadroRC5> saida;
  DecodificadorRC5 d;
  d.reiniciar();
  for (const Borda &x : b) {
    double t = x.t + atrasoMax * aleatorio();
    uint32_t us = base + ((uint32_t)t & ~3u);   // micros() anda de 4 em 4
    if (d.borda(us, x.nivel)) saida.push_back(d.quadro());
  }
  return saida;
}

int main() {
  // ---------------------------------------------------------------- 1. todos os comandos
  const double distorcoes[] = {-200, -100, 0, 100, 200, 300};
  const uint8_t enderecos[] = {0, 7, 11, 31};
  long total = 0, certos = 0;
  for (double dist : distorcoes)
    for (uint8_t end : enderecos)
      for (int cmd = 0; cmd < 128; cmd++)
        for (int tog = 0; tog < 2; tog++) {
          std::vector<Borda> b;
          quadro(b, 5000, end, (uint8_t)cmd, (uint8_t)tog, dist);
          std::vector<QuadroRC5> q = decodificar(b, 30);
          total++;
          if (q.size() == 1 && q[0].endereco == end && q[0].comando == cmd && q[0].toggle == tog) certos++;
        }
  std::printf("1. quadros isolados (distorcao -200..+300 us, atraso ate 30 us): %ld de %ld certos\n",
              certos, total);
  conferir(certos == total, "quadro isolado mal decodificado");

  // ---------------------------------------------------------------- 2. repeticao
  {
    std::vector<Borda> b;
    for (int k = 0; k < 10; k++) quadro(b, 5000 + k * 113778.0, 7, 1, 0, 150);
    std::vector<QuadroRC5> q = decodificar(b, 30);
    std::printf("2. botao segurado (10 quadros a cada 113,8 ms): %zu decodificados\n", q.size());
    conferir(q.size() == 10, "perdeu repeticao");
  }

  // ---------------------------------------------------------------- 3. volta do micros()
  {
    std::vector<Borda> b;
    quadro(b, 5000, 7, 21, 1, 100);
    std::vector<QuadroRC5> q = decodificar(b, 30, 0xFFFFFFFFu - 20000u);
    std::printf("3. micros() dando a volta no meio do quadro: %s\n",
                (q.size() == 1 && q[0].comando == 21) ? "ok" : "erro");
    conferir(q.size() == 1 && q[0].endereco == 7 && q[0].comando == 21, "volta do micros()");
  }

  // ---------------------------------------------------------------- 4. quadro estragado
  // Um pico de 100 us no meio de cada um dos bits: o quadro pode se perder,
  // mas nunca virar outro quadro. Depois dele vem um bom, que tem de passar.
  {
    int trocados = 0, perdidos = 0, bonsDepois = 0;
    for (int bit = 0; bit < 14; bit++) {
      std::vector<Borda> b;
      quadro(b, 5000, 7, 1, 0, 0);
      double tp = 5000 + bit * 1778 + 400;
      // pico: inverte o nivel por 100 us
      bool nivel = true;
      for (const Borda &x : b) if (x.t <= tp) nivel = x.nivel;
      b.push_back({tp, !nivel});
      b.push_back({tp + 100, nivel});
      std::vector<Borda> ord;
      for (const Borda &x : b) {
        size_t i = 0;
        while (i < ord.size() && ord[i].t <= x.t) i++;
        ord.insert(ord.begin() + i, x);
      }
      quadro(ord, 5000 + 113778.0, 7, 0, 1, 0);
      std::vector<QuadroRC5> q = decodificar(ord, 0);
      bool primeiroCerto = !q.empty() && q[0].endereco == 7 && q[0].comando == 1;
      bool ultimoCerto = !q.empty() && q.back().endereco == 7 && q.back().comando == 0 && q.back().toggle == 1;
      if (!q.empty() && !primeiroCerto && !(q.size() == 1 && ultimoCerto)) trocados++;
      if (q.size() < 2) perdidos++;
      if (ultimoCerto) bonsDepois++;
    }
    std::printf("4. pico de 100 us em cada bit: %d viraram outro quadro, %d perdidos, %d de 14 bons depois\n",
                trocados, perdidos, bonsDepois);
    conferir(trocados == 0, "pico virou outro quadro");
    conferir(bonsDepois == 14, "o quadro bom seguinte se perdeu");
  }

  // ---------------------------------------------------------------- 5. quadro cortado
  {
    std::vector<Borda> b, c;
    quadro(b, 5000, 7, 1, 0, 0);
    for (size_t i = 0; i < b.size() / 2; i++) c.push_back(b[i]);
    if (!c.back().nivel) c.push_back({c.back().t + 300, true});
    quadro(c, 40000, 7, 0, 1, 0);
    std::vector<QuadroRC5> q = decodificar(c, 0);
    std::printf("5. meio quadro e depois um inteiro: %zu decodificado(s)\n", q.size());
    conferir(q.size() == 1 && q[0].comando == 0 && q[0].toggle == 1, "quadro cortado");
  }

  // ---------------------------------------------------------------- 6. ruido
  // Pulsos aleatorios de 100 us a 3 ms (luz de camera, reflexo): conta quantos
  // quadros validos aparecem e quantos seriam partida ou parada na arena 0.
  {
    std::vector<Borda> b;
    double t = 0;
    bool nivel = true;
    while (t < 600e6) {   // 10 minutos
      t += 100 + 2900 * aleatorio();
      nivel = !nivel;
      b.push_back({t, nivel});
    }
    std::vector<QuadroRC5> q = decodificar(b, 30);
    int partida = 0, parada = 0, programa = 0;
    for (const QuadroRC5 &x : q) {
      ComandoIR c = interpretarRC5(x, 0, false, 0x0B, 0x07);
      if (c == IR_PARTIDA) partida++;
      if (c == IR_PARADA) parada++;
      if (c == IR_PROGRAMA) programa++;
    }
    std::printf("6. 10 min de ruido (%zu bordas): %zu quadros validos; partida %d, parada %d, programar %d\n",
                b.size(), q.size(), partida, parada, programa);
  }

  // ---------------------------------------------------------------- 7. significado
  {
    QuadroRC5 q;
    q.toggle = 0;
    q.endereco = 0x0B; q.comando = 0x14;
    conferir(interpretarRC5(q, 0, false, 0x0B, 0x07) == IR_PROGRAMA && arenaDoQuadro(q) == 0x14, "programar");
    q.endereco = 0x0B; q.comando = 0x15;
    conferir(arenaDoQuadro(q) == 0x14, "programar com numero impar");
    q.endereco = 0x07; q.comando = 0x15;
    conferir(interpretarRC5(q, 0x14, false, 0x0B, 0x07) == IR_PARTIDA, "partida");
    q.comando = 0x14;
    conferir(interpretarRC5(q, 0x14, false, 0x0B, 0x07) == IR_PARADA, "parada");
    q.comando = 0x16;
    conferir(interpretarRC5(q, 0x14, false, 0x0B, 0x07) == IR_OUTRO, "parada de outra arena");
    conferir(interpretarRC5(q, 0x14, true, 0x0B, 0x07) == IR_PARADA, "parada de qualquer arena");
    q.comando = 0x17;
    conferir(interpretarRC5(q, 0x14, true, 0x0B, 0x07) == IR_OUTRO, "partida de outra arena");
    q.endereco = 0x00; q.comando = 0x15;
    conferir(interpretarRC5(q, 0x14, true, 0x0B, 0x07) == IR_OUTRO, "outro aparelho");
    std::printf("7. significado dos quadros (programar, partida, parada, outra arena): conferido\n");
  }

  std::printf("\n%s\n", falhas ? "HOUVE FALHAS" : "tudo certo");
  return falhas ? 1 : 0;
}
