#include <iterator>
#include <functional>

#ifndef ITRBL
#define ITRBL
template <typename ValueType, class InputIterator>
class IgnoreIterator : public std::iterator<
                            std::input_iterator_tag,
                            ValueType,
                            long,
                            ValueType*,
                            ValueType>
{
    InputIterator current;
    InputIterator finish;
    std::function<bool(const ValueType&)> predicate;
public:
    IgnoreIterator(const InputIterator &begin, const InputIterator &end, std::function<bool(const ValueType&)> pred) :
        current(find_if(begin, end, pred)), finish(end), predicate(pred) {}
    ValueType operator *() const { return *current; }
    IgnoreIterator& operator ++() {
        while ((++current != finish) && !predicate(*current)){}
        return *this;
    }
    IgnoreIterator operator ++(int) {
        IgnoreIterator tmp(*this);
        ++tmp;
        return *this;
    }
    bool operator ==(const InputIterator &s) const { return (*this).current == s; }
    bool operator !=(const InputIterator &s) const { return !(*this == s); }
};

template <typename ValueType,
         typename InputIterator,
         typename const_InputIterator>
class Iterable
{
public:

    virtual InputIterator begin() = 0;
    virtual InputIterator end() = 0;
    virtual const_InputIterator begin() const = 0;
    virtual const_InputIterator end() const = 0;
    virtual IgnoreIterator<ValueType, InputIterator> filter(std::function<bool(const ValueType&)>) = 0;
};
#endif
