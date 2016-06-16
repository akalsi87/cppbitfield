/**
 * \file bitfield.hpp
 * \date Jun 15, 2016
 */

#ifndef CPPBITFIELD_BITFIELD_HPP
#define CPPBITFIELD_BITFIELD_HPP

#include <cstdint>
#include <type_traits>
#include <typeinfo>

#define VA_ARGS_X(...) , ##__VA_ARGS__

#define DECLARE_BITFIELD_ENUM_X(X, ...)  \
    enum class X                         \
    {                                    \
        __VA_ARGS__                      \
    }

#define DECLARE_BITFIELD_ENUM(X, Y, ...) \
    DECLARE_BITFIELD_ENUM_X(X, Y = 0  VA_ARGS_X(__VA_ARGS__), __NUM_FIELDS)

namespace cppbitfield {

    template <class EnumType, class IntType>
    struct AsIntType
    {
        template <EnumType X>
        struct Convert
        {
            static const IntType value = static_cast<IntType>(X);
        };
    };

    template <class EnumType, class IntType>
    struct AsEnumType
    {
        template <IntType X>
        struct Convert
        {
            static const EnumType value = static_cast<EnumType>(X);
        };
    };

    template <int Idx, int... Sizes>
    struct GetImpl;

    template <int Idx>
    struct GetImpl<Idx>
    {
        static const int value = -1;
    };

    template <int Idx, int S, int... Sizes>
    struct GetImpl<Idx, S, Sizes...>
    {
        static const int value = (Idx == 0) ? S : GetImpl<Idx - 1, Sizes...>::value;
    };


    template <int Idx, int... Sizes>
    struct SumTillImpl;

    template <int Idx>
    struct SumTillImpl<Idx>
    {
        static const int value = -1;
    };

    template <int Idx, int S, int... Sizes>
    struct SumTillImpl<Idx, S, Sizes...>
    {
        static const int value = (Idx == 0) ? 0 : (S + SumTillImpl<Idx - 1, Sizes...>::value);
    };

    template <int... Sizes>
    struct BitFieldSizes;

    template <>
    struct BitFieldSizes<>
    {
        static const int NumFields = 0;
    };

    template <int S, int... Sizes>
    struct BitFieldSizes<S, Sizes...>
    {
        static_assert(S >= 1 && S <= 64, "Bit field size must be in the range [1, 64].");
        static const int NumFields = 1 + sizeof...(Sizes);

        template <int Idx>
        struct Get
        {
            static_assert(Idx >= 0, "Index out of bounds.");
            static_assert(Idx < (sizeof...(Sizes) + 1), "Index out of bounds.");
            static const int value = GetImpl<Idx, S, Sizes...>::value;
        };

        template <int Idx>
        struct SumTill
        {
            static_assert(Idx >= 0, "Index out of bounds.");
            static_assert(Idx < (sizeof...(Sizes) + 1), "Index out of bounds.");
            static const int value = SumTillImpl<Idx, S, Sizes...>::value;
        };     
    };

    template <class EnumType>
    struct BitFieldDescriptor
    {
        static_assert(std::is_enum<EnumType>::value, "EnumType must be an enum!");
        static_assert(!std::is_convertible<EnumType, int>::value, "Feature requires use of enum class.");

        using IntType = typename std::underlying_type<EnumType>::type;

        template <EnumType X>
        using AsInt = typename AsIntType<EnumType, IntType>::template Convert<X>;

        template <IntType X>
        using AsEnum = typename AsEnumType<EnumType, IntType>::template Convert<X>;

        static const IntType NumFields = AsInt<EnumType::__NUM_FIELDS>::value;

        template <EnumType fieldname, int fieldlen>
        struct Field
        {
            static_assert(AsInt<fieldname>::value < NumFields, "Field name value must be less than total number of fields.");
            static_assert(AsInt<fieldname>::value >= 0, "Field name value must be non-negative.");
            static_assert(fieldlen >= 1, "Min field length supported is 1.");
            static_assert(fieldlen <= 64, "Max field length supported is 64.");
        };
    };

    template <class EnumType, class Sizes>
    struct BitFields
    {
        using Traits = BitFieldDescriptor<EnumType>;
        
        static const int NumFields = Traits::NumFields;

        using IntType = typename Traits::IntType;

        template <EnumType X>
        using AsInt = typename Traits::template AsInt<X>;

        template <IntType X>
        using AsEnum = typename Traits::template AsEnum<X>;

        template <int X>
        using FieldLength = typename Sizes::template Get<X>;

        template <int X>
        using FieldOffset = typename Sizes::template SumTill<X>;

        static_assert(Sizes::NumFields == NumFields, "Number of fields must match total number of fields.");
        static_assert(FieldOffset<NumFields - 1>::value + FieldLength<NumFields - 1>::value <= 64, "Total number of bits must be <= 64.");
        // static_assert(Sizes::SumTill<Sizes::NumFields - 1>) > 0, "Total number of bits must be <= 64.");

    };

} // namespace cppbitfield

#define DECLARE_BITFIELD_SIZES(X, ...) \
    using X = cppbitfield::BitFieldSizes<__VA_ARGS__>;

#define DECLARE_BITSET(N, X, Y) \
    using N = cppbitfield::BitFields<X, Y>;

#endif/*CPPBITFIELD_BITFIELD_HPP*/
