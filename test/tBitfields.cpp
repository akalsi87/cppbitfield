/**
 * \file tBitfields.cpp
 * \date Jun 15, 2016
 */

#include "unittest.hpp"

#include <cppbitfield/bitfield.hpp>

CPP_TEST( t0 )
{
    DECLARE_BITFIELD_ENUM(
         FooEnum,
               A,
               B,
               C);

    TEST_TRUE(static_cast<int>(FooEnum::A) == 0);

    DECLARE_BITFIELD_SIZES(
        FooSizes,
               1,
               2,
               3);

    TEST_TRUE(FooSizes::Get<0>::value == 1);
    TEST_TRUE(FooSizes::Get<1>::value == 2);
    TEST_TRUE(FooSizes::Get<2>::value == 3);
    TEST_TRUE(FooSizes::SumTill<0>::value == 0);
    TEST_TRUE(FooSizes::SumTill<1>::value == 1);
    TEST_TRUE(FooSizes::SumTill<2>::value == 3);

    DECLARE_BITSET(Foo, FooEnum, FooSizes);

    Foo x;
    static_cast<void>(&x);

    TEST_TRUE(Foo::NumFields == 3);

    TEST_TRUE(Foo::FieldOffset<0>::value == 0);
    TEST_TRUE(Foo::FieldOffset<1>::value == 1);
    TEST_TRUE(Foo::FieldOffset<2>::value == 3);

    TEST_TRUE(Foo::FieldLength<0>::value == 1);
    TEST_TRUE(Foo::FieldLength<1>::value == 2);
    TEST_TRUE(Foo::FieldLength<2>::value == 3);

    TEST_TRUE(Foo::AsEnum<0>::value == FooEnum::A);
    TEST_TRUE(Foo::AsEnum<1>::value == FooEnum::B);
    TEST_TRUE(Foo::AsEnum<2>::value == FooEnum::C);
}
