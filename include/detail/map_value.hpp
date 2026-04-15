#ifndef MSTD_DETAIL_MAP_VALUE_HPP
#define MSTD_DETAIL_MAP_VALUE_HPP

#include <utility>
#include <tuple>

namespace mstd {

template <class KeyT, class ValueT>
class MapValue {
public:

    MapValue() = default;
    MapValue(const MapValue&) = default;
    MapValue(MapValue&&) = default; // TODO(gogagum): make this noexcept when possible
    MapValue(const KeyT& key, const ValueT& value) : key_(key), value_(value) {}
    MapValue(KeyT&& key, ValueT&& value) : key_(std::move(key)), value_(std::move(value)) {}
    template <class KT, class VT>
    MapValue(KT&& key, VT&& value) : key_(std::forward<KT>(key)), value_(std::forward<VT>(value)) {}
    template <class... KeyArgsT, class... ValueArgsT>
    MapValue(std::piecewise_construct_t, std::tuple<KeyArgsT...> key_args, std::tuple<ValueArgsT...> value_args)
        : key_(std::make_from_tuple<KeyT>(std::move(key_args)))
        , value_(std::make_from_tuple<ValueT>(std::move(value_args))) {}

    const KeyT& key() const { return key_; }
    ValueT& value() { return value_; }
    const ValueT& value() const { return value_; }
    bool operator==(const MapValue& other) const {
        return key_ == other.key_ && value_ == other.value_;
    }

private:
    struct KeyProj {
        KeyT& operator()(MapValue& mv) const {
            return mv.key_;
        }

        const KeyT& operator()(const MapValue& mv) const {
            return mv.key_;
        }
    };

    MapValue& operator=(const MapValue&) = default;
    MapValue& operator=(MapValue&&) = default; // TODO(gogagum): make this noexcept when possible

    KeyT key_;
    ValueT value_;

    template <class, class, class, class>
    friend class map;

    template <class, class, class, class>
    friend class multimap;

    template <class, class, class, class>
    friend class Tree;
};


}

#endif // MSTD_DETAIL_MAP_VALUE_HPP
