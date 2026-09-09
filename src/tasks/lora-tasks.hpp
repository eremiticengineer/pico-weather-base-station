#pragma once

#include "hardware/spi.h"

#include <cstring>
#include <vector>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "LoRaPacket.hpp"
#include "SX1278.hpp"
#include "HttpsPostMessage.hpp"

namespace lora_config {
    inline spi_inst_t* SPI = spi0;
    inline constexpr uint SCK   = 18;
    inline constexpr uint MOSI  = 19;
    inline constexpr uint MISO  = 16;
    inline constexpr uint CS    = 17;
    inline constexpr uint RESET = 20;
}

struct LoraTaskParams {
    SX1278* lora;
    QueueHandle_t https_post_queue;
};

void lora_receive_weather_data_task(void *pvParameters);