#include "wifi-tasks.hpp"

void wifi_task(void *pvParameters) {
    WifiTaskParams* pParam = static_cast<WifiTaskParams*>(pvParameters);

    // For pico_cyw43_arch_lwip_sys_freertos, initialise CYW43 from a task,
    // after the scheduler has started.
    if (!pParam->wifi->initialise()) {
        printf("WiFi initialisation failed: %s\n", pParam->wifi->errorMessage().c_str());
        vTaskDelete(nullptr);
        return;
    }

    pParam->wifi->start();

    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        pParam->wifi->process();

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(1000));
    }
}
