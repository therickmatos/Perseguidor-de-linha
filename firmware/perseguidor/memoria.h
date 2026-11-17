/*
 * memoria.h - parametros, calibracao e numero da arena (IR) na EEPROM
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0
 */
#pragma once

#include "linha.h"
#include "params.h"

bool memCarregarParametros(Parametros &p);   // false = nada valido gravado
void memSalvarParametros(const Parametros &p);

// A calibracao vale so para o mapa da barra e a cor de linha com que foi feita
bool memCarregarCalibracao(Calibracao &c);
void memSalvarCalibracao(const Calibracao &c);

// Numero da arena do modulo de inicio (gravado pelo comando de programar do
// juiz, como no modulo de verdade). false = nunca foi gravado.
bool memCarregarArena(uint8_t &arena);
void memSalvarArena(uint8_t arena);
