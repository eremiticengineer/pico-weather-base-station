#include "lora-tasks.hpp"

void lora_receive_weather_data_task(void* pvParameters) {
    LoraTaskParams* pParam = static_cast<LoraTaskParams*>(pvParameters);

    std::vector<uint8_t> packet;

    uint32_t expectedSequence = 0;
    uint32_t currentSessionId = 0;
    bool haveSession = false;

    LoRaStats stats;

    while (true) {
        if (pParam->lora->receive(packet)) {
            /*
             * Read these immediately after receiving
             * the packet. They refer to the most
             * recently received LoRa packet.
             */
            const int16_t rssi = pParam->lora->getPacketRssi();
            const float snr = pParam->lora->getPacketSnr();

            if (packet.size() < sizeof(PacketHeader)) {
                printf("Invalid packet: too short\n");
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }

            PacketHeader header;

            std::memcpy(&header, packet.data(), sizeof(PacketHeader));

            const uint32_t sequence = header.sequence;

            /*
             * Session and sequence tracking.
             */
            if (!haveSession || header.sessionId != currentSessionId) {
                currentSessionId = header.sessionId;
                expectedSequence = sequence + 1;
                haveSession = true;

                printf("New sender session: %lu\n", static_cast<unsigned long>(currentSessionId));
            }
            else if (sequence == expectedSequence) {
                expectedSequence++;
            }
            else if (sequence > expectedSequence) {
                uint32_t lost = sequence - expectedSequence;

                printf("Lost %lu packet(s)\n", static_cast<unsigned long>(lost));

                stats.addLostPackets(lost);

                expectedSequence = sequence + 1;
            }
            else {
                printf("Old/duplicate packet: %lu\n", static_cast<unsigned long>(sequence));
            }

            /*
             * Add this successfully received packet
             * to the running radio statistics.
             */
            stats.addPacket(rssi, snr);

            switch (header.type) {
                case PacketType::Weather: {
                    const size_t expectedSize = sizeof(PacketHeader) + sizeof(WeatherPayload);

                    if (packet.size() != expectedSize) {
                        printf("Invalid weather packet size: %u\n", static_cast<unsigned>(packet.size()));
                        break;
                    }

                    WeatherPayload weather;

                    std::memcpy(&weather, packet.data() + sizeof(PacketHeader), sizeof(WeatherPayload));

                    printf(
                        "LINK "
                        "received=%llu "
                        "lost=%llu "
                        "loss=%.2f%% "
                        "RSSI avg=%.1fdBm "
                        "min=%ddBm "
                        "SNR avg=%.1fdB "
                        "min=%.1fdB\n",

                        static_cast<unsigned long long>(stats.packetsReceived),
                        static_cast<unsigned long long>(stats.packetsLost),

                        stats.packetLossPercentage(),

                        stats.averageRssi(),
                        stats.minimumRssi,

                        stats.averageSnr(),
                        stats.minimumSnr
                    );

                    printf(
                        "timestamp=%lu "
                        "bootId=%u "
                        "temp=%.1fC "
                        "pressure=%.1fhPa "
                        "humidity=%.1f%% "
                        "wind=%.1fmph "
                        "gust=%.1fmph "
                        "name=%s "
                        "direction=%u "
                        "rain=%d "
                        "lux=%.1f "
                        "battery=%.2fV "
                        "sensors=%u\n",

                        weather.timestamp,

                        weather.bootId,

                        weather.temperature,
                        weather.pressure,
                        weather.humidity,

                        weather.windSpeed,
                        weather.windGust,
                        weather.windDirectionName,
                        weather.windDirectionDegrees,

                        weather.rainTipsSinceBoot,

                        weather.lux,

                        weather.batteryVoltage,

                        weather.validSensors
                    );

                    printf("\n");

                    HttpsPostMessage message;
                    snprintf(
                        message.json,
                        sizeof(message.json),
                        "{"
                        "\"packetType\":%u,"
                        "\"received\":%llu,"
                        "\"lost\":%llu,"
                        "\"loss\":%.2f,"
                        "\"rssiAvg\":%.1f,"
                        "\"rssiMin\":%d,"
                        "\"snrAvg\":%.1f,"
                        "\"snrMin\":%.1f,"
                        "\"timestamp\":%lu,"
                        "\"bootId\":%lu,"
                        "\"temperature\":%.1f,"
                        "\"humidity\":%.1f,"
                        "\"pressure\":%.1f,"
                        "\"windSpeed\":%.1f,"
                        "\"windGust\":%.1f,"
                        "\"windDirectionName\":\"%s\","
                        "\"windDirectionDegrees\":%u,"
                        "\"lux\":%.1f,"
                        "\"rainTipsSinceBoot\":%lu,"
                        "\"batteryVoltage\":%.2f,"
                        "\"validSensors\":%u"
                        "}",
                        static_cast<unsigned int>(PacketType::Weather),
                        static_cast<unsigned long long>(stats.packetsReceived),
                        static_cast<unsigned long long>(stats.packetsLost),
                        stats.packetLossPercentage(),
                        stats.averageRssi(),
                        stats.minimumRssi,
                        stats.averageSnr(),
                        stats.minimumSnr,
                        static_cast<unsigned long>(weather.timestamp),
                        static_cast<unsigned long>(weather.bootId),
                        weather.temperature,
                        weather.humidity,
                        weather.pressure,
                        weather.windSpeed,
                        weather.windGust,
                        weather.windDirectionName,
                        weather.windDirectionDegrees,
                        weather.lux,
                        static_cast<unsigned long>(weather.rainTipsSinceBoot),
                        weather.batteryVoltage,
                        weather.validSensors);

                        xQueueSend(pParam->https_post_queue, &message, portMAX_DELAY);

                    break;
                }

                case PacketType::Status:
                    printf("Received status packet\n");
                    break;

                case PacketType::Debug:
                    printf("Received debug packet\n");
                    break;

                default:
                    printf(
                        "Unknown packet type: %u\n",
                        static_cast<unsigned>(header.type)
                    );

                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
