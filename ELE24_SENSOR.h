#ifndef ELE24_SENSOR_H
#define ELE24_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

// Inicializa o sensor capacitivo
void sensor_init(uint8_t pino_entrada);

// Deve ser chamada em loop; retorna true quando uma nova peça for detectada
bool sensor_detectou_peca(void);

#endif // SENSOR_CAPACITIVO_H
