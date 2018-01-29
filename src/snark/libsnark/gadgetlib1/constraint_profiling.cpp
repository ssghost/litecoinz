/** @file
 *****************************************************************************

 Implementation of interfaces for profiling constraints.

 See constraint_profiling.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include "gadgetlib1/constraint_profiling.hpp"
#include "common/profiling.hpp"

namespace libsnark {

uint64_t constraint_profiling_indent = 0;
std::vector<constraint_profiling_entry> constraint_profiling_table;

uint64_t PRINT_CONSTRAINT_PROFILING()
{
    uint64_t accounted = 0;
    print_indent();
    printf("Constraint profiling:\n");
    for (constraint_profiling_entry &ent : constraint_profiling_table)
    {
        if (ent.indent == 0)
        {
            accounted += ent.count;
        }

        print_indent();
        for (uint64_t i = 0; i < ent.indent; ++i)
        {
            printf("  ");
        }
        printf("* Number of constraints in [%s]: %zu\n", ent.annotation.c_str(), ent.count);
    }

    constraint_profiling_table.clear();
    constraint_profiling_indent = 0;

    return accounted;
}

}
