#ifndef _REST_STRUCT_H
#define _REST_STRUCT_H

// holds restaurant data read from SD card
struct restaurant {
    int32_t lat;
    int32_t lon;
    // from 0 to 10
    uint8_t rating;
    char name[55];
};

#endif
