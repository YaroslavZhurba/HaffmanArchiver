//
// Created by Yaroslav239 on 20.08.18.
//

#include "tree.h"

tree::tree() {
    freq_dict.resize(SIGMA_SIZE);
    dict.resize(SIGMA_SIZE);
    bs_repr.resize(SIGMA_SIZE);
    expected_offset = 0;
    max_tree_depth = 0;

    for (size_t i = 0; i < SIGMA_SIZE; i++) {
        bs_repr[i] = bit_sequence(1);
        for (size_t j = 0; j < 8; j++) {
            bs_repr[i].push(((i >> (7 - j)) & 1) == 1); /// all codes of symbols
        }
    }
}

void tree::update_dict(const char *data, size_t data_size) {
    for (size_t i = 0; i < data_size; ++i)
        ++freq_dict[char_to_u(data[i])];
}

tree::~tree() = default;

void tree::build_tree() {
    auto comp = [](std::shared_ptr<vertex> &a, std::shared_ptr<vertex> &b) { return a->value > b->value; };
    std::priority_queue<std::shared_ptr<vertex>, std::vector<std::shared_ptr<vertex> >, decltype(comp)> q(comp);
    for (size_t i = 0; i < freq_dict.size(); i++) {
        if (freq_dict[i] > 0)
            q.push(std::make_shared<vertex>(freq_dict[i], static_cast<int>(i)));
    }
    if (q.size() == 0)
        return;
    else if (q.size() == 1) {
        q.push(std::make_shared<vertex>(0, q.top()->key));
    }
    while (q.size() > 1) {
        auto l = q.top();
        q.pop();
        auto r = q.top();
        q.pop();
        q.push(std::make_shared<vertex>(l, r, l->value + r->value));
    }
    head = q.top();
    q.pop();
}

void tree::build_dict(std::shared_ptr<vertex> &v, bit_sequence &seq, size_t depth) {
    if (v->key != -1) {
        dict[v->key] = seq;
        if (depth > max_tree_depth) {
            max_tree_depth = depth;
            if (max_tree_depth > SIGMA_SIZE)
                throw std::runtime_error("max tree depth is reached");
        }
        return;
    }
    seq.push(false);
    build_dict(v->left, seq, depth + 1);
    seq.set(seq.bit_size() - 1, true);
    build_dict(v->right, seq, depth + 1);
    seq.pop_bit();
}

void tree::build() {
    build_tree();
    if (!head)
        return;
    bit_sequence tmp(8);
    build_dict(head, tmp, 0);
}

std::vector<char> tree::ull_to_char(tree::ull x) {
    std::vector<char> result(8);
    for (size_t i = 0; i < 8; ++i)
        result[i] = u_to_char((x >> ((7 - i) * 8)) & 0xFF);
    return result;
}

char tree::calc_data_size() {
    ull data_size = 0;
    for (size_t i = 0; i < SIGMA_SIZE; ++i)
        data_size += freq_dict[i] * dict[i].bit_size();
    //std::cout << data_size << std::endl;
    return expected_offset = static_cast<char>((8 - data_size % 8) % 8);
}

void tree::insert_ull(std::vector<char> &v, tree::ull x) {
    auto char_val = ull_to_char(x);
    v.insert(v.end(), char_val.begin(), char_val.end());
}

std::vector<char> tree::get_dict() {
    // get actual _bit_size of freq dict
    int size = 0;
    for (size_t i = 0; i < freq_dict.size(); ++i) {
        if (freq_dict[i] > 0)
            size++;
    }
    std::vector<char> out_dict;
    out_dict.reserve(static_cast<size_t>(size) * 9 + 2);
    //out_dict.reserve(static_cast<size_t>(size) * 8 + 2);
    out_dict.push_back(u_to_char<int>(size));
    for (size_t i = 0; i < freq_dict.size(); i++) {
        if (freq_dict[i] != 0) {
            out_dict.push_back(u_to_char(i));
            insert_ull(out_dict, freq_dict[i]);
        }
    }
    out_dict.push_back(calc_data_size());
    return out_dict;
}

void tree::build_by_freq_dict(const char *data, size_t data_size) {
    size_t i = 0;
    for (; i + 9 <= data_size; i += 9) {
        size_t ind = char_to_u(data[i]);
        for (size_t j = 0; j < 8; j++) {
            freq_dict[ind] += (char_to_u<ull>(data[i + 1 + j]) << (56 - 8 * j));
        }
    }
    expected_offset = data[i];
    build();
}

std::unique_ptr<bit_sequence> tree::compress(const char *data, size_t data_size, bit_sequence &offset) {
    auto compressed_data = std::make_unique<bit_sequence>(data_size * max_tree_depth / 8 + 2);
    compressed_data->append(offset);
    offset.clear();
    for (size_t i = 0; i < data_size; i++)
        compressed_data->append(dict[char_to_u(data[i])]);
    for (size_t i = 0; i < compressed_data->bit_size() % 8; ++i) {
        auto c = compressed_data->get_word(compressed_data->size() - 1);
        offset.push(bit_sequence::get_bit<uint8_t>(c, i));
    }

    while (compressed_data->bit_size() % 8 != 0)
        compressed_data->pop_bit();
    return compressed_data;
}

std::unique_ptr<bit_sequence> tree::decompress(const char *data, size_t data_size,
                                               bit_sequence &offset, bool is_last_chunk) {
    size_t bit_size = data_size * 8, result_pos = 0;
    auto result = std::make_unique<bit_sequence>(bit_size * 8 + 10);
    if (!head)
        return result;
    if (is_last_chunk)
        bit_size -= expected_offset;
    auto p = head;
    for (size_t i = 0; i < offset.bit_size(); i++)
        p = (offset.get_bit(i) ? p->right : p->left);
    offset.clear();
    for (size_t i = 0; i < bit_size; ++i) {
        if (p->key != -1) {
            result->set_byte(result_pos++, bs_repr[p->key].get_data()[0] ^ (uint8_t) 128u);
            p = head;
        }
        if (bit_sequence::get_bit(data[i >> 3], i & 0b111))
            p = p->right;
        else
            p = p->left;
    }
    if (p->key != -1) {
        result->set_byte(result_pos, bs_repr[p->key].get_data()[0] ^ (uint8_t) 128u);
    } else {
        size_t c = 0;
        while (p->left) {
            c++;
            p = p->left;
        }
        for (size_t i = 0; i < dict[p->key].bit_size() - c; i++) {
            offset.push(dict[p->key].get_bit(i));
        }
    }
    return result;
}