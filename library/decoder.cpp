//
// Created by Yaroslav239 on 20.08.18.
//

#include "decoder.h"

decoder::decoder(const std::string &src, const std::string &dst) : basic_coder(src, dst, 1024 * 1024) {}

void decoder::decode() {
    read_dictionary();
    decompress();
}

void decoder::decompress() {
    std::vector<char> buffer(buffer_size);
    bit_sequence bs(9);
    while (src_file) {
        src_file.read(buffer.data(), buffer_size);
        auto decompressed = h_tree->decompress(buffer.data(), static_cast<size_t>(src_file.gcount()), bs, !src_file);
        dst_file.write((char *) decompressed->get_data(), decompressed->size());
    }
    if (bs.bit_size() > 0)
        throw std::runtime_error("decoded file is invalid");
}

void decoder::read_dictionary() {
    std::vector<char> tmp(1);
    src_file.read(tmp.data(), 1);
    size_t MAX_DICT_SIZE = 256 * 9;
    size_t dict_size = tree::char_to_u<>(tmp[0]) * 9 + 1;
    if (dict_size == 1)
        dict_size += MAX_DICT_SIZE;
    std::vector<char> buffer(dict_size);
    src_file.read(buffer.data(), dict_size);
    if (src_file.gcount() == 1 && dict_size == MAX_DICT_SIZE + 1)
        dict_size = 1;
    if (dict_size > 1 && !src_file)
        throw std::runtime_error("decoded file is invalid");
    h_tree->build_by_freq_dict(buffer.data(), dict_size);
}
