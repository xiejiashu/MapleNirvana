#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Wz.hpp"
#include "Reader.hpp"
#include "Types.hpp"

namespace wz
{

    class Node;
    class File;

    typedef std::vector<Node *> WzList;
    typedef std::unordered_map<wzstring, WzList> WzMap;

    class Node
    {
    public:
        explicit Node();
        explicit Node(const Type &new_type, File *root_file);

        virtual ~Node();

        Node &operator[](const wzstring &name);

        virtual void appendChild(const wzstring &name, Node *node);

        Node *get_child(const wzstring &name);

        Node *get_child(const std::string &name);

        [[maybe_unused]] [[nodiscard]] virtual const WzMap &get_children() const;

        [[maybe_unused]] [[nodiscard]] virtual Node *get_parent() const;

        [[nodiscard]] [[maybe_unused]] size_t children_count() const;

        [[maybe_unused]] WzMap::iterator begin();

        [[maybe_unused]] WzMap::iterator end();

        [[maybe_unused]] [[nodiscard]] Type get_type() const;

        [[nodiscard]] bool is_property() const;

        Node *find_from_path(const std::u16string &path);

        Node *find_from_path(const std::string &path);

    public:
        Type type;

        Node *parent;
        WzMap children;

        File *file;
        Reader *reader = nullptr;

        std::u16string path = u"";

        bool parse_property_list(Node *target, size_t offset);
        void parse_extended_prop(const wzstring &name, Node *target, const size_t &offset);
        WzCanvas parse_canvas_property();
        WzSound parse_sound_property();

        [[nodiscard]] u8 *get_iv() const;
        [[nodiscard]] wz::MutableKey &get_key() const;

        friend class Directory;
    };

}