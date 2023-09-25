#include <assert.h>
#include "defs.h"
#include "utils.h"
#include <stdio.h>

// Gets branch type from instruction
static BRANCH_TYPE getBranchType(int instruction) {
    // Get bits that determine branch type
    int determiningbits = getBitsAt(instruction, BR_DET_BITS_START, BR_DET_BITS_LEN);

    switch (determiningbits) {
        case BR_DET_BITS_UNCOND:
            return UNCONDITIONAL;
        case BR_DET_BITS_REG:
            return REGISTER;
        case BR_DET_BITS_COND:
            return CONDITIONAL;
        default:
            // Not valid branch.
            return -1;
    }
}

// Determine if ARM PSTATE satisfies cond
static bool conditionCheck(int cond, ARM* arm) {
    int n = arm->pstate.N;
    int z = arm->pstate.Z;
    int v = arm->pstate.V;

    switch (cond) {
        // EQ (Equal)
        case BR_EQ:
            return (z == 1);
        // NE (Not Equal)
        case BR_NE:
            return (z == 0);
        // GE (Signed greater or equal)
        case BR_GE:
            return (n == 1);
        // LT (Signed less than)
        case BR_LT:
            return (n != 1);
        // GT (Signed greater than)
        case BR_GT:
            return (z == 0 && n == v);
        // LE (Signed less than or equal)
        case BR_LE:
            return (!(z == 0 && n == v));
        // AL (always)
        case BR_AL:
            return 1;
        // Condition not of permitted type
        default:
            return 0;
    }
}


// Execute branch instruction
void branch(ARM* arm, int instruction) {

    // Get type of branch instruction
    BRANCH_TYPE type = getBranchType(instruction);

    switch (type) {
        case UNCONDITIONAL: {
            int64_t simm26 = extendBits(getBitsAt(instruction, BR_SIMM26_START, SIMM26_LEN), SIMM26_LEN);
            int64_t offset = simm26 * BYTES_IN_WORD;
            // Branch to address encoded by literal
            arm->pc += offset;
            arm->pc -= INSTRUCTION_SIZE;
            break;
        }
        case REGISTER: {
            // Determining encoding of register Xn
            int xn = getBitsAt(instruction, BR_XN_START, REG_INDEX_SIZE);
            // Check if xn refers to an exisiting register
            assert(xn >= 0 && xn < NUM_OF_REGISTERS);
            // Branch to address stored in Xn
            arm->pc = arm->registers[xn];
            arm->pc -= INSTRUCTION_SIZE;
            break;
        }
        case CONDITIONAL: {
            int64_t simm19 = extendBits(getBitsAt(instruction, BR_SIMM19_START, SIMM19_LEN), SIMM19_LEN);
            int cond = getBitsAt(instruction, BR_COND_START, BR_COND_LEN);
            if (conditionCheck(cond, arm)) {
                int64_t offset = simm19 * BYTES_IN_WORD;
                // Branch to address encoded by literal
                arm->pc += offset;
                arm->pc -= INSTRUCTION_SIZE;
            }
            break;
        }
    }
}
