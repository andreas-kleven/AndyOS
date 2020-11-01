#include "dht.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define DHT_NUM_CODES 16

struct DHT_NODE
{
    bool leaf = false;
    DHT_NODE *left = 0;
    DHT_NODE *right = 0;
    uint8_t value = 0;
};

struct DHT_CODE
{
    DHT_CODE *next;
    uint8_t value;
};

DHT_TABLE::~DHT_TABLE()
{
    FreeNode(root_node);
}

void DHT_TABLE::FreeNode(DHT_NODE *node)
{
    if (!node)
        return;

    if (!node->leaf) {
        FreeNode(node->left);
        FreeNode(node->right);
    }

    delete node;
}

bool DHT_TABLE::Next(int bit)
{
    if (!current_node)
        return false;

    if (bit)
        current_node = current_node->right;
    else
        current_node = current_node->left;

    if (current_node && current_node->leaf) {
        current_value = current_node->value;
        current_node = root_node;
        return true;
    }

    return false;
}

bool DHT_TABLE::Parse(void *data, size_t length, DHT_TABLE *tables[8])
{
    uint8_t *ptr = (uint8_t *)data;
    uint8_t *end = (uint8_t *)data + length;

    int total_count = 0;

    while (ptr < end) {
        if (*ptr == 0xFF && *(ptr + 1) != 0)
            break;

        DHT_CODE *codes[DHT_NUM_CODES];
        memset(codes, 0, sizeof(codes));

        uint8_t type = *ptr++;
        uint8_t type_index = ((type & 0x10) >> 4) | (type & 0x3) << 1;
        uint8_t *code_ptr = (uint8_t *)ptr + 16;

        for (int i = 0; i < DHT_NUM_CODES; i++) {
            uint8_t count = *ptr++;
            DHT_CODE *prev_code = 0;

            for (int j = 0; j < count && code_ptr < end; j++) {
                DHT_CODE *code = new DHT_CODE();
                code->next = 0;
                code->value = *code_ptr++;

                if (!codes[i])
                    codes[i] = code;

                if (prev_code)
                    prev_code->next = code;

                prev_code = code;
            }
        }

        ptr = code_ptr;

        DHT_TABLE *table = ConstructTable(codes);
        tables[type_index] = table;

        for (int i = 0; i < DHT_NUM_CODES; i++) {
            if (codes[i])
                delete codes[i];
        }
    }

    return true;
}

DHT_TABLE *DHT_TABLE::ConstructTable(DHT_CODE **codes)
{
    DHT_NODE *root = new DHT_NODE();

    for (int i = 0; i < DHT_NUM_CODES; i++) {
        DHT_CODE *code = codes[i];
        DHT_NODE *node;

        while ((node = FindAvailableNode(root, i))) {
            node->left = new DHT_NODE();
            node->right = new DHT_NODE();
        }

        while (code) {
            node = FindAvailableNode(root, i + 1);

            if (!node) {
                fprintf(stderr, "No node available %d\n", i + 1);
                return 0;
            }

            node->leaf = true;
            node->value = code->value;
            code = code->next;
        }
    }

    DHT_TABLE *table = new DHT_TABLE();
    table->root_node = root;
    table->current_node = root;
    return table;
}

DHT_NODE *DHT_TABLE::FindAvailableNode(DHT_NODE *root, int level)
{
    DHT_NODE *node;

    if (level == 0) {
        if (!root->leaf && !root->left && !root->right)
            return root;

        return 0;
    }

    if (root->left) {
        if ((node = FindAvailableNode(root->left, level - 1)))
            return node;
    }

    if (root->right) {
        if ((node = FindAvailableNode(root->right, level - 1)))
            return node;
    }

    return 0;
}
