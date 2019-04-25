#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// hash calculates the MurmurHash3 (128 bits, x64 version) of the given values.
    template <typename Uint, typename HandleUint64Pair>
    class hash {
        public:
        hash(HandleUint64Pair handle_uint64_pair) :
            _handle_uint64_pair(std::forward<HandleUint64Pair>(handle_uint64_pair)),
            _shift(0),
            _block(0, 0),
            _hash(0, 0),
            _size(0) {}
        hash(const hash&) = delete;
        hash(hash&&) = default;
        hash& operator=(const hash&) = delete;
        hash& operator=(hash&&) = default;
        virtual ~hash() {
            if (_size > 0 || _shift > 0) {
                if (_shift * sizeof(Uint) > 8) {
                    std::get<1>(_block) *= 0x4cf5ad432745937full;
                    std::get<1>(_block) = rotate(std::get<1>(_block), 33);
                    std::get<1>(_block) *= 0x87c37b91114253d5ull;
                    std::get<1>(_hash) ^= std::get<1>(_block);
                }
                if (_shift * sizeof(Uint) > 0) {
                    std::get<0>(_block) *= 0x87c37b91114253d5ull;
                    std::get<0>(_block) = rotate(std::get<0>(_block), 31);
                    std::get<0>(_block) *= 0x4cf5ad432745937full;
                    std::get<0>(_hash) ^= std::get<0>(_block);
                }
                std::get<0>(_hash) ^= (_size * 16 + _shift);
                std::get<1>(_hash) ^= (_size * 16 + _shift);
                std::get<0>(_hash) += std::get<1>(_hash);
                std::get<1>(_hash) += std::get<0>(_hash);
                std::get<0>(_hash) = mix(std::get<0>(_hash));
                std::get<1>(_hash) = mix(std::get<1>(_hash));
                std::get<0>(_hash) += std::get<1>(_hash);
                std::get<1>(_hash) += std::get<0>(_hash);
                _handle_uint64_pair(_hash);
            }
        }

        /// operator() handles an event.
        virtual void operator()(Uint uint) {
            if (_shift < 8 / sizeof(Uint)) {
                std::get<0>(_block) |= (static_cast<uint64_t>(uint) << (_shift * sizeof(Uint) * 8));
                ++_shift;
            } else {
                std::get<1>(_block) |= (static_cast<uint64_t>(uint) << (_shift * sizeof(Uint) * 8 - 64));
                if (_shift < 16 / sizeof(Uint) - 1) {
                    ++_shift;
                } else {
                    _shift = 0;
                    ++_size;
                    std::get<0>(_block) *= 0x87c37b91114253d5ull;
                    std::get<0>(_block) = rotate(std::get<0>(_block), 31);
                    std::get<0>(_block) *= 0x4cf5ad432745937full;
                    std::get<0>(_hash) ^= std::get<0>(_block);
                    std::get<0>(_hash) = rotate(std::get<0>(_hash), 27);
                    std::get<0>(_hash) += std::get<1>(_hash);
                    std::get<0>(_hash) = std::get<0>(_hash) * 5 + 0x52dce729;
                    std::get<1>(_block) *= 0x4cf5ad432745937full;
                    std::get<1>(_block) = rotate(std::get<1>(_block), 33);
                    std::get<1>(_block) *= 0x87c37b91114253d5ull;
                    std::get<1>(_hash) ^= std::get<1>(_block);
                    std::get<1>(_hash) = rotate(std::get<1>(_hash), 31);
                    std::get<1>(_hash) += std::get<0>(_hash);
                    std::get<1>(_hash) = std::get<1>(_hash) * 5 + 0x38495ab5;
                    std::get<0>(_block) = 0;
                    std::get<1>(_block) = 0;
                }
            }
        }

        protected:
        /// rotate implements a bit-wise rotation.
        static uint64_t rotate(uint64_t value, uint8_t range) {
            return (value << range) | (value >> (64 - range));
        }

        /// mix implements a bit-wise mix.
        static uint64_t mix(uint64_t value) {
            value ^= value >> 33;
            value *= 0xff51afd7ed558ccdull;
            value ^= value >> 33;
            value *= 0xc4ceb9fe1a85ec53ull;
            value ^= value >> 33;
            return value;
        }

        HandleUint64Pair _handle_uint64_pair;
        uint8_t _shift;
        std::pair<uint64_t, uint64_t> _block;
        std::pair<uint64_t, uint64_t> _hash;
        uint64_t _size;
    };

    /// make_hash creates a hash from functors.
    template <typename Uint, typename HandleUint64Pair>
    inline hash<Uint, HandleUint64Pair> make_hash(HandleUint64Pair handle_uint64_pair) {
        return hash<Uint, HandleUint64Pair>(std::forward<HandleUint64Pair>(handle_uint64_pair));
    }
}
