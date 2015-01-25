/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: ActiveMolecules.hpp
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

#ifndef ACTIVEMOLECULES_HPP_
#define ACTIVEMOLECULES_HPP_

#include "similarities_input_placeholder.hpp"
#include "molecule_input_placeholder.hpp"
#include "CP.hpp"
#include "matrix.hpp"
#include "algebra.hpp"

#include <vector>
#include <string>
#include <numeric>
#include <utility>
#include <tuple>
#include <memory>

// essential state
namespace
{
    SimilaritiesInputPlaceholder    g_similarities_input_placeholder;
}

struct ActiveMolecules
{
    typedef std::vector<std::string> molecule_array_type;

    int
    similarity(int & abs_index, std::vector<double> & row);

    std::vector<int>
    rank(
        molecule_array_type & training_data,
        molecule_array_type & testing_data);

private:
    std::vector<int> &&
    rank(
        molecule_array_type && training_data,
        molecule_array_type && testing_data,
        SimilaritiesInputPlaceholder && similarities_input_placeholder) const;
};

int
ActiveMolecules::similarity(int & abs_index, std::vector<double> & row)
{
    g_similarities_input_placeholder.takeFrom(abs_index, std::move(row));

    return abs_index;
}

std::vector<int>
ActiveMolecules::rank(
    ActiveMolecules::molecule_array_type & training_data,
    ActiveMolecules::molecule_array_type & testing_data)
{
    return rank(std::move(training_data), std::move(testing_data), std::move(g_similarities_input_placeholder));
}

std::vector<int> &&
ActiveMolecules::rank(
    ActiveMolecules::molecule_array_type && training_data,
    ActiveMolecules::molecule_array_type && testing_data,
    SimilaritiesInputPlaceholder && similarities_input_placeholder) const
{
    constexpr std::size_t ACTIVITY_INDEX{21};
    const molecule_array_type::size_type TESTING_DATA_SIZE = testing_data.size();
    const molecule_array_type::size_type TRAINING_DATA_SIZE = training_data.size();

    MoleculeInputPlaceholder molecules_for_training_input_placeholder;
    MoleculeInputPlaceholder molecules_for_testing_input_placeholder;

    molecules_for_training_input_placeholder.takeFrom(std::move(training_data));
    molecules_for_testing_input_placeholder.takeFrom(std::move(testing_data));

    const std::unique_ptr<Matrix2d<double>> similarities = similarities_input_placeholder.render();
    std::unique_ptr<Matrix2d<double>> unn_train_data = molecules_for_training_input_placeholder.render();
    std::unique_ptr<Matrix2d<double>> unn_test_data = molecules_for_testing_input_placeholder.render();

    const std::unique_ptr<Matrix2d<double>> train_data = normalize_columns(std::move(unn_train_data));
    const std::unique_ptr<Matrix2d<double>> test_data = normalize_columns(std::move(unn_test_data));
//    const std::unique_ptr<Matrix2d<double>> train_data{std::move(unn_train_data)};
//    const std::unique_ptr<Matrix2d<double>> test_data{std::move(unn_test_data)};

    const std::valarray<double> activities = train_data->col(ACTIVITY_INDEX);

    typedef std::tuple<std::size_t, double, std::valarray<double>> scored_tuple_type;
    std::vector<scored_tuple_type> scored_tuples;
    scored_tuples.reserve(TESTING_DATA_SIZE);

    for (std::size_t idx{0}; idx < TESTING_DATA_SIZE; ++idx)
    {
        scored_tuples.push_back(std::make_tuple(idx + train_data->rows(), 0.0, test_data->row(idx)));
    }

    std::unique_ptr<Matrix2d<double>> jaccards(new Matrix2d<double>(TRAINING_DATA_SIZE, TRAINING_DATA_SIZE));

    for (std::size_t iidx{0}; iidx < TRAINING_DATA_SIZE; ++iidx)
    {
        const std::valarray<double> irow = train_data->row(iidx);

        for (std::size_t jidx{iidx}; jidx < TRAINING_DATA_SIZE; ++jidx)
        {
            double v = jaccard(irow, train_data->row(jidx));
            jaccards->write(iidx, jidx, v);
            jaccards->write(jidx, iidx, v);
        }
    }

    for (std::size_t idx{0}; idx < TESTING_DATA_SIZE; ++idx)
    {
//        auto kernel = [](const double & lhs, const double & rhs)
//        {
////            return fabs(lhs) >= rhs;
//            return fabs(lhs) <= rhs;
////            return (lhs) >= rhs;
////            return (lhs) <= rhs;
//        };
        std::get<1>(scored_tuples[idx]) =
            APSsim(
                std::get<0>(scored_tuples[idx]),
                0.0,
                similarities,
                activities
            );
        std::get<1>(scored_tuples[idx]) +=
            APSsim(
                std::get<0>(scored_tuples[idx]),
                0.0,
                jaccards,
                activities
            );
    }

    std::sort(scored_tuples.begin(), scored_tuples.end(),
        [](const scored_tuple_type & lhs, const scored_tuple_type & rhs)
        {
            return std::get<1>(lhs) > std::get<1>(rhs);
        }
    );

    static std::vector<int> result;
    std::transform(scored_tuples.cbegin(), scored_tuples.cend(),
        std::back_inserter(result),
        [](const scored_tuple_type & item)
        {
            return std::get<0>(item);
        }
    );

    return std::move(result);
}

#endif /* ACTIVEMOLECULES_HPP_ */
