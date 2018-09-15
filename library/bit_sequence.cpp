//
// Created by Yaroslav239 on 20.08.18.
//

#include "bit_sequence.h"

bit_sequence::bit_sequence() {
    _bit_size = 0;
    _capacity = 0;
    data.resize(1);
}

bit_sequence::bit_sequence(size_t capacity) {
    _bit_size = 0;
    _capacity = capacity;
    data.resize(_capacity);
}

bit_sequence::~bit_sequence() = default;

bool bit_sequence::get_bit(size_t bit_pos) const {
    size_t _pos = bit_pos & 0b111;
    bit_pos >>= 3;
    return get_bit(data[bit_pos], _pos);
}

bit_sequence &bit_sequence::operator=(bit_sequence const &bs) {
    data = bs.data;
    _capacity = bs._capacity;
    _bit_size = bs._bit_size;
    return *this;
}

void bit_sequence::set(size_t bit_pos, bool val) {
    set_bit(data[bit_pos >> 3], (bit_pos & 0b111), val);
}

void bit_sequence::push(bool x) {
    set(_bit_size++, x);
}

void bit_sequence::pop_bit() {
    set(--_bit_size, false);
}

size_t bit_sequence::size() const {
    return _bit_size % 8 == 0 ? _bit_size / 8 : _bit_size / 8 + 1;
}

size_t bit_sequence::bit_size() {
    return _bit_size;
}

void bit_sequence::append(bit_sequence &bs) {
    size_t i = 0;
    size_t mod8 = 8 - (_bit_size & 0b111);
    data[_bit_size >> 3] += (bs.data[0] >> (8 - mod8));
    if (mod8 >= bs._bit_size) {
        _bit_size += bs._bit_size;
        return;
    } else {
        _bit_size += mod8;
        i = mod8;
    }
    mod8 = (i & 0b111);
    for (; i + 8 <= bs._bit_size; i += 8) {
        data[_bit_size >> 3] = bs.get_word(i >> 3, mod8);
        _bit_size += 8;
    }
    if (i == bs._bit_size) {
        return;
    }
    size_t j = i;
    data[_bit_size >> 3] += (bs.data[i >> 3] << mod8);
    i += 8 - mod8;
    if (i < bs._bit_size)
        data[_bit_size >> 3] += (bs.data[i >> 3] >> (8 - mod8));
    _bit_size += bs._bit_size - j;
}

uint8_t bit_sequence::get_word(size_t pos) {
    return data[pos];
}

uint8_t* bit_sequence::get_data() {
    return data.data();
}

uint8_t bit_sequence::get_word(size_t pos, size_t offset) {
    return (data[pos] << offset) + (data[pos + 1] >> (8 - offset));
}

void bit_sequence::set_byte(size_t pos, uint8_t val) {
    if (pos == size())
        _bit_size += 8;
    data[pos] = val;
}

void bit_sequence::clear() {
    for (size_t i = 0; i < _capacity; i++)
        data[i] = 0;
    _bit_size = 0;
}

std::string bit_sequence::to_string() {
    std::string res;
    for (size_t i = 0; i < _bit_size; i++) {
        res += get_bit<uint8_t>(data[i / 8], i % 8) ? "1" : "0";
    }
    return res;
}