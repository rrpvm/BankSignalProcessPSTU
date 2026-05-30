#pragma once
#include <vector>
#include <algorithm>
template <typename From, typename To, typename Mapper>
std::vector<To> mapVector(const std::vector<From>& source, Mapper mapper)
{
    std::vector<To> result;
    result.reserve(source.size());

    std::transform(
        source.begin(),
        source.end(),
        std::back_inserter(result),
        mapper
    );

    return result;
}