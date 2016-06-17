/**
 * \file bitfield.hpp
 * \date Jun 15, 2016
 */

#ifndef CPPBITFIELD_BITFIELD_HPP
#define CPPBITFIELD_BITFIELD_HPP

#include <cstdint>
#include <type_traits>
#include <limits>

#include <cassert>

#if !defined(CPPBITFIELD_ASSERT)
#  define CPPBITFIELD_ASSERT assert
#endif

namespace cppbitfield {

    namespace detail {

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

        template <bool GE8, bool GE16, bool GE32>
        struct SelectorImpl;

        template <>
        struct SelectorImpl<0, 0, 0>
        {
            using type = uint8_t;
        };

        template <>
        struct SelectorImpl<1, 0, 0>
        {
            using type = uint16_t;
        };

        template <>
        struct SelectorImpl<1, 1, 0>
        {
            using type = uint32_t;
        };

        template <>
        struct SelectorImpl<1, 1, 1>
        {
            using type = uint64_t;
        };

        template <int Size>
        struct StorageTypeSelector
        {
            static const bool GE_8  = Size >=  8;
            static const bool GE_16 = Size >= 16;
            static const bool GE_32 = Size >= 32;

            using type = typename SelectorImpl<GE_8, GE_16, GE_32>::type;
        };

    } // namespace detail

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
            static const int value = detail::GetImpl<Idx, S, Sizes...>::value;
        };

        template <int Idx>
        struct SumTill
        {
            static_assert(Idx >= 0, "Index out of bounds.");
            static_assert(Idx < (sizeof...(Sizes) + 1), "Index out of bounds.");
            static const int value = detail::SumTillImpl<Idx, S, Sizes...>::value;
        };     
    };

    template <class EnumType>
    struct BitFieldDescriptor
    {
        static_assert(std::is_enum<EnumType>::value, "EnumType must be an enum!");
        static_assert(!std::is_convertible<EnumType, int>::value, "Feature requires use of enum class.");

        using IntType = typename std::underlying_type<EnumType>::type;

        template <EnumType X>
        using AsInt = typename detail::AsIntType<EnumType, IntType>::template Convert<X>;

        template <IntType X>
        using AsEnum = typename detail::AsEnumType<EnumType, IntType>::template Convert<X>;

        static const IntType NumFields = AsInt<EnumType::__NUM_FIELDS>::value;
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

        static const int NumBits = FieldOffset<NumFields - 1>::value + FieldLength<NumFields - 1>::value;

        static_assert(Sizes::NumFields == NumFields, "Number of fields must match total number of fields.");
        static_assert(NumBits <= 64, "Total number of bits must be <= 64.");

        using StorageType = typename detail::StorageTypeSelector<NumBits>::type;

      private:
        StorageType m_bits;

        static const StorageType ALL_ONES = std::numeric_limits<StorageType>::max();
        static const StorageType ONE = static_cast<StorageType>(1);
        static const StorageType ZERO = static_cast<StorageType>(0);
      public:
        BitFields() : m_bits(ZERO) { }

        ~BitFields() { }

        BitFields(const BitFields & rhs) : m_bits(rhs.m_bits) { }

        BitFields & operator=(const BitFields & rhs)
        {
            m_bits = rhs.m_bits;
            return *this;
        }

        template <EnumType X, class Y>
        Y get() const
        {
            static const IntType asInt = AsInt<X>::value;
            static const int offset = FieldOffset<asInt>::value;
            static const int length = FieldLength<asInt>::value;
            static const auto mask = ~((ALL_ONES) << length);
            return static_cast<Y>((m_bits >> offset) & mask);
        }

        template <EnumType X, class Y>
        void set(Y val)
        {
            static const IntType asInt = AsInt<X>::value;
            static const int offset = FieldOffset<asInt>::value;
            static const int length = FieldLength<asInt>::value;
            static const auto mask = ~((ALL_ONES) << length);
            auto valtosettrunc = static_cast<StorageType>(val) & mask;
            CPPBITFIELD_ASSERT("Value too large for bitfield length." &&
                               (static_cast<StorageType>(val) == valtosettrunc));
            m_bits = (m_bits & (~mask << offset)) | (valtosettrunc << offset);
        }

        template <EnumType X>
        void set(bool val)
        {
            set<X>(val ? ONE : ZERO);
        }
    };

} // namespace cppbitfield

#define BITFIELD_VA_ARGS_X(...) , ##__VA_ARGS__

#define DEFINE_BITFIELD_ENUM_X(X, ...)   \
    enum class X                         \
    {                                    \
        __VA_ARGS__                      \
    }

#define DEFINE_BITFIELD_ENUM(X, Y, ...) \
    DEFINE_BITFIELD_ENUM_X(X, Y = 0  BITFIELD_VA_ARGS_X(__VA_ARGS__), __NUM_FIELDS)

#define DEFINE_BITFIELD_SIZES(X, ...) \
    using X = cppbitfield::BitFieldSizes<__VA_ARGS__>;

#define DEFINE_BITFIELDS(N, X, Y) \
    using N = cppbitfield::BitFields<X, Y>;

#endif/*CPPBITFIELD_BITFIELD_HPP*/
