#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "HCF_IOTEC.h"
#include "HCF_LCD.h"
#include "HCF_ADC.h"
#include "HCF_MP.h"

// Macros
#define IN(x) ((entradas >> x) & 1)
#define RELE_NA 0   // Relé normalmente aberto na saída 0
#define RELE_NF 1   // Relé normalmente fechado na saída 1

#define LIGA_RELES       \
    saidas |= (1 << RELE_NA);   /* Liga NA */ \
    saidas &= ~(1 << RELE_NF);  /* Desliga NF */

#define DESLIGA_RELES    \
    saidas &= ~(1 << RELE_NA);  /* Desliga NA */ \
    saidas |= (1 << RELE_NF);   /* Liga NF */

#define ESTEIRA_ATIVA ((saidas & (1 << RELE_NA)) && !(saidas & (1 << RELE_NF)))

// Variáveis globais
static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0;
static char tecla = '-';
char escrever[40];
uint16_t contador_pecas = 0;
uint8_t sensor_anterior = 0;

void app_main(void)
{
    escrever[39] = '\0';

    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    // Inicializações
    iniciar_iotec();
    entradas = io_le_escreve(saidas);
    iniciar_lcd();
    escreve_lcd(1, 0, "    Jornada 1   ");
    escreve_lcd(2, 0, " Programa Basico");

    esp_err_t init_result = iniciar_adc_CHX(0);
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar ADC");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    limpar_lcd();

    // Desliga relés no início
    DESLIGA_RELES;
    entradas = io_le_escreve(saidas);

    while (1)
    {
        tecla = le_teclado();

        if (tecla == '1') {
            LIGA_RELES;
        }
        else if (tecla == '2') {
            DESLIGA_RELES;
        }
        else if (tecla == 'C' || tecla == 'c') {
            contador_pecas = 0;
        }

        entradas = io_le_escreve(saidas);

        // Contagem de peças via sensor capacitivo (entrada 0)
        uint8_t sensor_atual = IN(4);
        if (sensor_atual == 1 && sensor_anterior == 0) {
            contador_pecas++;
            sensor_anterior = 1;
        } else {
            sensor_anterior = sensor_atual;
        }

        // Mostrar entradas
        sprintf(escrever, "INPUTS:%d%d%d%d%d%d%d%d", IN(7), IN(6), IN(5), IN(4), IN(3), IN(2), IN(1), IN(0));
        escreve_lcd(1, 0, escrever);

        // Mostrar contagem e status dos relés
        sprintf(escrever, "Pecas:%04d %s", contador_pecas, ESTEIRA_ATIVA ? "ON " : "OFF");
        escreve_lcd(2, 0, escrever);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    adc_limpar();
}
