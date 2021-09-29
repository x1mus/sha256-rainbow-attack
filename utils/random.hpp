#include <random>
#include <type_traits>
#include <memory>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <chrono>
#endif

namespace rainbow {

template<class T>
T random(T min = std::numeric_limits<T>::min(),
         T max = std::numeric_limits<T>::max(),
         bool reproducible = false)
{
    if (max < min)
        throw std::invalid_argument {"Maximum must be higher than minimum"};

    static std::random_device rd;

#ifndef _WIN32
    static std::mt19937 rng {rd()};
#else //on windows somehow if I don't do this, the same sequences are always output
    static std::mt19937 rng {static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())};
#endif

    static std::mt19937 rng_deterministic {};

    using DIST = std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;
    static DIST dist;

    if (dist.min() != min || dist.max() != max)
        dist.param(typename decltype(dist)::param_type {min, max});

    if(! reproducible)
        return dist(rng);
    else
        return dist(rng_deterministic);
}

template<class T>
std::shared_ptr<std::vector<T>> dense_distinct_random(int n, T min, T max, bool reproducible = false)
{
    static_assert(std::is_integral_v<T>, "Generated numbers must be integers");

    if (max < min)
        throw std::invalid_argument {"Maximum must be higher than minimum"};

    std::vector<T> from;
    for(int i = 0; i < max - min + 1; i++)
        from.push_back(max - min + i);

    std::shared_ptr<std::vector<T>> out;
    for(int i = 0; i < n; i++)
    {
        auto index = random(0, from.size(), reproducible);

        out->push_back(from[index]);
        from.erase(from.begin() + index);
    }

    return out;
}

template<class T>
std::shared_ptr<std::vector<T>> sparse_distinct_random(int n,  T min = std::numeric_limits<T>::min(),
                                                       T max = std::numeric_limits<T>::max(),
                                                       bool reproducible = false)
{
    if (max < min)
        throw std::invalid_argument {"Maximum must be higher than minimum"};

    std::shared_ptr<std::vector<T>> out = std::make_shared<std::vector<T>>();
    for(int i = 0; i < n; i++)
    {
        T nbr = rainbow::random(min, max, reproducible);

        while(std::find(out->begin(), out->end(), nbr) != out->end())//while we can find 'nbr' in 'out'
            nbr = rainbow::random(min, max, reproducible); //it works because we have few numbers to generate

        out->push_back(nbr);
    }

    return out;
}

}//namespace rainbow
