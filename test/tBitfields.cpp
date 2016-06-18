/**
 * \file tBitfields.cpp
 * \date Jun 15, 2016
 */

#include "unittest.hpp"

#include <cppbitfield/bitfield.hpp>

CPP_TEST( t0 )
{
    DEFINE_BITFIELD_ENUM(
         FooEnum,
               A,
               B,
               C);

    DEFINE_BITFIELD_SIZES(
        FooSizes,
               1,
               2,
               3);

    DEFINE_BITFIELDS(
        Foo,
        FooEnum,
        FooSizes);

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

    TEST_TRUE(0 == Foo::AsInt<FooEnum::A>::value);
    TEST_TRUE(1 == Foo::AsInt<FooEnum::B>::value);
    TEST_TRUE(2 == Foo::AsInt<FooEnum::C>::value);

    auto isCorrectType = std::is_same<uint8_t, Foo::StorageType>::value;
    TEST_TRUE(isCorrectType);

    Foo x;
    auto aVal = x.get<FooEnum::A>();
    TEST_TRUE(aVal == 0);
    auto bVal = x.get<FooEnum::B>();
    TEST_TRUE(bVal == 0);
    auto cVal = x.get<FooEnum::C>();
    TEST_TRUE(cVal == 0);

    x.set<FooEnum::A>(1);
    x.set<FooEnum::B>(2);
    x.set<FooEnum::C>(7);
    aVal = x.get<FooEnum::A>();
    bVal = x.get<FooEnum::B>();
    cVal = x.get<FooEnum::C>();
    TEST_TRUE(aVal == 1);
    TEST_TRUE(bVal == 2);
    TEST_TRUE(cVal == 7);

    x.set<FooEnum::C>(true);
    aVal = x.get<FooEnum::A>();
    bVal = x.get<FooEnum::B>();
    cVal = x.get<FooEnum::C>();
    TEST_TRUE(aVal == 1);
    TEST_TRUE(bVal == 2);
    TEST_TRUE(cVal == 1);

    x.set<FooEnum::C>(4);
    aVal = x.get<FooEnum::A>();
    bVal = x.get<FooEnum::B>();
    cVal = x.get<FooEnum::C>();
    TEST_TRUE(aVal == 1);
    TEST_TRUE(bVal == 2);
    TEST_TRUE(cVal == 4);
}
