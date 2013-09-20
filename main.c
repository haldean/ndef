#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ndef.h"

int main(int argc, char** argv) {
    struct ndef_record* r = ndef_create(
            0x01, true, true, false, false, false,
            "application/vnd.org.haldean.test", 33,
            NULL, 0,
            "Why is the world in love again?", 32);

    if (r == NULL) {
        return -1;
    }

    printf("generated header: %hhx\n", r->buffer[0]);
    printf("record length: %zu bytes\n", r->length);
    printf("type length: %hhu bytes\n", r->type_length);
    printf("type offset: %zu bytes\n", r->type_offset);
    printf("id length: %hhu bytes\n", r->id_length);
    printf("id offset: %zu bytes\n", r->id_offset);
    printf("payload length: %d bytes\n", r->payload_length);
    printf("payload offset: %zu bytes\n", r->payload_offset);

    char* type = malloc(r->type_length);
    strncpy(type, r->buffer + r->type_offset, r->type_length);
    printf("\ntype: %s\n", type);

    char* payload = malloc(r->payload_length);
    strncpy(payload, r->buffer + r->payload_offset, r->payload_length);
    printf("payload: %s\n", payload);

    return 0;
}
