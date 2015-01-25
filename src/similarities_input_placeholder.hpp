/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: similarities_input_placeholder.hpp
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

#ifndef SIMILARITIES_INPUT_PLACEHOLDER_HPP_
#define SIMILARITIES_INPUT_PLACEHOLDER_HPP_

#include "matrix.hpp"

#include <utility>
#include <vector>
#include <cstddef>
#include <memory>

struct SimilaritiesInputPlaceholder
{
private:
    typedef std::size_t size_type;
    typedef std::vector<double> row_type;
    typedef std::vector<row_type> array_type;

public:
    void takeFrom(const size_type index, row_type && row)
    {
        m_array.resize(row.size());

        m_array[index] = std::move(row);
    }

    std::unique_ptr<Matrix2d<double>> render() const
    {
        std::unique_ptr<Matrix2d<double>> result(new Matrix2d<double>(m_array.size(), m_array.size()));

        for (size_type index = 0; index < m_array.size(); ++index)
        {
            result->copyRowFrom(index, m_array[index].data(), m_array[index].data() + m_array[index].size());
        }

        return result;
    }

private:
    array_type m_array;
};

#endif /* SIMILARITIES_INPUT_PLACEHOLDER_HPP_ */
