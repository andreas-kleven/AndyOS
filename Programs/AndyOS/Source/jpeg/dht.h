#pragma once
#include <sys/types.h>

#define DHT_PREFIX_MAP_SIZE 256

struct DHT_NODE;
struct DHT_CODE;

struct DHT_PREFIX_ENTRY
{
    uint8_t value;
    uint8_t size;
};

struct DHT_TABLE
{
    DHT_TABLE();
    ~DHT_TABLE();

    inline DHT_PREFIX_ENTRY *GetValue8(uint8_t input) { return &prefix_map[input]; }

    bool Next(int bit);
    inline uint8_t GetValue() { return current_value; }

    static bool Parse(void *data, size_t length, DHT_TABLE *tables[8]);

  private:
    uint8_t current_value = 0;
    DHT_NODE *root_node = 0;
    DHT_NODE *current_node = 0;
    DHT_PREFIX_ENTRY prefix_map[DHT_PREFIX_MAP_SIZE];

    void FreeNode(DHT_NODE *node);
    void UpdateMap(DHT_NODE *node);
    static DHT_TABLE *ConstructTable(DHT_CODE **codes, size_t count);
};
