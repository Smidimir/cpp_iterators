#include "bidirectional_iterator.h"


#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include <set>




int main()
{
    auto x = std::vector{1, 2, 3, 4, 5, 6};

    auto it_cbegin = x.begin();
    auto uit_cbegin = bidirectional_iterator{it_cbegin};

    auto t = std::tuple
            { ++uit_cbegin
            , uit_cbegin++
            , --uit_cbegin
            , uit_cbegin++
            , uit_cbegin += 1
            , uit_cbegin -= 1
            , uit_cbegin + 1
            , uit_cbegin - 1
            , *uit_cbegin
            , uit_cbegin[1] };


    auto it_begin = x.begin();
    auto it_rbegin = x.rbegin();



    auto uit_begin = bidirectional_iterator(it_begin);
    auto uit_rbegin = bidirectional_iterator(it_rbegin);

    uit_rbegin.stable_invert();


    std::cout << *uit_rbegin << " " << std::distance(uit_begin, uit_rbegin) << std::endl;
    std::cout << std::distance(it_begin, invert_iterator(it_rbegin)) << std::endl;

    auto itVar = bidirectional_iterator(x.begin());

    for(std::size_t i = 0; i < 10; ++i)
    {
        while(itVar != std::end(x))
            std::cout << *(itVar++) << std::endl;

        itVar = std::rbegin(x);

        while(itVar != std::rend(x))
            std::cout << *(itVar++) << std::endl;

        itVar = std::begin(x);
    }



    return 0;
}