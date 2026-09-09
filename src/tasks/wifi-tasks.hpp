#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"

#include "wifi.hpp"

struct WifiTaskParams {
    pico_wifi::PicoWifi *wifi;
};

void wifi_task(void *pvParameters);