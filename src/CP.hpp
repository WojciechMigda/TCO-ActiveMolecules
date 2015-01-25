/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: CP.hpp
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
 * 2015-01-10   wm              Initial version
 *
 ******************************************************************************/

#ifndef CP_HPP_
#define CP_HPP_

#include "matrix.hpp"
#include "cache.hpp"

#include <cstddef>
#include <valarray>
#include <utility>
#include <memory>
#include <algorithm>

#pragma GCC optimize ( "-ffast-math" )
#pragma GCC optimize ( "-Ofast" )

template<typename _ValueType>
_ValueType distance(
    const std::valarray<_ValueType> & lhs,
    const std::valarray<_ValueType> & rhs
    )
{
    return sqrt((lhs * lhs).sum() + (rhs * rhs).sum() - 2 * (lhs * rhs).sum());
}

template<typename _ValueType>
_ValueType jaccard(
    const std::valarray<_ValueType> & lhs,
    const std::valarray<_ValueType> & rhs
    )
{
    const _ValueType inter = (lhs * rhs).sum();
    const _ValueType result = inter / ((lhs * lhs).sum() + (rhs * rhs).sum() - inter);

    return fabs(result);
}

template<typename _ValueType, std::size_t _N>
struct MinMaxIndexer
{
    typedef std::size_t size_type;
    typedef _ValueType value_type;

    static constexpr size_type N{_N};

    explicit MinMaxIndexer(const value_type & min, const value_type & max)
    :
        m_min(min),
        m_width(max)
    {

    }

    template<typename _Iterator>
    explicit MinMaxIndexer(const std::pair<_Iterator, _Iterator> & min_max)
    :
        m_min(*(min_max.first)),
        m_width(*(min_max.second) - m_min)
    {

    }

    inline
    size_type indexFor(const value_type & value) const
    {
        return round((value - m_min) / m_width * (N - 1));
    }

private:
    const value_type m_min;
    const value_type m_width;
};

template<typename _ValueType, typename _Compare>
_ValueType CP(
    const _ValueType distance,
    const _ValueType activity_thr_A_star,
    const std::valarray<_ValueType> & distances,
    const std::valarray<_ValueType> & activities,
    _Compare compare
    )
{
    typedef std::size_t size_type;
    typedef _ValueType value_type;

    auto compare_A = [](const double & lhs, const double & rhs)
    {
        return fabs(lhs) <= rhs;
    };

    auto constexpr round_up = [](const size_type & what, const size_type & mult) -> size_type
    {
        return
            what % mult ?
                what + mult - what % mult
                :
                what;
    };

    const size_type N = activities.size();

    size_type numerator{0};
    size_type denominator{0};

    std::valarray<std::pair<value_type, value_type>> local_data;
    local_data.resize(N);
    for (size_type idx{0}; idx < local_data.size(); ++idx)
    {
        local_data[idx] = std::make_pair(distances[idx], activities[idx]);
    }

    const std::pair<value_type, value_type> * local_data_p = &local_data[0];

    for (size_type iidx{0}; iidx < (N - 1); ++iidx)
    {
        value_type distance_iidx = local_data_p[iidx].first;
        value_type activity_iidx = local_data_p[iidx].second;

        constexpr size_type ALIGN_VALUE{8};
        const size_type total_trips = N - (iidx + 1);
        const size_type initial_trip_count = round_up(iidx + 1, ALIGN_VALUE) - (iidx + 1);
        const size_type initial_trip_limit = initial_trip_count + (iidx + 1);
        const size_type final_trip_count = (total_trips - initial_trip_count) % ALIGN_VALUE;
        const size_type core_trip_count = total_trips - initial_trip_count - final_trip_count;
        const size_type core_trip_limit = initial_trip_limit + core_trip_count;
//        std::cout << total_trips << " = " << initial_trip_count << " + " << core_trip_count << " + " << final_trip_count << ", start=" << (iidx + 1) << std::endl;
//        std::cout << initial_trip_limit << " / " << core_trip_limit << " / " << N << std::endl;

        size_type jidx{iidx + 1};

        for (; jidx < initial_trip_limit; ++jidx)
        {
            bool Dist_i_j_GE_min_dist_x = compare((distance_iidx - local_data_p[jidx].first), (distance));
            bool Delta_A_i_j_LE_A_star = compare_A((activity_iidx - local_data_p[jidx].second), activity_thr_A_star);

            denominator += Dist_i_j_GE_min_dist_x;
            numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
        }
        for (; jidx < core_trip_limit; jidx += ALIGN_VALUE)
        {
            for (size_type vidx{0}; vidx < ALIGN_VALUE; ++vidx)
            {
                bool Dist_i_j_GE_min_dist_x = compare((distance_iidx - local_data_p[jidx + vidx].first), (distance));
                bool Delta_A_i_j_LE_A_star = compare_A((activity_iidx - local_data_p[jidx + vidx].second), activity_thr_A_star);

                denominator += Dist_i_j_GE_min_dist_x;
                numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
            }
        }
        for (; jidx < N; ++jidx)
        {
            bool Dist_i_j_GE_min_dist_x = compare((distance_iidx - local_data_p[jidx].first), (distance));
            bool Delta_A_i_j_LE_A_star = compare_A((activity_iidx - local_data_p[jidx].second), activity_thr_A_star);

            denominator += Dist_i_j_GE_min_dist_x;
            numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
        }
    }

    const value_type result = denominator != 0 ? (value_type)numerator / denominator : 0.0;

    return result;
}


template<typename _ValueType, typename _Compare>
_ValueType APS(
    const std::size_t jidx,
    const _ValueType activity_thr_A_star,
    const std::valarray<_ValueType> & features,
    const std::valarray<_ValueType> & activities,
    _Compare compare
    )
{
    typedef std::size_t size_type;
    typedef _ValueType value_type;

    const std::valarray<value_type> distances = features - features[jidx];
//    distances.apply(fabs);

    Cache<value_type, 51> cache;
    MinMaxIndexer<value_type, 51> cache_indexer(std::minmax_element(std::begin(distances), std::end(distances)));

    std::valarray<value_type> CPs;
    CPs.resize(activities.size());

    for (std::size_t iidx{0}; iidx < CPs.size(); ++iidx)
    {
        size_type cache_idx = cache_indexer.indexFor(distances[iidx]);

        if (!cache.isOccupiedAt(cache_idx))
        {
            CPs[iidx] = CP(distances[iidx], activity_thr_A_star, distances, activities, compare);
            cache.write(cache_idx, CPs[iidx]);
        }
        else
        {
            CPs[iidx] = cache.read(cache_idx);
        }
//        std::cout << "CP " << iidx << ": " << CPs[iidx] << std::endl;
    }

    const value_type numerator = (activities * CPs).sum();
    const value_type denominator = CPs.sum();

    const value_type  result = numerator / denominator;

//    std::cout << "APS " << result << std::endl;

    return result;
}

template<typename _ValueType>
_ValueType CPsim(
    const _ValueType distance,
    const _ValueType activity_thr_A_star,
    const std::unique_ptr<Matrix2d<double>> & similarities,
    const std::valarray<_ValueType> & activities
    )
{
    typedef std::size_t size_type;
    typedef _ValueType value_type;

    auto compare_A = [](const double & lhs, const double & rhs)
    {
        return fabs(lhs) <= rhs;
    };

    auto constexpr round_up = [](const size_type & what, const size_type & mult) -> size_type
    {
        return
            what % mult ?
                what + mult - what % mult
                :
                what;
    };

    const size_type N = activities.size();

    size_type numerator{0};
    size_type denominator{0};

    for (size_type iidx{0}; iidx < (N - 1); ++iidx)
    {
        value_type activity_iidx = activities[iidx];

        constexpr size_type ALIGN_VALUE{8};
        const size_type total_trips = N - (iidx + 1);
        const size_type initial_trip_count = round_up(iidx + 1, ALIGN_VALUE) - (iidx + 1);
        const size_type initial_trip_limit = initial_trip_count + (iidx + 1);
        const size_type final_trip_count = (total_trips - initial_trip_count) % ALIGN_VALUE;
        const size_type core_trip_count = total_trips - initial_trip_count - final_trip_count;
        const size_type core_trip_limit = initial_trip_limit + core_trip_count;

        size_type jidx{iidx + 1};

        for (; jidx < initial_trip_limit; ++jidx)
        {
            bool Dist_i_j_GE_min_dist_x = similarities->at(iidx, jidx) >= distance;
            bool Delta_A_i_j_LE_A_star = compare_A(activity_iidx - activities[jidx], activity_thr_A_star);

            denominator += Dist_i_j_GE_min_dist_x;
            numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
        }
        for (; jidx < core_trip_limit; jidx += ALIGN_VALUE)
        {
            for (size_type vidx{0}; vidx < ALIGN_VALUE; ++vidx)
            {
                bool Dist_i_j_GE_min_dist_x = similarities->at(iidx, jidx) >= distance;
                bool Delta_A_i_j_LE_A_star = compare_A(activity_iidx - activities[jidx], activity_thr_A_star);

                denominator += Dist_i_j_GE_min_dist_x;
                numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
            }
        }
        for (; jidx < N; ++jidx)
        {
            bool Dist_i_j_GE_min_dist_x = similarities->at(iidx, jidx) >= distance;
            bool Delta_A_i_j_LE_A_star = compare_A(activity_iidx - activities[jidx], activity_thr_A_star);

            denominator += Dist_i_j_GE_min_dist_x;
            numerator += Delta_A_i_j_LE_A_star * Dist_i_j_GE_min_dist_x;
        }
    }

    const value_type result = denominator != 0 ? (value_type)numerator / denominator : 0.0;

    return result;
}

template<typename _ValueType>
_ValueType APSsim(
    const std::size_t jidx,
    const _ValueType activity_thr_A_star,
    const std::unique_ptr<Matrix2d<double>> & similarities,
    const std::valarray<_ValueType> & activities
    )
{
    typedef std::size_t size_type;
    typedef _ValueType value_type;

    Cache<value_type, 101> cache;
    MinMaxIndexer<value_type, 101> cache_indexer(0.0, 1.0);

    std::valarray<value_type> CPs;
    CPs.resize(activities.size());


    for (std::size_t iidx{0}; iidx < CPs.size(); ++iidx)
    {
        size_type cache_idx = cache_indexer.indexFor(similarities->at(iidx, jidx));
        if (!cache.isOccupiedAt(cache_idx))
        {
            CPs[iidx] = CPsim(similarities->at(iidx, jidx), activity_thr_A_star, similarities, activities);
            cache.write(cache_idx, CPs[iidx]);
        }
        else
        {
            CPs[iidx] = cache.read(cache_idx);
        }
//        std::cout << "CP " << iidx << ": " << CPs[iidx] << std::endl;
    }

    const value_type numerator = (activities * CPs).sum();
    const value_type denominator = CPs.sum();

    const value_type  result = numerator / denominator;

//    std::cout << "APSsim " << result << std::endl;

    return result;
}

#endif /* CP_HPP_ */
