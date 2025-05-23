#include <iostream>

#include <string>
#include <cstdint> // for uint64_t
#include <vector>

namespace zeta {

#include <cstdint>
#include <stdexcept>

class BitsAppend {
public:
    BitsAppend(char* buf, size_t len) : buf_(buf), len_(len), bit_offset_(0) {}

    void Append(uint64_t v, uint8_t width) {
        for (uint8_t i = 0; i < width; ++i) {
            bool bit = (v >> (width - 1 - i)) & 1;
            AppendBit(bit);
        }
    }

    void Append(bool v) {
        AppendBit(v);
    }

    size_t ByteSize() const {
        if (bit_pos_ != 0) {
            return offset_ + 1;
        }
        return offset_;
    }

    size_t BitSize() const {
        return bit_offset_;
    }

private:
    void AppendBit(bool bit) {
        size_t byte_pos = bit_offset_ / 8;
        uint8_t bit_pos = bit_offset_ % 8;

        if (bit) {
            buf_[byte_pos] |= (1 << (7 - bit_pos));
        } else {
            buf_[byte_pos] &= ~(1 << (7 - bit_pos));
        }
        ++bit_offset_;
    }

    char* buf_;
    size_t len_;
    union {
        struct {
            uint32_t bit_pos_ : 3;
            uint32_t offset_ : 29;
        };
        uint32_t bit_offset_;
    };
};

#include <cstdint>
#include <stdexcept>
#include <algorithm>

class BitsReader {
public:
    BitsReader(const char* buf, size_t bit_len)
        : buf_(reinterpret_cast<const uint8_t*>(buf)),
          total_bits_(bit_len),
          bit_pos_(0) {}

    // 读取指定位数到uint64_t (1-64 bits)
    bool Read(uint8_t width, uint64_t* value) {
        if (width == 0 || width > 64) return false;
        if (bit_pos_ + width > total_bits_) return false;

        uint64_t result = 0;
        size_t remaining_bits = width;

        while (remaining_bits > 0) {
            const size_t byte_idx = bit_pos_ / 8;
            const uint8_t bit_in_byte = bit_pos_ % 8;
            const uint8_t available_bits = 8 - bit_in_byte;
            const uint8_t read_bits = static_cast<uint8_t>(std::min<size_t>(remaining_bits, available_bits));

            // 提取当前字节中的有效位
            const uint8_t mask = (1 << read_bits) - 1;
            const uint8_t bits = (buf_[byte_idx] >> (available_bits - read_bits)) & mask;

            // 合并到结果
            result = (result << read_bits) | bits;
            bit_pos_ += read_bits;
            remaining_bits -= read_bits;
        }

        *value = result;
        return true;
    }

    // 读取指定位数到uint32_t (1-32 bits)
    bool Read(uint8_t width, uint32_t* value) {
        uint64_t tmp;
        if (!Read(width, &tmp) || tmp > UINT32_MAX) return false;
        *value = static_cast<uint32_t>(tmp);
        return true;
    }

    // 读取单个比特
    bool ReadBool(bool* value) {
        uint64_t tmp;
        if (!Read(1, &tmp)) return false;
        *value = (tmp != 0);
        return true;
    }

    // 跳转到指定比特位置
    void Seek(uint32_t bit_offset) {
        if (bit_offset > total_bits_) {
            throw std::out_of_range("Seek position exceeds buffer size");
        }
        bit_pos_ = bit_offset;
    }

    // 获取当前比特位置
    size_t Tell() const { return bit_pos_; }

    // 获取剩余可读比特数
    size_t RemainingBits() const { return total_bits_ - bit_pos_; }

private:
    const uint8_t* buf_;
    const size_t total_bits_;
    size_t bit_pos_;
};

}  // namespace zeta


