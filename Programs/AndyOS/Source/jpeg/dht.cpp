#include "dht.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define DHT_NUM_CODES 16

struct DHT_NODE
{
    bool leaf = false;
    DHT_NODE *parent = 0;
    DHT_NODE *left = 0;
    DHT_NODE *right = 0;
    DHT_NODE *next_leaf = 0;
    uint8_t value = 0;
};

struct DHT_CODE
{
    DHT_CODE *next;
    uint8_t value;
};

DHT_TABLE::DHT_TABLE()
{
    memset(prefix_map, 0, sizeof(prefix_map));
}

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
        size_t code_count = 0;

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
                code_count = i + 1;
            }
        }

        ptr = code_ptr;

        if (tables[type_index])
            delete tables[type_index];

        DHT_TABLE *table = ConstructTable(codes, code_count);
        tables[type_index] = table;

        for (int i = 0; i < DHT_NUM_CODES; i++) {
            DHT_CODE *code = codes[i];

            while (code) {
                DHT_CODE *next = code->next;
                delete code;
                code = next;
            }
        }
    }

    return true;
}

void DHT_TABLE::UpdateMap(DHT_NODE *node)
{
    int size = 0;
    uint8_t index = 0;
    DHT_NODE *cur = node;

    while (cur && cur->parent) {
        index >>= 1;

        if (cur == cur->parent->right)
            index |= 1 << 7;

        cur = cur->parent;
        size += 1;

        if (size > 8)
            return;
    }

    for (int i = 0; i < (1 << (8 - size)); i++) {
        if (prefix_map[index + i].size) {
            fprintf(stderr, "Already mapped\n");
            fprintf(stderr, "%d %d %x\n", index + i, size, node->value);
        }

        prefix_map[index + i].size = size;
        prefix_map[index + i].value = node->value;
    }
}

DHT_TABLE *DHT_TABLE::ConstructTable(DHT_CODE **codes, size_t count)
{
    DHT_TABLE *table = new DHT_TABLE();
    DHT_NODE *root = new DHT_NODE();
    DHT_NODE *first_leaf = root;

    for (int i = 0; i < count; i++) {
        DHT_CODE *code = codes[i];
        DHT_NODE *node;

        DHT_NODE *leaf = first_leaf;
        DHT_NODE *prev_right = 0;

        while (leaf) {
            leaf->left = new DHT_NODE();
            leaf->right = new DHT_NODE();

            leaf->left->parent = leaf;
            leaf->right->parent = leaf;

            leaf->left->next_leaf = leaf->right;

            if (leaf == first_leaf)
                first_leaf = leaf->left;
            else if (prev_right)
                prev_right->next_leaf = leaf->left;

            prev_right = leaf->right;
            leaf = leaf->next_leaf;
        }

        while (code) {
            if (!first_leaf) {
                fprintf(stderr, "No node available %d\n", i + 1);
                return 0;
            }

            first_leaf->leaf = true;
            first_leaf->value = code->value;

            table->UpdateMap(first_leaf);

            first_leaf = first_leaf->next_leaf;
            code = code->next;
        }
    }

    table->root_node = root;
    table->current_node = root;
    return table;
}
