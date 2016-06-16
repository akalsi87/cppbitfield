/**
 * \file tBitfields.cpp
 * \date Jun 15, 2016
 */

#include "unittest.hpp"

#include <cppbitfield/bitfield.hpp>

using namespace cppbitfield;

CPP_TEST( SingleElementEnum )
{
    DECLARE_BITFIELD_ENUM(
        Foo,
        FOO);

    using FieldMaker = BitFieldDescriptor<Foo>;

    using MyBitfield = BitFields<
                        Foo,
                        FieldMaker::Field<Foo::FOO, 64>>;

    MyBitfield X;
    static_cast<void>(&X);
}
