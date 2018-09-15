#ifndef HUFFMAN_BITSEQ_H
#define HUFFMAN_BITSEQ_H

#include <iostream>
#include <cassert>
#include <utility>
#include <memory>
#include <vector>

class bit_sequence {
public:
    bit_sequence();

    bit_sequence(size_t capacity);

    bool get_bit(size_t bit_pos) const;

    bit_sequence &operator=(bit_sequence const &);

    void set(size_t bit_pos, bool val);

    void push(bool x);

    void pop_bit();

    size_t bit_size();

    size_t size() const;

    void append(bit_sequence &bs);

    uint8_t get_word(size_t pos);

    uint8_t get_word(size_t pos, size_t offset);

    void set_byte(size_t pos, uint8_t val);

    void clear();

    template<typename T>
    static bool get_bit(T& c, size_t pos) {
        return ((c >> (7 - pos)) & 1) == 1;
    }

    template<typename T>
    static void set_bit(T &c, size_t pos, bool val) {
        if (val)
            c |= (1 << (7 - pos));
        else
            c &= ~(1 << (7 - pos));
    }

    uint8_t* get_data();

    std::string to_string();

    ~bit_sequence();

private:
    size_t _bit_size, _capacity;
    std::vector<uint8_t> data;
};


#endif //HUFFMAN_BITSEQ_H
