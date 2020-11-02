#pragma once
#include <sys/types.h>

struct DHT_NODE;
struct DHT_CODE;

struct DHT_TABLE
{
    ~DHT_TABLE();

    bool Next(int bit);
    inline uint8_t GetValue() { return current_value; }

    static bool Parse(void *data, size_t length, DHT_TABLE *tables[8]);

  private:
    uint8_t current_value = 0;
    DHT_NODE *root_node = 0;
    DHT_NODE *current_node = 0;

    void FreeNode(DHT_NODE *node);
    static DHT_TABLE *ConstructTable(DHT_CODE **codes, size_t count);
};
