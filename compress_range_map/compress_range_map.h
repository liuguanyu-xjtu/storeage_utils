#include <iostream>


#include <string>
#include <cstdint> // for uint64_t
#include <vector>

namespace zeta {

struct CompressKeyInfo {
    uint32_t entry_offset_;
    union {
        struct {
            uint32_t bit_pos_ : 3;
            uint32_t offset_ : 29;
        };
        uint32_t bit_offset_;
    };
};

//class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator> {
//public:
//    virtual char* allocate(size_t size) {
//        return reinterpret_cast<char *>(malloc(size));
//    }
//
//    virtual void deallocate(void* buf, size_t len) {
//        free(buf);
//    }
//};
//
//struct Range {
//    uint32_t len_ = 0;
//    uint32_t logical_offset_ = 0;
//    uint64_t global_offset_ = 0;
//
//    bool operator < (const Range& rhs) const {
//        auto& lhs = *this;
//        return lhs.logical_offset_ < rhs.logical_offset_;
//    }
//};
//
//
//class CompressRangeMap : public std::enable_shared_from_this<MemoryAllocator> {
//public:
//    bool LookUp(uint32_t offset, uint32_t len, std::vector<Range>* result) const;
//
//    class Iterator;
//    class Builder;
//
//private:
//    CompressRangeMap() {}
//
//    struct CompressKeyInfo {
//        uint32_t entry_offset_;
//        union {
//            struct {
//                uint32_t bit_pos_ : 3;
//                uint32_t offset_ : 29;
//            };
//            uint32_t bit_offset_;
//        };
//    };
//
//    size_t entry_size_ = 4096;
//    uint8_t global_offset_width_ = 64;
//    uint8_t logical_offset_width_ = 64;
//
//    const char* raw_data_buf_ = nullptr;
//    uint32_t raw_data_len_ = 0;
//
//    std::vector<CompressKeyInfo> key_infos_;
//
//    std::shared_ptr<MemoryAllocator> allocator_;
//};
//
//class CompressRangeMap::Iterator {
//public:
//    virtual void SeekToFirst() = 0;
//
//    virtual bool Valid() = 0;
//
//    virtual Range Value() const = 0;
//
//    virtual void MoveNext() = 0;
//};
//
//class MergeIterator : CompressRangeMap::Iterator {
//public:
//    MergeIterator(CompressRangeMap::Iterator* f, CompressRangeMap::Iterator* s) {
//        first_level_ =  f;
//        second_level_ = s;
//    }
//
//    void SeekToFirst() override {
//        first_level_->SeekToFirst();
//        second_level_->SeekToFirst();
//        commit_pos_ = 0;
//        record_ = Record();
//        MoveNext();
//    }
//
//    bool Valid() override {
//        return record_.is_valid_;
//    }
//
//    Range Value() const override {
//        return record_.range_;
//    }
//
//    void MoveNext() override {
//        if (record_.is_valid_) {
//            commit_pos_ = record_.range_.LogicalEnd();
//        }
//
//        Record first_record = getRecord(first_level_, commit_pos_);
//        Record second_record = getRecord(second_level_, commit_pos_);
//
//        if (!first_record.is_valid_ && !second_record.is_valid_) {
//            record_ = Record();
//            return;
//        }
//
//        if (!first_record.is_valid_ || !second_record.is_valid_) {
//            if (first_record.is_valid_ ) {
//                record_ = first_record;
//            } else {
//                record_ = second_record;
//            }
//            return;
//        }
//
//        if (first_record.range_.LogicalBegin() <= second_record.range_.LogicalBegin()) {
//            record_ = first_record;
//        } else {
//            record_ = second_record;
//            auto len = std::min<uint32_t>(second_record.range_.len_,
//                    first_record.range_.LogicalBegin() - second_record.range_.LogicalBegin());
//            record_.range_ = record_.range_.Sub(len);
//        }
//    }
//
//private:
//    struct Record {
//        bool is_valid_ = false;
//        Range range_;
//    };
//
//    static Record getRecord(CompressRangeMap::Iterator* t, uint64_t commit_pos) {
//        while (true) {
//            if (!t->Valid()) {
//                return Record(); // Not valid
//            }
//            Record r;
//            r.is_valid_ = true;
//            r.range_ = t->Value();
//            if (r.range_.LogicalEnd() <= commit_pos) {
//                t->MoveNext();
//                continue;
//            }
//            uint32_t diff = commit_pos - r.range_.LogicalBegin();
//            r.range_.AddDelta(diff);
//            return r;
//        }
//    }
//
//    CompressRangeMap::Iterator* first_level_ = nullptr;
//    CompressRangeMap::Iterator* second_level_ = nullptr;
//
//    uint32_t commit_pos_ = 0;
//    Record record_;
//};
//
//
//class CompressRangeMap::Builder {
//public:
//    std::shared_ptr<CompressRangeMap> Merge(std::shared_ptr<MemoryAllocator> allocator,
//                                            size_t entry_size,
//                                            CompressRangeMap::Iterator* first_level,
//                                            CompressRangeMap::Iterator* second_level) {
//        uint32_t max_logical_offset = 0;
//        uint64_t max_global_offset = 0;
//        uint64_t max_empty_cnt = 0;
//        size_t range_cnt = 0;
//        size_t empty_cnt = 0;
//        {
//            MergeIterator m(first_level, second_level);
//            m.SeekToFirst();
//            while (true) {
//                if (!m.Valid()) {
//                    break;
//                }
//                Range r = m.Value();
//                max_global_offset = std::max(max_global_offset, r.global_offset_);
//                max_logical_offset = std::max(max_logical_offset, r.logical_offset_);
//                range_cnt++;
//                m.MoveNext();
//            }
//        }
//        uint8_t global_offset_width = bit_length(max_global_offset);
//        uint8_t logical_offset_width = bit_length(max_logical_offset);
//
//
//    }
//
//    int bit_length(uint64_t n) {
//        if (n == 0) return 1;
//        int bits = 0;
//        while (n != 0) {
//            n >>= 1;
//            bits++;
//        }
//        return bits;
//    }
//};


class BitsAppend {
public:
    BitsAppend(char* buf, size_t len);

    void Append(uint64_t v, uint8_t width);

    void Append(bool v);

    size_t Size();

private:
    char* buf_ = nullptr;
    size_t len_ = 0;
    uint32_t bit_offset_ = 0;
};

class BitsReader {
public:
    BitsReader(const char* buf, size_t len);

    uint32_t Read(uint8_t width);

    bool ReadBool();

    void Seek(uint32_t bit_offset);
};


}  // namespace zeta


