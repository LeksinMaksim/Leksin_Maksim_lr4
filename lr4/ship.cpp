#include "ship.h"

#include <stdexcept>

Ship::Ship(int length, Orientation orientation)
    : length(length), orientation(orientation), valid(true)
{
    if (length < MIN_LENGTH || length > MAX_LENGTH)
    {
        valid = false;
        this->length = 0;
    }
    else
        segmentDamages.resize(length, SegmentStatus::Intact);
}

bool Ship::isValid() const
{
    return valid;
}

Ship::~Ship() = default;

int Ship::getLength() const
{
    return length;
}

Orientation Ship::getOrientation() const
{
    return orientation;
}

SegmentStatus Ship::getSegmentStatus(int index) const
{
    if (index < 0 || index >= length)
        throw std::out_of_range("Segment index out of range.");

    return segmentDamages[index];
}

void Ship::applyDamage(int index, int damage)
{
    if (index < 0 || index >= length)
        throw std::out_of_range("Segment index out of range.");

    if (damage < 0)
        throw std::invalid_argument("Damage cannot be negative.");

    int currentDamage = static_cast<int>(segmentDamages[index]) + damage;

    if (currentDamage >= static_cast<int>(SegmentStatus::Destroyed))
    {
        segmentDamages[index] = SegmentStatus::Destroyed;
    }
    else if (currentDamage >= static_cast<int>(SegmentStatus::Damaged))
    {
        segmentDamages[index] = SegmentStatus::Damaged;
    }
}

bool Ship::isSunk() const
{
    for (const auto& status : segmentDamages)
    {
        if (status != SegmentStatus::Destroyed)
            return false;
    }
    return true;
}
