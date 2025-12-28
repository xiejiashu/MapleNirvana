#pragma once

#include <cmath>
#include <array>
#include <utility>
#include <iostream>
#include "Node.hpp"
#include "Keys.hpp"

namespace wz
{
    template <typename T>
    class Property : public Node
    {
    public:
        explicit Property(const Type &new_type, File *root_file) : Node(new_type, root_file) {}

        explicit Property(const Type &new_type, File *root_file, T new_data)
            : data(new_data), Node(new_type, root_file) {}

        void set(T new_data)
        {
            data = new_data;
        }

        [[maybe_unused]] const T &get() const
        {
            return data;
        }

        [[nodiscard]] [[maybe_unused]] std::vector<u8> get_raw_data();

        [[nodiscard]] [[maybe_unused]] wz::Node *get_uol();

    private:
        T data;
    };
}
