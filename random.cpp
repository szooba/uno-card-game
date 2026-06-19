#include "random.h"
#include <random>


namespace Random {
    static std::mt19937& get_engine() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    int get_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(get_engine());
    }
    int random_type()
    {
        std::discrete_distribution<int> dist({ 76, 8, 8, 12, 4 });
        return dist(get_engine());
    }
}