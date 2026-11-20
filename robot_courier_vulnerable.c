#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_PAYLOAD 256

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

/* === УЯЗВИМОСТЬ 1: переполнение кучи (heap overflow) → Invalid write === */
void handle_ota_update(const char *firmware_packet) {
    size_t len = strlen(firmware_packet);
    /* Выделяем буфер ровно под длину пакета, БЕЗ учёта '\0' */
    char *staging = (char *)malloc(len);
    if (!staging) return;

    /* strcpy запишет len+1 байт (с '\0') → 1 байт за границей блока */
    strcpy(staging, firmware_packet);

    printf("[OTA] Staged firmware (%zu bytes): %s\n", len, staging);
    free(staging);
}

/* === УЯЗВИМОСТЬ 2: неинициализированная переменная === */
int compute_telemetry_checksum(const telemetry_t *t) {
    int sum;  /* не инициализирована */
    for (int i = 0; i < (int)strlen(t->payload); i++) {
        sum += (unsigned char)t->payload[i];
    }
    return sum;
}

/* === УЯЗВИМОСТЬ 3: Double Free + Use-After-Free (арбитраж TMR) === */
void process_arbiter_votes(void) {
    vote_t *votes = (vote_t *)malloc(sizeof(vote_t) * 3);
    if (!votes) return;

    for (int i = 0; i < 3; i++) {
        votes[i].channel_id = i;
        votes[i].weight = 1;
        votes[i].timestamp = 0;
    }

    free(votes);
    free(votes);            /* Invalid free */

    votes[0].weight = 2;    /* Invalid write — UAF */
    printf("[VOTE] Channel 0 weight = %d\n", votes[0].weight); /* Invalid read */
}

int main(int argc, char **argv) {
    telemetry_t telemetry;
    telemetry.sensor_id = 1;
    telemetry.status = 0;
    strncpy(telemetry.payload, "battery:87;temp:23", MAX_PAYLOAD - 1);
    telemetry.payload[MAX_PAYLOAD - 1] = '\0';

    int cs = compute_telemetry_checksum(&telemetry);
    printf("[TELEM] checksum=%d\n", cs);

    process_arbiter_votes();

    /* OTA: передаём пакет, длина которого больше выделенного буфера */
    if (argc > 1) {
        handle_ota_update(argv[1]);
    } else {
        handle_ota_update("firmware_v1.0.0");
    }

    return 0;
}