#include "Directory.hpp"

#ifdef __EMSCRIPTEN__
#include "Emscripten.hpp"
#include <ranges>

bool wz::Directory::parse_image(Node *node)
{
    auto url = "Img/" + std::string{this->path.begin(), this->path.end()};
    Emscripten::load_file(url);
    node->path = this->path;
    node->reader = new Reader(reader->key, Emscripten::file_data, Emscripten::file_size);
    this->reader = node->reader;
    // parse img
    if (reader->is_wz_image())
    {
        return parse_property_list(node, 0);
    }
    return true;
}
#else
bool wz::Directory::parse_image(Node *node)
{
    if (is_image())
    {
        node->reader = reader;
        node->path = this->path;
        const auto current_offset = get_offset();
        reader->set_position(current_offset);
        if (reader->is_wz_image())
        {
            return parse_property_list(node, current_offset);
        }
    }
    return false;
}
#endif
wz::Directory::Directory(File *root_file, bool img, int new_size, int new_checksum, unsigned int new_offset)
    : image(img), size(new_size), checksum(new_checksum), offset(new_offset), Node(img ? Type::Image : Type::Directory, root_file)
{
}

bool wz::Directory::children_parsed() const
{
    return childrenParsed;
}

void wz::Directory::mark_children_parsed()
{
    childrenParsed = true;
}

u32 wz::Directory::get_offset() const
{
    return offset;
}

bool wz::Directory::is_image() const
{
    return image;
}
