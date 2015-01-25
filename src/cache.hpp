/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: cache.hpp
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
 * 2015-01-11   wm              Initial version
 *
 ******************************************************************************/

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <vector>

template<typename _ValueType, std::size_t _N>
struct Cache
{
    typedef _ValueType value_type;
    typedef std::size_t size_type;
    static constexpr size_type N{_N};

    explicit Cache(const value_type & init_value = -1.0)
    :
        m_occupied(N, false),
        m_vec(N, init_value),
        m_occupied_p(m_occupied.data()),
        m_vec_p(m_vec.data())
    {

    }

    inline
    value_type read(size_type index) const
    {
        return m_vec_p[index];
    }

    inline
    void write(size_type index, value_type value)
    {
        m_vec_p[index] = value;
        m_occupied_p[index] = true;
    }

    inline
    bool isOccupiedAt(size_type index) const
    {
        return m_occupied_p[index] == true;
    }

private:
    std::vector<char> m_occupied;
    std::vector<value_type> m_vec;
    char * m_occupied_p;
    value_type * m_vec_p;
};

#endif /* CACHE_HPP_ */
