/**
 * \file bitfield.hpp
 * \date Jun 15, 2016
 */

#ifndef CPPBITFIELD_BITFIELD_HPP
#define CPPBITFIELD_BITFIELD_HPP

#include <cstdint>
#include <type_traits>

#define DECLARE_BITFIELD_ENUM(X, Y, ...) \
    enum class X                         \
    {                                    \
        Y = 0,                           \
        __VA_ARGS__                      \
        , NUM_FIELDS                     \
    }

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

        static const IntType NumFields = AsInt<EnumType::NUM_FIELDS>::value;

        template <EnumType fieldname, int fieldlen>
        struct Field
        {
            static_assert(AsInt<fieldname>::value < NumFields, "Field name value must be less than total number of fields.");
            static_assert(AsInt<fieldname>::value >= 0, "Field name value must be non-negative.");
            static_assert(fieldlen >= 1, "Min field length supported is 1.");
            static_assert(fieldlen <= 64, "Max field length supported is 64.");
        };
    };

    template <class EnumType, class... Fields>
    struct BitFields
    {
        using Traits = BitFieldDescriptor<EnumType>;
        static_assert(sizeof...(Fields) == Traits::NumFields, "Number of fields must match total number of fields.");
    };

} // namespace cppbitfield

#endif/*CPPBITFIELD_BITFIELD_HPP*/
