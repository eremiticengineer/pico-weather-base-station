#include "https-tasks.hpp"

void https_post_task(void *pvParameters) {
    HttpsTaskParams* pParam = static_cast<HttpsTaskParams*>(pvParameters);

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
