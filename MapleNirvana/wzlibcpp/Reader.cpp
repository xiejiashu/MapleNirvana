#include <cassert>
#include <vector>
#include <codecvt>
#include "Reader.hpp"
#include "Keys.hpp"

#ifdef __EMSCRIPTEN__
#include "Emscripten.hpp"

wz::Reader::Reader(wz::MutableKey &new_key, const char *file_path)
    : cursor(0), key(new_key), url(file_path)
{
}

wz::Reader::Reader(wz::MutableKey &new_key, unsigned char *data, size_t size)
    : cursor(0), key(new_key), buffer_data(data), buffer_size(size)
{
}

mio::mmap_source::size_type wz::Reader::size() const
{
    return 0;
}
#else
wz::Reader::Reader(wz::MutableKey &new_key, const char *file_path)
    : cursor(0), key(new_key), path(file_path ? file_path : "")
{
    std::error_code error_code;
    mmap = mio::make_mmap_source<decltype(file_path)>(file_path, error_code);

    if (error_code)
    {
        std::cerr << "\x1b[31m"
                  << "[wz][error] mmap failed: path=" << path
                  << " code=" << error_code.value() << " msg=" << error_code.message()
                  << "\x1b[0m" << std::endl;
    }
}

mio::mmap_source::size_type wz::Reader::size() const
{
    return mmap.size();
}
#endif

u8 wz::Reader::read_byte()
{
    return read<u8>();
}

[[maybe_unused]] std::vector<u8> wz::Reader::read_bytes(const size_t &len)
{
    std::vector<u8> result;
    result.reserve(len);

    for (size_t i = 0; i < len; ++i)
    {
        result.push_back(read_byte());
    }

    return result;
}

wz::wzstring wz::Reader::read_string()
{
    wz::wzstring result{};

    while (true)
    {
        auto c = static_cast<char>(read_byte());
        if (!c)
            break;
        result.push_back(c);
    }

    return result;
}

wz::wzstring wz::Reader::read_string(const size_t &len)
{
    wz::wzstring result{};

    for (int i = 0; i < len; ++i)
    {
        result.push_back(read_byte());
    }

    return result;
}

void wz::Reader::set_position(const size_t &size)
{
    cursor = size;
}

size_t wz::Reader::get_position() const
{
    return cursor;
}

void wz::Reader::skip(const size_t &size)
{
    cursor += size;
}

i32 wz::Reader::read_compressed_int()
{
    i32 result = static_cast<i32>(read<i8>());
    if (result == INT8_MIN)
        return read<i32>();
    return result;
}

i16 wz::Reader::read_i16()
{
    i16 result = static_cast<i16>(read<i16>());
    return result;
}

wz::wzstring wz::Reader::read_wz_string()
{
    auto len8 = read<i8>();

    if (len8 == 0)
        return {};

    i32 len;

    if (len8 > 0)
    {
        u16 mask = 0xAAAA;

        len = len8 == 127 ? read<i32>() : len8;

        if (len <= 0)
        {
            return {};
        }

        wz::wzstring result{};

        for (int i = 0; i < len; ++i)
        {
            auto encryptedChar = read<u16>();
            encryptedChar ^= mask;
            encryptedChar ^= *reinterpret_cast<u16 *>(&key[2 * i]);
            result.push_back(encryptedChar);
            mask++;
        }

        return result;
    }

    u8 mask = 0xAA;

    if (len8 == -128)
    {
        len = read<i32>();
    }
    else
    {
        len = -len8;
    }

    if (len <= 0)
    {
        return {};
    }

    wz::wzstring result{};

    for (int n = 0; n < len; ++n)
    {
        u8 encryptedChar = read_byte();
        encryptedChar ^= mask;
        encryptedChar ^= key[n];
        result.push_back(static_cast<u16>(encryptedChar));
        mask++;
    }

    return result;
}

bool wz::Reader::is_wz_image()
{
    if (read<u8>() != 0x73)
        return false;
    if (read_wz_string() != u"Property")
        return false;
    if (read<u16>() != 0)
        return false;
    return true;
}

wz::wzstring wz::Reader::read_string_block(const size_t &offset)
{
    switch (read<u8>())
    {
    case 0:
        [[fallthrough]];
    case 0x73:
        return read_wz_string();
    case 1:
        [[fallthrough]];
    case 0x1B:
        return read_wz_string_from_offset(offset + read<u32>());
    default:
    {
        assert(0);
    }
    }
    return {};
}

wz::wzstring wz::Reader::read_wz_string_from_offset(const size_t &offset)
{
    auto prev = get_position();
    set_position(offset);
    auto result = read_wz_string();
    set_position(prev);
    return result;
}

void wz::Reader::set_key(wz::MutableKey &new_key)
{
    key = new_key;
}
