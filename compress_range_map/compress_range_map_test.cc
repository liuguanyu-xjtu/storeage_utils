
#include <cstdint> // for uint64_t
#include <string_view>

#include "compress_range_map/bits_stream.h"
#include "gtest/gtest.h"

namespace zeta {
namespace unittest {

TEST(BitOp, basic) {
    size_t len = 10 * 1024;
    std::unique_ptr<char> buf(new char[len]);
    BitsAppend append(buf.get(), len);


    append.Append(32, 8);
    append.Append(true);
    append.Append(342, 16);

    auto offset = append.BitSize();

    append.Append(16, 8);
    append.Append(true);
    append.Append(322, 16);

    auto len1 = append.ByteSize();
    BitsReader r(buf.get(), append.BitSize());
    r.Seek(offset);

    uint32_t v1, v2;
    bool f;
    EXPECT_EQ(r.Read(8, &v1), true);
    EXPECT_EQ(r.ReadBool(&f), true);
    EXPECT_EQ(r.Read(16, &v2), true);

    EXPECT_EQ(v1, 16);
    EXPECT_EQ(f, true);
    EXPECT_EQ(v2, 322);

}

}
}
