#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class AbilityUnavailableException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "No abilities available to use.";
    }
};

class ShipPlacementException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Cannot place ship at specified position: adjacent to or overlapping another ship.";
    }
};

class OutOfBoundsException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Cell position out of range";
    }
};

#endif
