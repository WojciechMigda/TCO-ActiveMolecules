/*******************************************************************************
 * Copyright (c) 2015 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the GNU LGPL v3
 *******************************************************************************
 *
 * Filename: molecule_input_placeholder.hpp
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

#ifndef MOLECULE_INPUT_PLACEHOLDER_HPP_
#define MOLECULE_INPUT_PLACEHOLDER_HPP_

#include <vector>
#include <string>
#include <utility>
#include <cstddef>
#include <cstdio>
#include <sstream>
#include <memory>

struct MoleculeInputPlaceholder
{
    typedef std::size_t size_type;
    typedef std::vector<std::string> array_type;
    static constexpr size_type N_COL{22};

    void takeFrom(array_type && array)
    {
        m_array = std::move(array);
    }

    std::unique_ptr<Matrix2d<double>> render() const
    {
        std::unique_ptr<Matrix2d<double>> result(new Matrix2d<double>(m_array.size(), N_COL));

        for (size_type row = 0; row < m_array.size(); ++row)
        {
            std::stringstream stream(m_array[row]);
            std::string token;
            for (size_type index = 0; (index < 14) && std::getline(stream, token, ','); ++index)
            {
                sscanf(token.c_str(), "%lf", result->row_begin(row) + index);
            }

            std::getline(stream, token, ','); // skip formula

            for (size_type index = 14; (index < 22) && std::getline(stream, token, ','); ++index)
            {
                sscanf(token.c_str(), "%lf", result->row_begin(row) + index);
            }
        }

        return result;
    }

private:
    array_type m_array;
};

#endif /* MOLECULE_INPUT_PLACEHOLDER_HPP_ */
