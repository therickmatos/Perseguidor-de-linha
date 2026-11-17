/*
 * estado.h - variaveis globais do sketch e funcoes do console
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0
 */
#pragma once

#include "corrida.h"

extern Parametros P;
extern Calibracao cal;
extern bool       temCal;
extern uint8_t    nivelAtual;
extern Corrida    corrida;
extern uint16_t   cicloMaxUs;        // maior tempo de um ciclo na ultima partida
extern uint16_t   ciclosAtrasados;   // ciclos que passaram de PERIODO_US
extern uint8_t    arena;             // numero da arena do modulo de inicio (par)
extern bool       arenaGravada;      // o juiz (ou o console) ja gravou um numero

// console.cpp (so e chamado com o robo parado)
void consoleProcessar();
void consoleEstado();
void consoleNivel();
void consoleResumo();
void consoleCalibracao(const Calibracao &c, uint16_t falha);
void consoleArena();
void testeMotores();
