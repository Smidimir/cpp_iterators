#pragma once


#include <iterator>
#include <variant>


template<typename Iterator>
using bidirectional_iterator_variant_t = std::variant<Iterator, std::reverse_iterator<Iterator>>;

#define DECLARE_VARIANT_ITERATOR_FUNCTION(FUNCTION_NAME) \
    template<typename Iterator> \
    bidirectional_iterator_variant_t<Iterator> FUNCTION_NAME(bidirectional_iterator_variant_t<Iterator> itVar) \
    { \
        return std::visit( \
                [](auto const& it) -> bidirectional_iterator_variant_t<Iterator> \
                { \
                    return FUNCTION_NAME(it); \
                } \
                , itVar ); \
    }

template<typename Iterator>
std::reverse_iterator<Iterator> invert_iterator(Iterator it)
{
    return std::make_reverse_iterator(it);
}

template<typename Iterator>
Iterator invert_iterator(std::reverse_iterator<Iterator> it)
{
    return it.base();
}

DECLARE_VARIANT_ITERATOR_FUNCTION(invert_iterator)

template<typename Iterator>
auto stable_invert_iterator(Iterator it)
{
    return invert_iterator(std::next(it));
}

DECLARE_VARIANT_ITERATOR_FUNCTION(stable_invert_iterator)

template<typename Iterator>
Iterator to_forward_iterator(Iterator it)
{
    return it;
}

template<typename Iterator>
Iterator to_forward_iterator(std::reverse_iterator<Iterator> it)
{
    return invert_iterator(it);
}

DECLARE_VARIANT_ITERATOR_FUNCTION(to_forward_iterator)

template<typename Iterator>
Iterator stable_to_forward_iterator(Iterator it)
{
    return it;
}

template<typename Iterator>
Iterator stable_to_forward_iterator(std::reverse_iterator<Iterator> it)
{
    return stable_invert_iterator(it);
}

DECLARE_VARIANT_ITERATOR_FUNCTION(stable_to_forward_iterator)

template<typename Iterator>
std::reverse_iterator<Iterator> to_reverse_iterator(Iterator it)
{
    return invert_iterator(it);
}

template<typename Iterator>
Iterator to_reverse_iterator(std::reverse_iterator<Iterator> it)
{
    return it;
}

DECLARE_VARIANT_ITERATOR_FUNCTION(to_reverse_iterator)

template<typename Iterator>
std::reverse_iterator<Iterator> stable_to_reverse_iterator(Iterator it)
{
    return stable_invert_iterator(it);
}

template<typename Iterator>
std::reverse_iterator<Iterator> stable_to_reverse_iterator(std::reverse_iterator<Iterator> it)
{
    return it;
}

DECLARE_VARIANT_ITERATOR_FUNCTION(stable_to_reverse_iterator)


template<typename Iterator>
class bidirectional_iterator
{
public:
    using iterator_forward_type = Iterator;
    using iterator_reverse_type = std::reverse_iterator<iterator_forward_type>;
    using iterator_variant_type = bidirectional_iterator_variant_t<iterator_forward_type>;

    using value_type        = typename iterator_forward_type::value_type;
    using reference         = typename iterator_forward_type::reference;
    using pointer           = typename iterator_forward_type::pointer;
    using iterator_category = typename iterator_forward_type::iterator_category;
    using difference_type   = typename iterator_forward_type::difference_type;

    bidirectional_iterator(bidirectional_iterator const&) = default;
    explicit bidirectional_iterator(iterator_forward_type it) : m_iterator{it} {}
    explicit bidirectional_iterator(iterator_reverse_type it) : m_iterator{it} {}
    explicit bidirectional_iterator(iterator_variant_type it) : m_iterator{it} {}

    bidirectional_iterator& operator = (bidirectional_iterator const&) = default;
    bidirectional_iterator& operator = (iterator_forward_type it) { m_iterator = it; return *this; }
    bidirectional_iterator& operator = (iterator_reverse_type it) { m_iterator = it; return *this; }
    bidirectional_iterator& operator = (iterator_variant_type it) { m_iterator = it; return *this; }

    reference                      operator *  ()          { return indirection(m_iterator); }
    reference                      operator *  ()    const { return indirection(m_iterator); }
    pointer                        operator -> ()          { return structure_dereference(m_iterator); }
    pointer                        operator -> ()    const { return structure_dereference(m_iterator); }
    bidirectional_iterator       & operator ++ ()          { return prefix_increment(m_iterator); }
    bidirectional_iterator       & operator -- ()          { return prefix_decrement(m_iterator); }
    bidirectional_iterator  const  operator ++ (int)       { return postfix_increment(m_iterator); }
    bidirectional_iterator  const  operator -- (int)       { return postfix_decrement(m_iterator); }

#define DECLARE_ITERATOR_INVERTOR(MEMBER_FUNCTION_NAME, FUNCTION_NAME) \
    bidirectional_iterator& MEMBER_FUNCTION_NAME() { m_iterator = FUNCTION_NAME(m_iterator); return *this; }

    DECLARE_ITERATOR_INVERTOR(invert           , invert_iterator           )
    DECLARE_ITERATOR_INVERTOR(stable_invert    , stable_invert_iterator    )
    DECLARE_ITERATOR_INVERTOR(to_forward       , to_forward_iterator       )
    DECLARE_ITERATOR_INVERTOR(stable_to_forward, stable_to_forward_iterator)
    DECLARE_ITERATOR_INVERTOR(to_reverse       , to_reverse_iterator       )
    DECLARE_ITERATOR_INVERTOR(stable_to_reverse, stable_to_reverse_iterator)
#undef DECLARE_ITERATOR_INVERTOR

#define DECLARE_ITERATOR_EXTRACTOR(ITERATOR_TYPE, MEMBER_FUNCTION_NAME, EXTRACTOR) \
        ITERATOR_TYPE MEMBER_FUNCTION_NAME() const noexcept \
        { \
            return std::get<ITERATOR_TYPE>(EXTRACTOR(m_iterator)); \
        }

    DECLARE_ITERATOR_EXTRACTOR(iterator_forward_type, extract_forward_iterator       , to_forward_iterator       )
    DECLARE_ITERATOR_EXTRACTOR(iterator_forward_type, stable_extract_forward_iterator, stable_to_forward_iterator)
    DECLARE_ITERATOR_EXTRACTOR(iterator_reverse_type, extract_reverse_iterator       , to_reverse_iterator       )
    DECLARE_ITERATOR_EXTRACTOR(iterator_reverse_type, stable_extract_reverse_iterator, stable_to_reverse_iterator)
#undef DECLARE_ITERATOR_EXTRACTOR

    explicit operator iterator_forward_type () const noexcept { return extract_forward_iterator(); }
    explicit operator iterator_reverse_type () const noexcept { return extract_reverse_iterator(); }

#define DECLARE_COMPERISON_OP_FOR_VARIANT( OP_NAME, OP, VARIANT_TYPE ) \
    bool operator OP (VARIANT_TYPE const& it) const { return this->OP_NAME(m_iterator, it); }

#define DECLARE_COMPERISON_OP( OP_NAME, OP ) \
    DECLARE_COMPERISON_OP_FOR_VARIANT(OP_NAME, OP, iterator_forward_type) \
    DECLARE_COMPERISON_OP_FOR_VARIANT(OP_NAME, OP, iterator_reverse_type) \
    DECLARE_COMPERISON_OP_FOR_VARIANT(OP_NAME, OP, iterator_variant_type) \
    DECLARE_COMPERISON_OP_FOR_VARIANT(OP_NAME, OP, bidirectional_iterator)

    DECLARE_COMPERISON_OP(equal_to                , ==)
    DECLARE_COMPERISON_OP(not_equal_to            , !=)
    DECLARE_COMPERISON_OP(less_than               , < )
    DECLARE_COMPERISON_OP(less_than_or_equal_to   , <=)
    DECLARE_COMPERISON_OP(greater_than            , > )
    DECLARE_COMPERISON_OP(greater_than_or_equal_to, >=)

#undef DECLARE_COMPERISON_OP
#undef DECLARE_COMPERISON_OP_FOR_VARIANT

    bidirectional_iterator& operator += (difference_type const& n)                   { return addition_assignment(m_iterator, n); }
    bidirectional_iterator& operator -= (difference_type const& n)                   { return subtraction_assignment(m_iterator, n); }
    bidirectional_iterator  operator +  (difference_type const& n)             const { return bidirectional_iterator{*this} += n;  }
    bidirectional_iterator  operator -  (difference_type const& n)             const { return bidirectional_iterator{*this} -= n;  }
    difference_type         operator -  (iterator_forward_type const& it)      const { return difference(m_iterator, it); }
    difference_type         operator -  (iterator_reverse_type const& it)      const { return difference(m_iterator, it); }
    difference_type         operator -  (iterator_variant_type const& it)      const { return difference(m_iterator, it); }
    difference_type         operator -  (bidirectional_iterator     const& it) const { return difference(m_iterator, it); }
    reference               operator [] (difference_type const& n)                   { return subscript(m_iterator, n);  }
    reference               operator [] (difference_type const& n)             const { return subscript(m_iterator, n);  }

private:
#define DECLARE_UNARY_OPERATOR_COMMON(RETURN_TYPE, ITERATOR_VARIANT_TYPE, OPERATOR_NAME, OPERATOR, FUNCTION_QULIFIERS) \
    RETURN_TYPE OPERATOR_NAME(ITERATOR_VARIANT_TYPE& it) FUNCTION_QULIFIERS \
    { \
        return std::visit( \
                [this](auto& it) -> RETURN_TYPE \
                { \
                    UNARY_OPERATOR_BODY(OPERATOR) \
                }, it); \
    }
#define DECLARE_UNARY_OPERATOR(RETURN_TYPE, ITERATOR_VARIANT_TYPE, OPERATOR_NAME, OPERATOR) \
    DECLARE_UNARY_OPERATOR_COMMON(RETURN_TYPE, ITERATOR_VARIANT_TYPE, OPERATOR_NAME, OPERATOR,)
#define DECALRE_CONST_UNARY_OPERATOR(RETURN_TYPE, ITERATOR_VARIANT_TYPE, OPERATOR_NAME, OPERATOR) \
    DECLARE_UNARY_OPERATOR_COMMON(RETURN_TYPE, ITERATOR_VARIANT_TYPE const, OPERATOR_NAME, OPERATOR, const)

#define UNARY_OPERATOR_BODY(OPERATOR) \
    return it.operator OPERATOR ();

    DECLARE_UNARY_OPERATOR      (reference, iterator_variant_type, indirection          , * )
    DECALRE_CONST_UNARY_OPERATOR(reference, iterator_variant_type, indirection          , * )
    DECLARE_UNARY_OPERATOR      (pointer  , iterator_variant_type, structure_dereference, ->)
    DECALRE_CONST_UNARY_OPERATOR(pointer  , iterator_variant_type, structure_dereference, ->)
#undef UNARY_OPERATOR_BODY

#define UNARY_OPERATOR_BODY(OPERATOR) \
    it.operator OPERATOR (); return *this;

    DECLARE_UNARY_OPERATOR(bidirectional_iterator&, iterator_variant_type, prefix_increment, ++);
    DECLARE_UNARY_OPERATOR(bidirectional_iterator&, iterator_variant_type, prefix_decrement, --);
#undef UNARY_OPERATOR_BODY

#define UNARY_OPERATOR_BODY(OPERATOR) \
    auto tmp = *this; it.operator OPERATOR (); return tmp;

    DECLARE_UNARY_OPERATOR(bidirectional_iterator const, iterator_variant_type, postfix_increment, ++);
    DECLARE_UNARY_OPERATOR(bidirectional_iterator const, iterator_variant_type, postfix_decrement, --);
#undef UNARY_OPERATOR_BODY

#undef DECALRE_CONST_UNARY_OPERATOR
#undef DECLARE_UNARY_OPERATOR
#undef DECLARE_UNARY_OPERATOR_COMMON

#define DECLARE_ONE_VARIANT_FUNCTION(RETURN_TYPE, OPERATOR_NAME, OTHER_VARIANT_TYPE) \
    RETURN_TYPE OPERATOR_NAME(iterator_variant_type const& it, OTHER_VARIANT_TYPE const& other_it) const \
    { \
        return std::visit( \
                [this, &other_it](auto& it) -> RETURN_TYPE \
                { \
                    return this->OPERATOR_NAME(it, other_it); \
                }, it); \
    }
#define DECLARE_TWO_VARIANT_FUNCTION(RETURN_TYPE, OPERATOR_NAME) \
    RETURN_TYPE OPERATOR_NAME(iterator_variant_type const& it, iterator_variant_type const& other_it) const \
    { \
        return std::visit( \
                [this](auto& it, auto const& other_it) -> RETURN_TYPE \
                { \
                    return this->OPERATOR_NAME(it, other_it); \
                }, it, other_it); \
    }

#define DEFINE_BINARY_OPERATOR(RETURN_TYPE, OPERATOR_NAME, OPERATOR) \
    RETURN_TYPE OPERATOR_NAME(iterator_forward_type const& it, iterator_forward_type const& other_it) const \
    { \
        return it OPERATOR other_it; \
    } \
    RETURN_TYPE OPERATOR_NAME(iterator_forward_type const& it, iterator_reverse_type const& other_it) const \
    { \
        return it OPERATOR invert_iterator(other_it); \
    } \
    RETURN_TYPE OPERATOR_NAME(iterator_reverse_type const& it, iterator_reverse_type const& other_it) const \
    { \
        return it OPERATOR other_it; \
    } \
    RETURN_TYPE OPERATOR_NAME(iterator_reverse_type const& it, iterator_forward_type const& other_it) const \
    { \
        return it OPERATOR invert_iterator(other_it); \
    } \
    DECLARE_ONE_VARIANT_FUNCTION(RETURN_TYPE, OPERATOR_NAME, iterator_forward_type) \
    DECLARE_ONE_VARIANT_FUNCTION(RETURN_TYPE, OPERATOR_NAME, iterator_reverse_type) \
    DECLARE_TWO_VARIANT_FUNCTION(RETURN_TYPE, OPERATOR_NAME) \
    RETURN_TYPE OPERATOR_NAME(iterator_variant_type const& it, bidirectional_iterator const& other_it) const \
    { \
        return this->OPERATOR_NAME(it, other_it.m_iterator); \
    }

    DEFINE_BINARY_OPERATOR(bool           , equal_to                , ==);
    DEFINE_BINARY_OPERATOR(bool           , not_equal_to            , !=);
    DEFINE_BINARY_OPERATOR(bool           , less_than               , < );
    DEFINE_BINARY_OPERATOR(bool           , less_than_or_equal_to   , <=);
    DEFINE_BINARY_OPERATOR(bool           , greater_than            , > );
    DEFINE_BINARY_OPERATOR(bool           , greater_than_or_equal_to, >=);
    DEFINE_BINARY_OPERATOR(difference_type, difference              , - );

#undef DEFINE_BINARY_OPERATOR
#undef DECLARE_TWO_VARIANT_FUNCTION
#undef DECLARE_ONE_VARIANT_FUNCTION

#define DECLARE_ASSIGNMENT_OPERATOR(RETURN_TYPE, OPERATOR_NAME, OPERATOR) \
    RETURN_TYPE OPERATOR_NAME(iterator_variant_type& it, difference_type const& n) \
    { \
        return std::visit( \
                [this, &n](auto& it) -> RETURN_TYPE \
                { \
                    it.operator OPERATOR (n); return *this; \
                } \
                , it ); \
    }

    DECLARE_ASSIGNMENT_OPERATOR(bidirectional_iterator&, addition_assignment   , +=)
    DECLARE_ASSIGNMENT_OPERATOR(bidirectional_iterator&, subtraction_assignment, -=)

#undef DECLARE_ASSIGNMENT_OPERATOR

#define DECLARE_INDEXING_OPERATOR(RETURN_TYPE, ITERATOR_VARIANT_TYPE, OPERATOR_NAME, OPERATOR) \
    RETURN_TYPE OPERATOR_NAME(ITERATOR_VARIANT_TYPE & it, difference_type const& n) \
    { \
        return std::visit( \
                [this, &n](auto& it) -> RETURN_TYPE \
                { \
                    return it.operator OPERATOR (n); \
                } \
                , it ); \
    }

    DECLARE_INDEXING_OPERATOR(reference, iterator_variant_type      , subscript, [])
    DECLARE_INDEXING_OPERATOR(reference, iterator_variant_type const, subscript, [])

#undef DECLARE_INDEXING_OPERATOR

private:
    iterator_variant_type m_iterator;

};

template<typename Iterator>
bidirectional_iterator(Iterator) -> bidirectional_iterator<Iterator>;

template<typename Iterator>
bidirectional_iterator(std::reverse_iterator<Iterator>) -> bidirectional_iterator<Iterator>;

template<typename Iterator>
bidirectional_iterator(std::variant<Iterator, std::reverse_iterator<Iterator>>) -> bidirectional_iterator<Iterator>;


#define DECLARE_ITERATOR_INVERTOR(FUNCTION_NAME, MEMBER_FUNCTION_NAME) \
    template<typename Iterator> \
    bidirectional_iterator<Iterator> FUNCTION_NAME(bidirectional_iterator<Iterator> it) \
    { \
        return it.MEMBER_FUNCTION_NAME(); \
    }

DECLARE_ITERATOR_INVERTOR(invert_iterator           , invert           )
DECLARE_ITERATOR_INVERTOR(stable_invert_iterator    , stable_invert    )
DECLARE_ITERATOR_INVERTOR(to_forward_iterator       , to_forward       )
DECLARE_ITERATOR_INVERTOR(stable_to_forward_iterator, stable_to_forward)
DECLARE_ITERATOR_INVERTOR(to_reverse_iterator       , to_reverse       )
DECLARE_ITERATOR_INVERTOR(stable_to_reverse_iterator, stable_to_reverse)
#undef DECLARE_ITERATOR_INVERTOR

#define DECLARE_ITERATOR_EXTRACTOR(ITERATOR_TYPE, FUNCTION_NAME, MEMBER_FUNCTION_NAME) \
    template<typename Iterator> \
    ITERATOR_TYPE FUNCTION_NAME(bidirectional_iterator<Iterator> it) \
    { \
        return it.MEMBER_FUNCTION_NAME(); \
    }

DECLARE_ITERATOR_EXTRACTOR(Iterator                       , extract_forward_iterator       , extract_forward_iterator       )
DECLARE_ITERATOR_EXTRACTOR(Iterator                       , stable_extract_forward_iterator, stable_extract_forward_iterator)
DECLARE_ITERATOR_EXTRACTOR(std::reverse_iterator<Iterator>, extract_reverse_iterator       , extract_reverse_iterator       )
DECLARE_ITERATOR_EXTRACTOR(std::reverse_iterator<Iterator>, stable_extract_reverse_iterator, stable_extract_reverse_iterator)
#undef DECLARE_ITERATOR_EXTRACTOR
