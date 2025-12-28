#pragma once

#include "Node.hpp"
#include "Reader.hpp"
#include "Wz.hpp"
#include "Keys.hpp"
#include <string>
#include <filesystem>

namespace wz
{
    class File final
    {

    public:
        [[maybe_unused]] explicit File(const std::initializer_list<u8> &new_iv, const char *path);

        [[maybe_unused]] explicit File(u8 *new_iv, const char *path);

        ~File();

        [[maybe_unused]] bool parse(const wzstring &name = u"");

        [[maybe_unused]] [[nodiscard]] Node *get_root() const;
        Node &get_child(const wzstring &name);

        [[nodiscard]] std::string get_base_path() const;

        MutableKey key;

    private:
        // u8* key;
        u8 *iv;

        Node *root;

        Description desc{};

        Reader reader;

        std::string base_path;

        bool parse_directories(Node *node);

    #ifndef __EMSCRIPTEN__
        bool parse_directories_from_fs(Node *node, const std::filesystem::path &dir_path);
    #endif

        u32 get_wz_offset();

        void init_key();

        friend class Node;
    };
}
