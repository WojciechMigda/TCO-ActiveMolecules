/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: algebra.hpp
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

#ifndef ALGEBRA_HPP_
#define ALGEBRA_HPP_

#include "matrix.hpp"
#include <utility>
#include <memory>
#include <cstddef>
#include <cassert>
#include <iterator>

template<typename _OutIterator>
std::size_t find_k_nearest_neighbours(
    const double * begin,
    const double * end,
    std::size_t knn,
    _OutIterator && out_iterator)
{
    assert(std::distance(begin, end) >= knn);

    typedef std::pair<std::size_t, double> pair_type;

    std::vector<pair_type> candidates;
    candidates.reserve(knn);

    const double * pos{begin};

    for (std::size_t cnt = 0; cnt < knn; ++cnt)
    {
        candidates.push_back(std::make_pair(cnt, *pos++));
    }

    auto ascending_predicate = [](const pair_type & lhs, const pair_type & rhs)
    {
        return lhs.second >= rhs.second;
//        return lhs.second <= rhs.second;
    };

    std::make_heap(candidates.begin(), candidates.end(), ascending_predicate);

    while (pos != end)
    {
        if (*pos >= candidates.front().second)
//        if (*pos <= candidates.front().second)
        {
            std::pop_heap(candidates.begin(), candidates.end(), ascending_predicate);

            candidates.back() = std::make_pair(std::distance(begin, pos), *pos);
            std::push_heap(candidates.begin(), candidates.end(), ascending_predicate);
        }

        ++pos;
    }

    std::sort(candidates.begin(), candidates.end(),
        [](const pair_type & lhs, const pair_type & rhs)
        {
            return lhs.second > rhs.second;
//            return lhs.second < rhs.second;
        }
    );

    for (auto pair : candidates)
    {
        *out_iterator++ = pair.first;
    }

    return knn;
}

template<typename _ValueType>
std::unique_ptr<Matrix2d<_ValueType>>
normalize_columns(std::unique_ptr<Matrix2d<_ValueType>> && matrix)
{
    typedef _ValueType value_type;
    typedef std::size_t size_type;

    for (size_type icol = 0; icol < matrix->cols(); ++icol)
    {
        std::valarray<value_type> column = matrix->col(icol);

        value_type mean = column.sum() / column.size();
        column -= mean;

        column /= sqrt(1.0 / (column.size() - 1) * (column * column).sum());

        matrix->col(icol) = column;
    }

    return std::move(matrix);
}

template<typename _ValueType, typename _OutIterator>
std::unique_ptr<Matrix2d<_ValueType>>
calculate_distances(
    std::valarray<_ValueType> && ref_item,
    std::valarray<_ValueType> && similarities,
    std::unique_ptr<Matrix2d<_ValueType>> && matrix,
    _OutIterator && out_iterator
    )
{
    typedef _ValueType value_type;

    if (1) // raw similarities
    {
        std::valarray<value_type> distances = /*1.0 - */similarities;

        std::copy(std::begin(distances), std::end(distances), out_iterator);
    }

    if (0)
    {
        for (std::size_t idx{0}; idx < matrix->rows(); ++idx)
        {
            value_type result{0};

            result +=  + (matrix->at(idx, 16) - ref_item[16]);
            result +=  - (matrix->at(idx, 15) - ref_item[15]);
            result +=  + (matrix->at(idx, 14) - ref_item[14]);

            result += 2.0 * similarities[idx];

            *out_iterator = result;
            ++out_iterator;
        }
    }

    return std::move(matrix);
}

#endif /* ALGEBRA_HPP_ */
