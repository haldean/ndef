#ifndef __NDEF_NDEF_H__
#define __NDEF_NDEF_H__

#define NO_ID 0

#include <stdbool.h>
#include <stdint.h>

struct ndef_message {
    size_t length;
    struct ndef_record* records;
};

// A zero-copy data structure for storing ndef records. All fields should be
// accessed using accessors defined below.
struct ndef_record {
    char* buffer;
    size_t length;

    uint8_t type_length;
    size_t type_offset;

    uint8_t id_length;
    size_t id_offset;

    uint32_t payload_length;
    size_t payload_offset;
};

struct ndef_record* ndef_parse(char* buffer, size_t offset);
struct ndef_record* ndef_create(
        uint8_t tnf, bool is_begin, bool is_end, bool is_chunk,
        bool is_short, bool has_length,
        char* type, uint8_t type_length,
        char* id, uint8_t id_length,
        char* payload, uint32_t payload_length);

uint8_t ndef_tnf(struct ndef_record*);
bool ndef_is_message_begin(struct ndef_record*);
bool ndef_is_message_end(struct ndef_record*);
bool ndef_is_chunk(struct ndef_record*);
bool ndef_is_short_record(struct ndef_record*);
bool ndef_has_id_length(struct ndef_record*);

#endif
