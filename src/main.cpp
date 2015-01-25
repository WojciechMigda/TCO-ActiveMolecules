/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: main.cpp
 *
 * Description:
 *      description
 *
 * Authors:
 *          Wojciech Migda (wm)
 *
 *******************************************************************************
 * History:
 * --------
 * Date         Who  Ticket     Description
 * ----------   ---  ---------  ------------------------------------------------
 * 2015-01-06   wm              Initial version
 *
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <iterator>

#include "ActiveMolecules.hpp"

int main()
{
    ActiveMolecules active_molecules;

    int X;
    int Y;

    std::cin >> X >> Y;

    std::vector<double> similarities;
    similarities.reserve(4000);

    for (int i = 0; i < X + Y; i++)
    {
        similarities.clear();
        double d;

        auto sim_iterator = std::back_inserter(similarities);

        for (int j = 0; j < X + Y; j++)
        {
            std::cin >> d;
            *sim_iterator++ = d;
        }

        active_molecules.similarity(i, similarities);
    }

    std::vector<std::string> training_data;
    auto train_iterator = std::back_inserter(training_data);
    std::string s;

    for (int i = 0; i < X; i++)
    {
        std::cin >> s;
        *train_iterator++ = s;
    }

    std::vector<std::string> testing_data;
    auto test_iterator = std::back_inserter(testing_data);

    for (int i = 0; i < Y; i++)
    {
        std::cin >> s;
        *test_iterator++ = s;
    }

    auto result = active_molecules.rank(training_data, testing_data);

    std::copy(result.cbegin(), result.cend(), std::ostream_iterator<int>(std::cout, "\n"));

    std::cout << std::flush;

    return 0;
}
