
#include "Random.hpp"

namespace Ilya
{
    std::mt19937 Random::engine;
    std::uniform_int_distribution<std::mt19937::result_type> Random::distribution;
}