#pragma once

#include "Property.hpp"

void load_background(wz::Node *node, int id);
enum
{
    NORMAL,
    HTILED,
    VTILED,
    TILED,
    HMOVEA,
    VMOVEA,
    HMOVEB,
    VMOVEB
};