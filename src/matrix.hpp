/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: matrix.hpp
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

#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <cstddef>
#include <algorithm>
#include <valarray>

template<typename _Type>
struct Matrix2d
{
    typedef _Type value_type;
    typedef value_type * pointer;
    typedef const value_type * const_pointer;
    typedef std::size_t size_type;
    typedef std::valarray<value_type> column_type;
    typedef std::valarray<value_type> row_type;


    Matrix2d(size_type n_row, size_type n_col, value_type value = value_type())
    :
        m_n_row(n_row),
        m_n_col(n_col),
        m_eff_row_size(effective_row_size(n_col)),
        m_data(effective_nelem())
    {
    }

    void copyRowFrom(size_type row_index, const_pointer cbegin, const_pointer cend)
    {
        std::copy(cbegin, cend, row_begin(row_index));
    }

    pointer row_begin(const size_type & index)
    {
        return &m_data[row_start(index)];
    }

    pointer row_end(const size_type & index)
    {
        return row_begin(index) + m_n_col;
    }

    const_pointer row_cbegin(const size_type & index) const
    {
        return &m_data[row_start(index)];
    }

    const_pointer row_cend(const size_type & index) const
    {
        return row_cbegin(index) + m_n_col;
    }

    value_type at(const size_type row, const size_type column) const
    {
        return *(row_cbegin(row) + column);
    }

    void write(const size_type row, const size_type column, value_type value)
    {
        *(row_begin(row) + column) = value;
    }

    size_type rows() const
    {
        return m_n_row;
    }

    size_type cols() const
    {
        return m_n_col;
    }

    std::valarray<value_type> row(const size_type index) const
    {
        return m_data[std::slice(index * m_eff_row_size, m_n_col, 1)];
    }

    std::valarray<value_type> col(const size_type index) const
    {
        return m_data[std::slice(index , m_n_row, m_eff_row_size)];
    }

    std::slice_array<value_type> col(const size_type index)
    {
        return m_data[std::slice(index , m_n_row, m_eff_row_size)];
    }

    ~Matrix2d()
    {
    }

private:
    size_type effective_row_size(const size_type & x_dim) const
    {
        auto constexpr round_up = [](const size_type & what, const size_type & mult) -> size_type
        {
            return
                what % mult ?
                    what + mult - what % mult
                    :
                    what;
        };

        return round_up(x_dim, 512);
    }

    size_type effective_nelem() const
    {
        return m_n_row * m_eff_row_size;
    }

    size_type row_start(const size_type & index) const
    {
        return index * m_eff_row_size;
    }

private:
    const size_type m_n_row;
    const size_type m_n_col;
    const size_type m_eff_row_size;
    std::valarray<value_type> m_data;
};

#endif /* MATRIX_HPP_ */
