#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ndef.h"

struct ndef_record* ndef_parse(char* buffer, size_t offset) {
    int i, j;
    struct ndef_record* record = malloc(sizeof(struct ndef_record));
    if (record == NULL) {
        return NULL;
    }

    record->buffer = buffer;

    i = 1;
    record->type_length = buffer[i++];
    if (ndef_is_short_record(record)) {
        record->payload_length = buffer[i++];
    } else {
        record->payload_length = 0;
        for (j = 0; j < 4; ++j) {
            record->payload_length |= ((uint32_t) buffer[i++]) << ((3 - j) * 8);
        }
    }

    if (ndef_has_id_length(record)) {
        record->id_length = buffer[i++];
    } else {
        record->id_length = 0;
    }

    record->type_offset = i;
    record->id_offset = record->type_offset + record->type_length;
    record->payload_offset = record->id_offset + record->id_length;
    record->length = record->payload_offset + record->payload_length;

    return record;
}

struct ndef_record* ndef_create(
        uint8_t tnf, bool is_begin, bool is_end, bool is_chunk,
        bool is_short, bool has_id,
        char* type, uint8_t type_length,
        char* id, uint8_t id_length,
        char* payload, uint32_t payload_length) {
    int i, j;
    struct ndef_record* record = malloc(sizeof(struct ndef_record));
    if (record == NULL) {
        return NULL;
    }

    // one byte for header, one for type length, one or four for payload length,
    // zero or one for id length
    record->length = 2 + (is_short ? 1 : 4) + (has_id ? 1 : 0)
        + type_length + id_length + payload_length;
    record->buffer = malloc(sizeof(char) * record->length);
    if (record->buffer == NULL) {
        goto error;
    }

    record->type_length = type_length;
    record->id_length = has_id ? id_length : 0;
    record->payload_length = payload_length;

    if (tnf > 0x06) {
        goto error;
    }
    record->buffer[0] = tnf;

    if (is_begin) record->buffer[0] |= 0x80;
    if (is_end)   record->buffer[0] |= 0x40;
    if (is_chunk) record->buffer[0] |= 0x20;
    if (is_short) record->buffer[0] |= 0x10;
    if (has_id)   record->buffer[0] |= 0x08;

    i = 1;
    record->buffer[i++] = type_length;
    if (is_short) {
        if (payload_length > 1 << 7) {
            goto error;
        }
        record->buffer[i++] = (uint8_t) payload_length;
    } else {
        for (j = 0; j < 4; ++j) {
            record->buffer[i++] = 0xFF & (payload_length >> ((3 - j) * 8));
        }
    }
    if (has_id) {
        record->buffer[i++] = id_length;
    }

    record->type_offset = i;
    record->id_offset = record->type_offset + record->type_length;
    record->payload_offset = record->id_offset + record->id_length;
    record->length = record->payload_offset + record->payload_length;

    memcpy(record->buffer + record->type_offset, type, type_length);
    if (has_id && id_length > 0) {
        memcpy(record->buffer + record->id_offset, id, id_length);
    }
    memcpy(record->buffer + record->payload_offset, payload, payload_length);

    return record;
error:
    if (record->buffer != NULL) {
        free(record->buffer);
    }
    free(record);
    return NULL;
}

struct ndef_record* ndef_destroy_buffer(ndef_record*) {
    if (record->buffer != NULL) {
        free(record->buffer);
    }
    free(record);
}

struct ndef_record* ndef_destroy(ndef_record*) {
    free(record);
}

uint8_t ndef_tnf(struct ndef_record* record) {
    return record->buffer[0] & 0x03;
}

bool ndef_is_message_begin(struct ndef_record* record) {
    return record->buffer[0] & 0x80;
}

bool ndef_is_message_end(struct ndef_record* record) {
    return record->buffer[0] & 0x40;
}

bool ndef_is_chunk(struct ndef_record* record) {
    return record->buffer[0] & 0x20;
}

bool ndef_is_short_record(struct ndef_record* record) {
    return record->buffer[0] & 0x10;
}

bool ndef_has_id_length(struct ndef_record* record) {
    return record->buffer[0] & 0x08;
}
