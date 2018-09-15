//
// Created by matveich on 16.06.18.
//


#include <cmath>
#include <fstream>
#include <library/encoder.h>
#include <library/decoder.h>
#include "gtest/gtest.h"
#include "tree.h"


void check_eq(const std::string& data) {
    tree t;
    t.update_dict(data.data(), data.size());
    t.build();
    bit_sequence offset(1), offset1(1);
    auto compressed = t.compress(data.data(), data.size(), offset);
    if (offset.size() > 0)
        compressed->append(offset);
    EXPECT_TRUE(compressed->size() <= data.size());

    tree t1;
    std::vector<char> dict = t.get_dict();
    t1.build_by_freq_dict(dict.data() + 1, dict.size() - 1);
    auto decompressed = t1.decompress((char*)compressed->get_data(), compressed->size(), offset1, true);
    EXPECT_EQ(data, std::string((char*)(decompressed->get_data()), decompressed->size()));
}


TEST(correctness, basic) {
    check_eq("abc");
}


TEST(correctness, empty) {
    check_eq("");
}

TEST(correctness, one) {
    check_eq("1");
}

TEST(correctness, back_slash_n) {
    check_eq("\n");
}

TEST(correctness, back_slash_n_plus_char) {
    check_eq("\na");
}

TEST(correctness, chars_back_slash_n_plus_chars) {
    check_eq("ajtjglt\nnln");
}

TEST(correctness, chars_tabs) {
    check_eq("a\tjtj\tglt\tnln\t");
}

TEST(correctness, spaces) {
    check_eq("       ");
}

TEST(correctness, one_hundred_a) {
    std::string input(100, 'a');
    check_eq(input);
}

TEST(correctness, small_fuzzing_a_z) {
    srand(0);
    for (int i = 0; i < 100; ++i) {
        std::string input;
        for (int j = 0; j < 100; ++j) {
            input += ('a' + static_cast<unsigned char>(rand() % 26));
        }
        check_eq(input);
    }
}

TEST(correctness, small_fuzzing_0_128) {
    srand(0);
    for (int i = 0; i < 100; ++i) {
        std::string input;
        for (int j = 0; j < 100; ++j) {
            input += static_cast<char>(rand() % 128);
        }
        check_eq(input);
    }
}

TEST(correctness, critical_size) {
    size_t CZ = 8 * 1024 * 1024;
    check_eq(std::string(CZ, 'a'));
}

TEST(correctness, critical_size_random) {
    size_t CZ = 8 * 1024 * 1024;
    std::string s;
    s.resize(CZ);
    for (auto &c : s) {
        c = static_cast<char>(rand() % 128);
    }
    check_eq(s);
}

void cmp_files(const std::string& a_name, const std::string& b_name) {
    std::ifstream a(a_name), b(b_name);
    EXPECT_TRUE(a.is_open());
    EXPECT_TRUE(b.is_open());

    size_t buffer_size = 1024 * 128;
    auto buffer_a = std::unique_ptr<char[]>(new char[buffer_size]);
    auto buffer_b = std::unique_ptr<char[]>(new char[buffer_size]);
    while (a) {
        a.read(buffer_a.get(), buffer_size);
        b.read(buffer_b.get(), buffer_size);
        EXPECT_EQ(a.eof(), b.eof());
        EXPECT_EQ(a.gcount(), b.gcount());
        size_t size = static_cast<size_t>(a.gcount());
        for (size_t i = 0; i < size; ++i)
            EXPECT_EQ(buffer_a[i], buffer_b[i]);
    }
}

void check_eq_file(const std::string& src_name) {
    std::string dst_name = src_name + "_";
    {
        encoder e(src_name, "encoded");
        e.encode();
    }
    {
        decoder d("encoded", dst_name);
        d.decode();
    }

    std::remove("encoded");

    cmp_files(src_name, dst_name);
    std::remove(dst_name.c_str());
}

/*
TEST(file_correctness, empty) {
    check_eq_file("../tests/files/empty");
}

TEST(file_correctness, simple_text_file) {
    check_eq_file("../tests/files/test1.txt");
}

TEST(file_correctness, text_file) {
    check_eq_file("../tests/files/prestuplenie_i_nakazanie.pdf");
}

TEST(file_correctness, big_text_file) {
    check_eq_file("../tests/files/shildt.pdf");
}

TEST(file_correctness, small_video_file) {
    check_eq_file("../tests/files/02.avi");
}

TEST(file_correctness, big_video_file) {
    check_eq_file("../tests/files/linch_geniy.mp4");
}
*/