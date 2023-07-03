#include "bitset.h"
#include "chunked_label_pool.h"
#include "contiguous_label_pool.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>

struct Label
{
    double cost;                // 8 bytes
    int16_t time;               // 2 bytes
    int16_t load;               // 2 bytes
    unsigned char customers[0]; // 0 bytes
};
static_assert(sizeof(Label) == 8 + 2 + 2 + 4); // 4 bytes of padding

int main()
{
    {
        uint64_t bitset = 0;
        set_bitset(&bitset, 1);
        printf("%llu\n", bitset);
        printf("%d\n", get_bitset(&bitset, 1));
        printf("\n");
    }

    {
        ContiguousLabelPool contiguous_label_pool;
        contiguous_label_pool.reset(sizeof(Label));

        auto label = contiguous_label_pool.get_label_buffer();
        label->cost = 123;
        set_bitset(label->customers, 1);
        contiguous_label_pool.commit_label_in_buffer();

        printf("%f %d\n", label->cost, get_bitset(label->customers, 1));
        printf("\n");
    }

    {
        const auto num_customers = 64;
        size_t label_size = 8 + 2 + 2 + num_customers/8;
        label_size += (8 - label_size) % 8; // Increase to next multiple of 8 for padding
        printf("Label size %zu\n", label_size);

        ChunkedLabelPool chunked_label_pool;
        chunked_label_pool.reset(label_size);

        auto label = chunked_label_pool.get_label_buffer();
        label->cost = -123;
        set_bitset(label->customers, 63);
        chunked_label_pool.commit_label_in_buffer();

        printf("%f %d\n", label->cost, get_bitset(label->customers, 63));
        printf("\n");
    }

    fflush(stdout);
    return 0;
}
