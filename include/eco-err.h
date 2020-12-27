#pragma once

#include <stdint.h>
#include <stdio.h>

typedef uint16_t esp_status;

#define HARDRESET_STATUS 0x00
#define LINKED_STATUS 0x01
#define CONN_STATUS 0x02
#define DOWNLOADED_STATUS 0x03
#define ONGOING_STATUS 0x04
#define ENDED_STATUS 0x05
#define BUG_STATUS 0xFF
