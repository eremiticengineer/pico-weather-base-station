#include "https-tasks.hpp"

void https_post_task(void *pvParameters) {
    HttpsTaskParams* pParam = static_cast<HttpsTaskParams*>(pvParameters);

    if (!pParam->https_client->initialiseWifi(pParam->wifi_ssid, pParam->wifi_password)) {
        printf("WiFi failed: %s\n", pParam->https_client->errorMessage().c_str());
        while (true) { tight_loop_contents(); }
    }

    HttpsPostMessage message;

    while (true) {
        if (xQueueReceive(pParam->https_post_queue, &message, portMAX_DELAY)) {
            printf("posting to website...\n");

            if (!pParam->https_client->post(message.json)) {
                printf("POST failed: %s\n", pParam->https_client->errorMessage().c_str());
            } else {
                printf("POST successful: HTTP %d\n", pParam->https_client->statusCode());
            }
        }
    }
}
