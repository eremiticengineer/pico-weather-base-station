#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "secrets.hpp"

#include "tasks/lora-tasks.hpp"
#include "tasks/https-tasks.hpp"

// The lora receive task will tell the https post task to post the weather data to the server
QueueHandle_t https_post_queue;

int main( void )
{
    stdio_init_all();

    sleep_ms(2000);

    SX1278Config sx1278_config;
    sx1278_config.frequencyHz = 433920000;
    sx1278_config.bandwidth = LoRaBandwidth::BW_125_KHZ;
    sx1278_config.codingRate = LoRaCodingRate::CR_4_5;
    sx1278_config.spreadingFactor = 7;
    sx1278_config.crcEnabled = true;
    sx1278_config.preambleLength = 8;
    sx1278_config.syncWord = 0x12;
    // mW = 10^(txPowerDbm / 10) = 10mW
    // dBm = 10log10(mw)
    // +3dB ~ doubles the power, +10dB multiplies power by 10
    sx1278_config.txPowerDbm = 10;

    SX1278 sx1278(
        lora_config::SPI,
        lora_config::CS,
        lora_config::RESET,
        lora_config::SCK,
        lora_config::MOSI,
        lora_config::MISO
    );

    https_post_queue = xQueueCreate(8, sizeof(HttpsPostMessage));
    LoraTaskParams lora_task_params {
        .lora = &sx1278,
        .https_post_queue = https_post_queue
    };

    constexpr UBaseType_t LORA_RECEIVE_TASK_PRIORITY = tskIDLE_PRIORITY + 2UL;
    constexpr configSTACK_DEPTH_TYPE LORA_RECEIVE_TASK_STACK_SIZE = 512;

    HttpsClient https_client(
        SERVER,
        SERVER_PATH,
        WEB_SERVER_ROOT_CA,
        sizeof(WEB_SERVER_ROOT_CA),
        API_KEY
    );

    HttpsTaskParams https_task_params {
        .https_client = &https_client,
        .https_post_queue = https_post_queue,
        .wifi_ssid = WIFI_SSID,
        .wifi_password = WIFI_PASSWORD
    };

    constexpr UBaseType_t HTTPS_TASK_PRIORITY = tskIDLE_PRIORITY + 2UL;
    constexpr configSTACK_DEPTH_TYPE HTTPS_TASK_STACK_SIZE = 1024;

    if (sx1278.init(sx1278_config)) {
        printf("SX1278 detected, version: 0x%02X\n", sx1278.getVersion());
        sx1278.startReceive();

        xTaskCreate(lora_receive_weather_data_task, "LoRaReceiveWeatherDataTask", LORA_RECEIVE_TASK_STACK_SIZE,
            (void*)&lora_task_params, LORA_RECEIVE_TASK_PRIORITY, nullptr);

        xTaskCreate(https_post_task, "HttpsPostTask", HTTPS_TASK_STACK_SIZE,
            (void*)&https_task_params, HTTPS_TASK_PRIORITY, nullptr);

        vTaskStartScheduler();
    }
    else {
        printf("SX1278 not detected\n");
    }

    while (true) { tight_loop_contents(); }
}