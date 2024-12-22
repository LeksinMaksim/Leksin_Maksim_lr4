#ifndef SHIP_H
#define SHIP_H

#include <sys/wait.h>

#include <vector>

enum class Orientation
{
    Horizontal,
    Vertical
};

enum class SegmentStatus
{
    Intact,
    Damaged,
    Destroyed
};

class Ship
{
public:
    Ship(int length, Orientation orientation);
    ~Ship();

    bool isValid() const;

    int getLength() const;
    Orientation getOrientation() const;
    SegmentStatus getSegmentStatus(int index) const;
    void applyDamage(int index, int damage);
    bool isSunk() const;

    void setOrientation(Orientation newOrientation) { orientation = newOrientation; }

private:
    static const int MIN_LENGTH = 1;
    static const int MAX_LENGTH = 4;
    int length;
    Orientation orientation;
    std::vector<SegmentStatus> segmentDamages;

    Ship(const Ship&) = delete;
    Ship& operator=(const Ship&) = delete;

    bool valid;
};

#endif
