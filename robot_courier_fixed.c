#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_PAYLOAD 256
#define NUM_CHANNELS 3

typedef struct {
    uint16_t sensor_id;
    uint8_t  status;
    char     payload[MAX_PAYLOAD];
} telemetry_t;

typedef struct {
    uint8_t channel_id;
    uint16_t weight;
    uint32_t timestamp;
} vote_t;

/* === ИСПРАВЛЕНО 1: переполнение кучи (heap overflow) ===
 * Выделяем len + 1 байт (с учётом '\0') и копируем через memcpy,
 * явно ставя терминатор внутри выделенного блока.
 */
void handle_ota_update(const char *firmware_packet) {
    if (firmware_packet == NULL) return;

    size_t len = strlen(firmware_packet);

    /* +1 байт для завершающего нуля */
    char *staging = (char *)malloc(len + 1);
    if (!staging) {
        fprintf(stderr, "[OTA] malloc failed\n");
        return;
    }

    /* Копируем ровно len байт, затем явно ставим '\0' */
    memcpy(staging, firmware_packet, len);
    staging[len] = '\0';

    printf("[OTA] Staged firmware (%zu bytes): %s\n", len, staging);

    free(staging);
    staging = NULL;  /* защита от повторного использования */
}

/* === ИСПРАВЛЕНО 2: неинициализированная переменная ===
 * Инициализируем sum нулём.
 */
int compute_telemetry_checksum(const telemetry_t *t) {
    if (t == NULL) return 0;

    unsigned int sum = 0;  /* явная инициализация */

    size_t n = strlen(t->payload);
    for (size_t i = 0; i < n; i++) {
        sum += (unsigned char)t->payload[i];
    }

    return (int)(sum & 0xFFFF);  /* явное приведение к 16 битам */
}

/* === ИСПРАВЛЕНО 3: Double Free + Use-After-Free ===
 * Один free, обращение к памяти — только до освобождения,
 * указатель обнуляется после free.
 */
void process_arbiter_votes(void) {
    vote_t *votes = (vote_t *)malloc(sizeof(vote_t) * NUM_CHANNELS);
    if (!votes) {
        fprintf(stderr, "[VOTE] malloc failed\n");
        return;
    }

    for (int i = 0; i < NUM_CHANNELS; i++) {
        votes[i].channel_id = (uint8_t)i;
        votes[i].weight = 1;
        votes[i].timestamp = 0;
    }

    /* Сначала используем память, пока она валидна */
    votes[0].weight = 2;
    printf("[VOTE] Channel 0 weight = %d\n", votes[0].weight);

    /* Затем освобождаем ровно один раз */
    free(votes);
    votes = NULL;
}

int main(int argc, char **argv) {
    telemetry_t telemetry;
    memset(&telemetry, 0, sizeof(telemetry));  /* чистая структура */

    telemetry.sensor_id = 1;
    telemetry.status = 0;
    strncpy(telemetry.payload, "battery:87;temp:23", MAX_PAYLOAD - 1);
    telemetry.payload[MAX_PAYLOAD - 1] = '\0';

    int cs = compute_telemetry_checksum(&telemetry);
    printf("[TELEM] checksum=%d\n", cs);

    process_arbiter_votes();

    if (argc > 1) {
        handle_ota_update(argv[1]);
    } else {
        handle_ota_update("firmware_v1.0.0");
    }

    return 0;
}