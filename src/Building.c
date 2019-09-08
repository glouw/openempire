#include "Building.h"

Building Building_Make(const Point cart, const Graphics file, const Color color)
{
    static Building zero;
    Building building = zero;
    building.trait = Trait_Build(file);
    building.cart = cart;
    building.color = color;
    building.file = file;
    building.health = building.trait.max_health;
    return building;
}
