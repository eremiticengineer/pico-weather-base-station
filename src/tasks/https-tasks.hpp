#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "HttpsClient.hpp"
#include "WebServerCertificate.hpp"

#include "HttpsPostMessage.hpp"

struct HttpsTaskParams {
    HttpsClient* https_client;
    QueueHandle_t https_post_queue;
    const char* wifi_ssid;
    const char* wifi_password;
};

void https_post_task(void *pvParameters);