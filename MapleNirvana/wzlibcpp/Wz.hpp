#pragma once

#include "Types.hpp"

#define U8 static_cast<u8>
#define IV4(A, B, C, D) \
    {                   \
        U8(A), U8(B), U8(C), U8(D)}

//////////////////////////////////////////////////////////////////////////

namespace wz
{
    enum class Type : u8
    {
        NotSet = 0x00,
        Directory = 0x10,
        Image = 0x20,
        Property = 0x30,

        Null = 0x31,
        Int = 0x32,
        UnsignedShort = 0x33,
        Float = 0x34,
        Double = 0x35,
        String = 0x36,

        SubProperty = 0x37,
        Canvas = 0x38,
        Vector2D = 0x39,
        Convex2D = 0x3A,
        Sound = 0x3B,
        UOL = 0x3C,
    };

    constexpr u8 bit(const Type &type)
    {
        return static_cast<u8>(type);
    }

    namespace keys
    {
        [[maybe_unused]]
        const unsigned char gms[4] = {
            0x4D, 0x23, 0xC7, 0x2B};

        [[maybe_unused]]
        const unsigned char kms[4] = {
            0xB9, 0x7D, 0x63, 0xE9};
    }
}

//////////////////////////////////////////////////////////////////////////

[[deprecated]]
void WzGenKeys(const unsigned char *IV);

namespace wz
{

    struct Description
    {
        u32 start;
        u32 hash;
        i16 version;
    };

    u32 get_version_hash(i32 encryptedVersion, i32 realVersion);

    [[deprecated]]
    void initAES(const u8 *iv);

}