#include <iostream>


#include <algorithm>

#include <wz/Node.hpp>
#include <wz/File.hpp>
#include <wz/Directory.hpp>
#include <wz/Property.hpp>

#include <iostream>
#include <fstream>

#define U8 static_cast<u8>
#define IV4(A, B, C, D)            \
    {                              \
        U8(A), U8(B), U8(C), U8(D) \
    }

int main()
{
    const auto iv = IV4(0xb9, 0x7d, 0x63, 0xe9);
    wz::File file(iv, "Map.wz");

    if (file.parse())
    {
        wz::Node *node=file.get_root()->find_from_path(u"Map/Map1/101000000.img");
        return 1;
    }
    return 0;
}