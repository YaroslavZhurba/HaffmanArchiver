#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <vector>
#include <iostream>
#include <queue>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <utility>
#include <memory>
#include "bit_sequence.h"

class tree {
public:
    typedef unsigned long long ull;

    tree();

    void update_dict(const char *data, size_t data_size);

    void build_by_freq_dict(const char *data, size_t data_size);

    void build();

    std::vector<char> get_dict();

    std::unique_ptr<bit_sequence> compress(const char *data, size_t data_size, bit_sequence &offset);

    std::unique_ptr<bit_sequence> decompress(const char *data,
                                             size_t data_size, bit_sequence &offset, bool is_last_chunk);

    ~tree();

    template<typename T = size_t>
    static char u_to_char(T x) {
        return static_cast<char>(x - 128u);
    }

    template<typename T = size_t>
    static T char_to_u(char x) {
        return static_cast<T>(x) + 128u;
    }

private:
    const size_t SIGMA_SIZE = 256;
    //const size_t CHAR_SIZE = sizeof(char);
    //const size_t ULL_SIZE = sizeof(ull);

    std::vector<ull> freq_dict;
    std::vector<bit_sequence> dict;
    std::vector<bit_sequence> bs_repr;

    char expected_offset;

    struct vertex {
        std::shared_ptr<vertex> left;
        std::shared_ptr<vertex> right;
        ull value;
        int key;

        vertex(std::shared_ptr<vertex>& l, std::shared_ptr<vertex>& r, ull val, int key = -1) :
                left(l),
                right(r),
                value(val),
                key(key)
        {}

        vertex(ull val, int key = -1) :
                value(val),
                key(key)
        {}

        ~vertex() = default;
    };

    std::shared_ptr<vertex> head;

    size_t max_tree_depth;

    void build_tree();

    void build_dict(std::shared_ptr<vertex>& v, bit_sequence &seq, size_t depth);

    char calc_data_size();

    std::vector<char> ull_to_char(ull x);

    void insert_ull(std::vector<char> &v, ull x);
};


#endif //HUFFMAN_TREE_H
