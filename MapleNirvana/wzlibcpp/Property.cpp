#include <zlib.h>
#include "Property.hpp"
#include "Types.hpp"


// get ARGB4444 piexl,ARGB8888 piexl and others.....
template <>
std::vector<u8> wz::Property<wz::WzCanvas>::get_raw_data()
{
    WzCanvas canvas = get();

    std::vector<u8> data_stream;
    reader->set_position(canvas.offset);
    size_t end_offset = reader->get_position() + canvas.size;
    unsigned long uncompressed_len = canvas.uncompressed_size;
    u8 *uncompressed = new u8[uncompressed_len];
    if (!canvas.is_encrypted)
    {
        for (size_t i = 0; i < canvas.size; ++i)
        {
            data_stream.push_back(reader->read_byte());
        }
        uncompress(uncompressed, (unsigned long *)&uncompressed_len, data_stream.data(), data_stream.size());
    }
    else
    {
        auto wz_key = get_key();

        while (reader->get_position() < end_offset)
        {
            auto block_size = reader->read<i32>();
            for (size_t i = 0; i < block_size; ++i)
            {
                auto n = wz_key[i];
                data_stream.push_back(
                    static_cast<u8>(reader->read_byte() ^ n));
            }
        }
        uncompress(uncompressed, (unsigned long *)&uncompressed_len, data_stream.data(), data_stream.size());
    }

    std::vector<u8> pixel_stream(uncompressed, uncompressed + uncompressed_len);
    delete[] uncompressed;
    return pixel_stream;
}

// get Sound node raw data
template <>
std::vector<u8> wz::Property<wz::WzSound>::get_raw_data()
{
    WzSound sound = get();
    std::vector<u8> data_stream;

    reader->set_position(sound.offset);
    size_t end_offset = reader->get_position() + sound.size;

    while (reader->get_position() < end_offset)
    {
        data_stream.push_back(static_cast<u8>(reader->read_byte()));
    }
    return data_stream;
}

// get uol By uol node
template <>
wz::Node *wz::Property<wz::WzUOL>::get_uol()
{
    auto path = get().uol;
    auto uol_node = parent->find_from_path(path);
    while (uol_node->type == wz::Type::UOL)
    {
        path = dynamic_cast<wz::Property<wz::WzUOL> *>(uol_node)->get().uol;
        uol_node = uol_node->parent->find_from_path(path);
    }

    return uol_node;
}
