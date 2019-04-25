#include "../source/hash.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"
#include <vector>

TEST_CASE("Hash a list of numbers", "[hash]") {
    auto hash = tarsier::make_hash<uint8_t>([](std::pair<uint64_t, uint64_t> hash) -> void {
        REQUIRE(std::get<0>(hash) == 0xb06f9999c14051caull);
        REQUIRE(std::get<1>(hash) == 0x0fbd6d93c8340799ull);
    });
    for (uint8_t index = 0; index < 100; ++index) {
        hash(index);
    }
}
