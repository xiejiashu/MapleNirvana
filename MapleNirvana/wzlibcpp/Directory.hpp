#pragma once

#include "Node.hpp"
#include "NumTypes.hpp"

namespace wz {
    class Directory : public Node {
    public:
        explicit Directory(File* root_file, bool img, int new_size, int new_checksum, unsigned int new_offset);

        [[nodiscard]]
        bool children_parsed() const;

        void mark_children_parsed();

        [[nodiscard]]
        u32 get_offset() const;

        [[nodiscard]]
        bool is_image() const;

        [[maybe_unused]]
        bool parse_image(Node* node);

    private:
        bool image;
        int size;
        int checksum;
        unsigned int offset;

        bool childrenParsed = false;
    };
}
