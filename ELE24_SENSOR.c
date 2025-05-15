#include "ele24_sensor.h"
#include "HCF_IOTEC.h"

static uint8_t pino_sensor = 0;
static uint8_t ultimo_estado = 0;

void sensor_init(uint8_t pino_entrada)
{
    pino_sensor = pino_entrada;
    ultimo_estado = (io_le_escreve(0) >> pino_sensor) & 1;
}

bool sensor_detectou_peca(void)
{
    uint8_t entradas = io_le_escreve(0);  // só lê sem alterar saídas
    uint8_t estado_atual = (entradas >> pino_sensor) & 1;

    bool detectado = false;
    if (estado_atual == 1 && ultimo_estado == 0) {
        detectado = true;
    }

    ultimo_estado = estado_atual;
    return detectado;
}


