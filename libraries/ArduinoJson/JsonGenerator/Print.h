/*
 * Arduino JSON library
 * Benoit Blanchon 2014 - MIT License
 */

#pragma once

#ifndef ARDUINO

typedef unsigned char uint8_t;

// This class reproduces Arduino's Print
class Print
{
public:

    virtual size_t write(uint8_t) = 0;

    size_t print(const char[]);
    size_t print(double, int = 2);
    size_t print(long);
};

#else

#include <Print.h>

#endif
