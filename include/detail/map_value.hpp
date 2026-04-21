#ifndef MSTD_DETAIL_MAP_VALUE_HPP
#define MSTD_DETAIL_MAP_VALUE_HPP

#include <utility>
#include <tuple>

#include "concepts/allocator_concept.hpp"
#include "concepts/compare_concepts.hpp"

namespace mstd {

template <class KeyT, class ValueT>
class MapValue {
public:

    MapValue() = default;
    MapValue(const MapValue&) = default;
    MapValue(MapValue&&) = default; // TODO(gogagum): make this noexcept when possible
    MapValue(const KeyT& key, const ValueT& value) : key_(key), value_(value) {}
    // TODO(gogagum): think of a constructor that gets key and value via structured binding
    MapValue(const std::pair<KeyT, ValueT>& kv_pair) : key_(kv_pair.first), value_(kv_pair.second) {}
    MapValue(std::pair<KeyT, ValueT>&& kv_pair) : key_(std::move(kv_pair.first)), value_(std::move(kv_pair.second)) {}
    MapValue(const std::tuple<KeyT, ValueT>& kv_pair) : key_(std::get<0>(kv_pair)), value_(std::get<1>(kv_pair)) {}
    MapValue(std::tuple<KeyT, ValueT>&& kv_pair) : key_(std::move(std::get<0>(kv_pair))), value_(std::move(std::get<1>(kv_pair))) {}
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

    template <class KeyT2, class, OrdersAtLeastWeakly<KeyT2> CompareT2, Allocator AllocatorT>
    friend class map;

    template <class KeyT2, class, OrdersAtLeastWeakly<KeyT2> CompareT2, Allocator AllocatorT>
    friend class multimap;

    template <class, class, class, class>
    friend class Tree;
};


}

#endif // MSTD_DETAIL_MAP_VALUE_HPP
