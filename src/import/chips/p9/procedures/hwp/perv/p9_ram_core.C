/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/perv/p9_ram_core.C $       */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2019                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

///----------------------------------------------------------------------------
///
/// @file p9_ram_core.C
/// @brief Class that implements the base ramming capability
///
///----------------------------------------------------------------------------
/// *HWP HWP Owner        : Liu Yang Fan <shliuyf@cn.ibm.com>
/// *HWP HWP Backup Owner : Gou Peng Fei <shgoupf@cn.ibm.com>
/// *HWP FW Owner         : Thi Tran <thi@us.ibm.com>
/// *HWP Team             : Perv
/// *HWP Level            : 3
/// *HWP Consumed by      : SBE

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <p9_ram_core.H>
#include <p9_quad_scom_addresses.H>
#include <p9_quad_scom_addresses_fld.H>

// identifiers for special registers
const uint32_t RAM_REG_NIA   = 2000;
const uint32_t RAM_REG_MSR   = 2001;
const uint32_t RAM_REG_CR    = 2002;
const uint32_t RAM_REG_FPSCR = 2003;
const uint32_t RAM_REG_VSCR  = 2004;
const uint32_t RAM_REG_SLBE  = 2005;
const uint32_t RAM_REG_SLBV  = 2006;

// opcode for ramming
const uint32_t OPCODE_MTSPR_FROM_GPR0_TO_SPRD      = 0x7C1543A6;
const uint32_t OPCODE_MTSPR_FROM_GPR1_TO_SPRD      = 0x7C3543A6;
const uint32_t OPCODE_MFSPR_FROM_SPRD_TO_GPR0      = 0x7C1542A6;
const uint32_t OPCODE_MFSPR_FROM_SPRD_TO_GPR1      = 0x7C3542A6;
const uint32_t OPCODE_MFSPR_FROM_SPR0_TO_GPR0      = 0x7C0002A6;
const uint32_t OPCODE_MTSPR_FROM_GPR0_TO_SPR0      = 0x7C0003A6;
const uint32_t OPCODE_MFFPRD_FROM_FPR0_TO_GPR0     = 0x7C000066;
const uint32_t OPCODE_MTFPRD_FROM_GPR0_TO_FPR0     = 0x7C000166;
const uint32_t OPCODE_MFVSRD_FROM_VSR0_TO_GPR0     = 0x7C000066;
const uint32_t OPCODE_MFVSRD_FROM_VSR32_TO_GPR0    = 0x7C000067;
const uint32_t OPCODE_MFVSRLD_FROM_VSR0_TO_GPR0    = 0x7C000266;
const uint32_t OPCODE_MFVSRLD_FROM_VSR32_TO_GPR0   = 0x7C000267;
const uint32_t OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR0  = 0x7C010366;
const uint32_t OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR32 = 0x7C010367;
const uint32_t OPCODE_MFSPR_FROM_LR_TO_GPR0        = 0x7C0802A6;
const uint32_t OPCODE_MTSPR_FROM_GPR0_TO_LR        = 0x7C0803A6;
const uint32_t OPCODE_MTMSR_L0                     = 0x7C000124;
const uint32_t OPCODE_MTMSRD_L0                    = 0x7C000164;
const uint32_t OPCODE_MTSPR_IAMR                   = 0x7C1D0BA6;
const uint32_t OPCODE_MTSPR_PIDR                   = 0x7C100BA6;
const uint32_t OPCODE_MTSPR_LPIDR                  = 0x7C1F4BA6;
const uint32_t OPCODE_MTSPR_LPCR                   = 0x7C1E4BA6;
const uint32_t OPCODE_MTSPR_MMCRA                  = 0x7C12C3A6;
const uint32_t OPCODE_MTSPR_MMCR1                  = 0x7C1EC3A6;
const uint32_t OPCODE_MTSPR_SEIDBAR                = 0x7C1F7BA6;
const uint32_t OPCODE_MTSPR_XER                    = 0x7C0103A6;
const uint32_t OPCODE_MFSPR_XER                    = 0x7C0102A6;
const uint32_t OPCODE_MFFS                         = 0xFC00048E;
const uint32_t OPCODE_SLBMFEE                      = 0x7C000726;
const uint32_t OPCODE_SLBMFEV                      = 0x7C0006A6;
const uint32_t OPCODE_DCBZ                         = 0x7C0007EC;
const uint32_t OPCODE_DCBF                         = 0x7C0000AC;
const uint32_t OPCODE_LD                           = 0xE8000000;
const uint32_t OPCODE_STD                          = 0xF8000000;
const uint32_t OPCODE_LFD                          = 0xC8000000;
const uint32_t OPCODE_STFD                         = 0xD8000000;
const uint32_t OPCODE_LVX                          = 0x7C0000CE;
const uint32_t OPCODE_STVX                         = 0x7C0001CE;
const uint32_t OPCODE_LXVD2X                       = 0x7C000698;
const uint32_t OPCODE_STXVD2X                      = 0x7C000798;
const uint32_t OPCODE_MFMSR_TO_GPR0                = 0x7C0000A6;
const uint32_t OPCODE_MFOCRF_TO_GPR0               = 0x7C100026;
const uint32_t OPCODE_MTOCRF_FROM_GPR0             = 0x7C100120;
const uint32_t OPCODE_MTFSF_FROM_FPR0              = 0xFE00058E;
const uint32_t OPCODE_MFVSCR_TO_VR0                = 0x10000604;
const uint32_t OPCODE_MTVSCR_FROM_VR0              = 0x10000644;
const uint32_t OPCODE_MFNIA_RT                     = 0x001ac804;
const uint32_t OPCODE_MTNIA_LR                     = 0x4c1e00a4;
const uint32_t OPCODE_GPR_MOVE                     = 0x00000010;
const uint32_t OPCODE_VSR_MOVE_HI                  = 0x00000110;
const uint32_t OPCODE_VSR_MOVE_LO                  = 0x00000210;
const uint32_t OPCODE_XER_MOVE                     = 0x00000310;
const uint32_t OPCODE_CR_MOVE                      = 0x00000410;

// poll count for check ram status
const uint32_t RAM_CORE_STAT_POLL_CNT = 10;

//-----------------------------------------------------------------------------------
// Function definitions
// Note: All function doxygen can be found in class definition in header file.
//-----------------------------------------------------------------------------------
RamCore::RamCore(const fapi2::Target<fapi2::TARGET_TYPE_CORE>& i_target,
                 const uint8_t i_thread)
{
    iv_target = i_target;
    iv_thread = i_thread;
    iv_ram_enable    = false;
    iv_ram_scr0_save = false;
    iv_ram_setup     = false;
    iv_ram_err       = false;
    iv_write_gpr0    = false;
    iv_write_gpr1    = false;
    iv_backup_buf0   = 0;
    iv_backup_buf1   = 0;
    iv_backup_buf2   = 0;
}

// Sequence of operations in put reg and get reg
// functions follow the below template of a putscom
// before the opcode ramming and a getscom post that
// or a combination of these. This structure can be
// used to streamline these operations with an array
// of this structure.
// If the pointers are null, corresponding operations are skipped.
typedef struct
{
    // putscom to be done before ramming opcode
    fapi2::buffer<uint64_t>* prePutScom;
    // ramming opcode
    uint32_t opCode;
    // getscom to be done after ramming opcode
    fapi2::buffer<uint64_t>* postGetScom;
} putreg_op_t;

RamCore::~RamCore()
{
    if(iv_ram_setup)
    {
        FAPI_ERR("RamCore Destructor error: Ram is still in active state!!!");
    }
}

//-----------------------------------------------------------------------------------
fapi2::ReturnCode RamCore::ram_setup()
{
    FAPI_DBG("Start ram setup");
    fapi2::buffer<uint64_t> l_data = 0;
    uint32_t l_opcode = 0;
    bool l_thread_active = false;
    uint8_t l_thread_stop = 0;

    // set RAM_MODEREG Scom to enable RAM mode
    FAPI_TRY(fapi2::getScom(iv_target, C_RAM_MODEREG, l_data));
    l_data.setBit<C_RAM_MODEREG_MODE_ENABLE>();
    FAPI_TRY(fapi2::putScom(iv_target, C_RAM_MODEREG, l_data));

    // read RAS_STATUS Scom to check the thread is stopped for ramming
    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_RAS_STATUS, l_data));
    FAPI_DBG("RAS_STATUS:%#lx", l_data());
    FAPI_TRY(l_data.extractToRight(l_thread_stop,
                                   C_RAS_STATUS_T0_CORE_MAINT + 8 * iv_thread, 2));
    FAPI_ASSERT(l_thread_stop == 3,
                fapi2::P9_RAM_THREAD_NOT_STOP_ERR()
                .set_CORE_TARGET(iv_target)
                .set_THREAD(iv_thread),
                "Thread to perform ram is not stopped. "
                "C_RAS_STATUS reg 0x%.16llX", l_data);

    // read THREAD_INFO Scom to check the thread is active for ramming
    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_THREAD_INFO, l_data));
    FAPI_DBG("THREAD_INFO:%#lx", l_data());
    FAPI_TRY(l_data.extractToRight(l_thread_active,
                                   C_THREAD_INFO_VTID0_V + iv_thread, 1));

    if (!l_thread_active)
    {
        FAPI_TRY(l_data.setBit(C_THREAD_INFO_RAM_THREAD_ACTIVE + iv_thread));
        FAPI_TRY(fapi2::putScom(iv_target, C_THREAD_INFO, l_data));
        FAPI_TRY(fapi2::getScom(iv_target, C_THREAD_INFO, l_data));
        FAPI_TRY(l_data.extractToRight(l_thread_active,
                                       C_THREAD_INFO_VTID0_V + iv_thread, 1));
    }

    FAPI_ASSERT(l_thread_active,
                fapi2::P9_RAM_THREAD_INACTIVE_ERR()
                .set_CORE_TARGET(iv_target)
                .set_THREAD(iv_thread),
                "Thread to perform ram is inactive. "
                "C_THREAD_INFO reg 0x%.16llX", l_data);

    iv_ram_enable = true;

    // backup registers SCR0/GPR0/GPR1
    //SCR0->iv_backup_buf0
    FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, iv_backup_buf0));
    iv_ram_scr0_save = true;

    //GPR0->iv_backup_buf1
    //1.setup SPRC to use SCRO as SPRD
    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_SPR_MODE, l_data));
    l_data.insertFromRight<C_SPR_MODE_MODEREG_SPRC_LT0_SEL, 8>(0xFF);
    FAPI_TRY(fapi2::putScom(iv_target, C_SPR_MODE, l_data));

    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_SCOMC, l_data));
    l_data.insertFromRight<C_SCOMC_MODE_CX, C_SCOMC_MODE_CX_LEN>(0);
    FAPI_TRY(fapi2::putScom(iv_target, C_SCOMC, l_data));

    //2.create mtsprd<gpr0> opcode, ram into thread to get GPR0
    l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
    FAPI_TRY(ram_opcode(l_opcode, true));

    //3.get GPR0 from SCR0
    FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, iv_backup_buf1));

    //GPR1->iv_backup_buf2
    //1.create mtsprd<gpr1> opcode, ram into thread to get GPR1
    l_opcode = OPCODE_MTSPR_FROM_GPR1_TO_SPRD;
    FAPI_TRY(ram_opcode(l_opcode, true));

    //2.get GPR1 from SCR0
    FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, iv_backup_buf2));

    iv_ram_setup = true;

fapi_try_exit:

    // Error happened and SCR0 saved, to restore SCR0
    // Do not use "FAPI_TRY" to avoid endless loop
    if((fapi2::current_err != fapi2::FAPI2_RC_SUCCESS) && iv_ram_scr0_save)
    {
        fapi2::putScom(iv_target, C_SCR0, iv_backup_buf0);
    }

    FAPI_DBG("Exiting ram setup");
    return fapi2::current_err;
}

//-----------------------------------------------------------------------------------
fapi2::ReturnCode RamCore::ram_cleanup()
{
    FAPI_DBG("Start ram cleanup");
    uint32_t l_opcode = 0;
    fapi2::buffer<uint64_t> l_data = 0;

    FAPI_ASSERT(iv_ram_setup,
                fapi2::P9_RAM_NOT_SETUP_ERR()
                .set_CORE_TARGET(iv_target),
                "Attempting to cleanup ram without setup before");

    // setup SPRC to use SCRO as SPRD
    FAPI_TRY(fapi2::getScom(iv_target, C_SPR_MODE, l_data));
    l_data.insertFromRight<C_SPR_MODE_MODEREG_SPRC_LT0_SEL, 8>(0xFF);
    FAPI_TRY(fapi2::putScom(iv_target, C_SPR_MODE, l_data));

    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_SCOMC, l_data));
    l_data.insertFromRight<C_SCOMC_MODE_CX, C_SCOMC_MODE_CX_LEN>(0);
    FAPI_TRY(fapi2::putScom(iv_target, C_SCOMC, l_data));

    // restore GPR1
    if(!iv_write_gpr1)
    {
        //iv_backup_buf2->GPR1
        //1.put restore data into SCR0
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, iv_backup_buf2));

        //2.create mfsprd<gpr1> opcode, ram into thread to restore GPR1
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR1;
        FAPI_TRY(ram_opcode(l_opcode, true));
    }

    // restore GPR0
    if(!iv_write_gpr0)
    {
        //iv_backup_buf1->GPR0
        //1.put restore data into SCR0
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, iv_backup_buf1));

        //2.create mfsprd<gpr0> opcode, ram into thread to restore GPR0
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR0;
        FAPI_TRY(ram_opcode(l_opcode, true));
    }

    //iv_backup_buf0->SCR0
    FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, iv_backup_buf0));

    // set RAM_MODEREG Scom to clear RAM mode
    l_data.flush<0>();
    FAPI_TRY(fapi2::getScom(iv_target, C_RAM_MODEREG, l_data));
    l_data.clearBit<C_RAM_MODEREG_MODE_ENABLE>();
    FAPI_TRY(fapi2::putScom(iv_target, C_RAM_MODEREG, l_data));

    iv_ram_enable    = false;
    iv_ram_scr0_save = false;
    iv_ram_setup     = false;
    iv_write_gpr0    = false;
    iv_write_gpr1    = false;

fapi_try_exit:
    FAPI_DBG("Exiting ram cleanup");
    return fapi2::current_err;
}

//-----------------------------------------------------------------------------------
fapi2::ReturnCode RamCore::ram_opcode(const uint32_t i_opcode,
                                      const bool i_allow_mult)
{
    FAPI_DBG("Start ram opcode");
    fapi2::buffer<uint64_t> l_data = 0;
    uint8_t  l_predecode = 0;
    uint32_t l_poll_count = RAM_CORE_STAT_POLL_CNT;
    bool l_is_load_store = false;

    // check the opcode for load/store
    l_is_load_store = is_load_store(i_opcode);

    // ram_setup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_setup());
    }

    FAPI_ASSERT(iv_ram_enable,
                fapi2::P9_RAM_NOT_SETUP_ERR()
                .set_CORE_TARGET(iv_target),
                "Attempting to ram opcode without enable RAM mode before");

    // write RAM_CTRL Scom for ramming the opcode
    l_data.insertFromRight<C_RAM_CTRL_RAM_VTID, C_RAM_CTRL_RAM_VTID_LEN>(iv_thread);
    l_predecode = gen_predecode(i_opcode);
    l_data.insertFromRight<C_RAM_CTRL_PPC_PREDCD, C_RAM_CTRL_PPC_PREDCD_LEN>(l_predecode);
    l_data.insertFromRight<C_RAM_CTRL_PPC_INSTR, C_RAM_CTRL_PPC_INSTR_LEN>(i_opcode);
    FAPI_TRY(fapi2::putScom(iv_target, C_RAM_CTRL, l_data));

    // poll RAM_STATUS_REG Scom for the completion
    l_data.flush<0>();

    while(1)
    {
        FAPI_TRY(fapi2::getScom(iv_target, C_RAM_STATUS, l_data));

        // attempting to ram during recovery
        FAPI_ASSERT(!l_data.getBit<C_RAM_STATUS_RAM_CONTROL_ACCESS_DURING_RECOV>(),
                    fapi2::P9_RAM_STATUS_IN_RECOVERY_ERR()
                    .set_CORE_TARGET(iv_target),
                    "Attempting to ram during recovery. "
                    "C_RAM_STATUS reg 0x%.16llX", l_data);

        // exception or interrupt
        FAPI_ASSERT(!l_data.getBit<C_RAM_STATUS_RAM_EXCEPTION>(),
                    fapi2::P9_RAM_STATUS_EXCEPTION_ERR()
                    .set_CORE_TARGET(iv_target),
                    "Exception or interrupt happened during ramming. "
                    "C_RAM_STATUS reg 0x%.16llX", l_data);

        // load/store opcode need to check LSU empty and PPC complete
        if (l_is_load_store)
        {
            if ( l_data.getBit<C_RAM_STATUS_RAM_COMPLETION>() &&
                 l_data.getBit<C_RAM_STATUS_LSU_EMPTY>() )
            {
                FAPI_DBG("ram_opcode:: RAM is done");
                break;
            }
        }
        else
        {
            if ( l_data.getBit<C_RAM_STATUS_RAM_COMPLETION>() )
            {
                FAPI_DBG("ram_opcode:: RAM is done");
                break;
            }
        }

        --l_poll_count;

        FAPI_ASSERT(l_poll_count > 0,
                    fapi2::P9_RAM_STATUS_POLL_THRESHOLD_ERR()
                    .set_CORE_TARGET(iv_target),
                    "Timeout for ram to complete, poll count expired. "
                    "C_RAM_STATUS reg 0x%.16llX", l_data);
    }

    // ram_cleanup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_cleanup());
    }

fapi_try_exit:

    if(fapi2::current_err != fapi2::FAPI2_RC_SUCCESS)
    {
        iv_ram_err = true;
    }

    FAPI_DBG("Exiting ram opcode");
    return fapi2::current_err;
}

//-----------------------------------------------------------------------------------
uint8_t RamCore::gen_predecode(const uint32_t i_opcode)
{
    uint8_t  l_predecode = 0;
    uint32_t l_opcode_pattern0 = i_opcode & 0xFC0007FE;
    uint32_t l_opcode_pattern1 = i_opcode & 0xFC1FFFFE;

    if((i_opcode == OPCODE_MFNIA_RT)    ||
       (i_opcode == OPCODE_GPR_MOVE)    ||
       (i_opcode == OPCODE_VSR_MOVE_HI) ||
       (i_opcode == OPCODE_VSR_MOVE_LO) ||
       (i_opcode == OPCODE_XER_MOVE)    ||
       (i_opcode == OPCODE_CR_MOVE))
    {
        l_predecode = 2;
    }
    else if((i_opcode == OPCODE_MTNIA_LR)             ||
            (l_opcode_pattern0 == OPCODE_MTMSR_L0)    ||
            (l_opcode_pattern0 == OPCODE_MTMSRD_L0))
    {
        l_predecode = 8;
    }
    else if((l_opcode_pattern1 == OPCODE_MTSPR_IAMR)    ||
            (l_opcode_pattern1 == OPCODE_MTSPR_PIDR)    ||
            (l_opcode_pattern1 == OPCODE_MTSPR_LPIDR)   ||
            (l_opcode_pattern1 == OPCODE_MTSPR_LPCR)    ||
            (l_opcode_pattern1 == OPCODE_MTSPR_MMCRA)   ||
            (l_opcode_pattern1 == OPCODE_MTSPR_MMCR1)   ||
            (l_opcode_pattern1 == OPCODE_MTSPR_SEIDBAR) ||
            (l_opcode_pattern1 == OPCODE_MTSPR_XER)     ||
            (l_opcode_pattern1 == OPCODE_MFSPR_XER)     ||
            (l_opcode_pattern0 == OPCODE_MFFS)          ||
            (l_opcode_pattern0 == OPCODE_SLBMFEE)       ||
            (l_opcode_pattern0 == OPCODE_SLBMFEV))
    {
        l_predecode = 4;
    }

    return l_predecode;
}

//-----------------------------------------------------------------------------------
bool RamCore::is_load_store(const uint32_t i_opcode)
{
    bool l_load_store = false;
    uint32_t l_opcode_pattern0 = i_opcode & 0xFC0007FE;
    uint32_t l_opcode_pattern1 = i_opcode & 0xFC000000;

    if((l_opcode_pattern0 == OPCODE_DCBZ)   ||
       (l_opcode_pattern0 == OPCODE_DCBF)   ||
       (l_opcode_pattern1 == OPCODE_LD)     ||
       (l_opcode_pattern1 == OPCODE_LFD)    ||
       (l_opcode_pattern1 == OPCODE_STD)    ||
       (l_opcode_pattern1 == OPCODE_LFD)    ||
       (l_opcode_pattern1 == OPCODE_STFD)   ||
       (l_opcode_pattern0 == OPCODE_LVX)    ||
       (l_opcode_pattern0 == OPCODE_STVX)   ||
       (l_opcode_pattern0 == OPCODE_LXVD2X) ||
       (l_opcode_pattern0 == OPCODE_STXVD2X))
    {
        l_load_store = true;
    }

    return l_load_store;
}

//-----------------------------------------------------------------------------------
fapi2::ReturnCode RamCore::get_reg(const Enum_RegType i_type,
                                   const uint32_t i_reg_num,
                                   fapi2::buffer<uint64_t>* o_buffer,
                                   const bool i_allow_mult)
{
    FAPI_DBG("Start get register");
    uint32_t l_opcode = 0;
    uint32_t l_spr_regnum_lo = 0;
    uint32_t l_spr_regnum_hi = 0;
    fapi2::buffer<uint64_t> l_backup_gpr0 = 0;
    fapi2::buffer<uint64_t> l_backup_fpr0 = 0;
    fapi2::buffer<uint64_t> l_backup_vr0_dw0 = 0;
    fapi2::buffer<uint64_t> l_backup_vr0_dw1 = 0;
    fapi2::buffer<uint64_t> l_crf_data = 0;
    uint64_t l_cr_data = 0;
    uint32_t l_cr_num = 0;
    static const size_t MAX_OPCODES = 10;
    putreg_op_t opcodes[MAX_OPCODES] = {};
    bool array_op = false;

    // ram_setup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_setup());
    }

    FAPI_ASSERT(iv_ram_setup,
                fapi2::P9_RAM_NOT_SETUP_ERR()
                .set_CORE_TARGET(iv_target)
                .set_REG_TYPE(i_type)
                .set_REG_NUM(i_reg_num)
                .set_ALLOW_MULT(i_allow_mult),
                "Attempting to cleanup ram without setup before. "
                "Reg type: %u, Reg num: %u, Allow Mult: %u",
                i_type, i_reg_num, i_allow_mult);

    //backup GPR0 if it is written
    if(iv_write_gpr0)
    {
        l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
        FAPI_TRY(ram_opcode(l_opcode, true));
        FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, l_backup_gpr0));
    }

    // get register value
    if(i_type == REG_GPR)
    {
        //1.create mtsprd<i_reg_num> opcode, ram into thread
        //2.get GPR value from SCR0
        opcodes[0] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD + (i_reg_num << 21), &o_buffer[0]};

        array_op = true;
    }
    else if(i_type == REG_SPR)
    {
        if(i_reg_num == RAM_REG_CR)
        {
            for(l_cr_num = 0; l_cr_num < 8; l_cr_num++)
            {
                //1.create mfocrf opcode, ram into thread
                l_opcode = OPCODE_MFOCRF_TO_GPR0 | ((0x00080000 >> l_cr_num) & 0x000FF000);
                FAPI_TRY(ram_opcode(l_opcode, true));

                //2.get CR value from SCR0
                l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
                FAPI_TRY(ram_opcode(l_opcode, true));

                FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, l_crf_data));
                l_cr_data |= l_crf_data();
            }

            o_buffer[0] = l_cr_data;
        }
        else
        {
            if(i_reg_num == RAM_REG_NIA)
            {
                //1.ram MFNIA_RT opcode
                opcodes[0] = {NULL, OPCODE_MFNIA_RT, NULL};
                //2.get NIA from GPR0
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
            }
            else if(i_reg_num == RAM_REG_MSR)
            {
                //1.create mfmsr opcode, ram into thread
                opcodes[0] = {NULL, OPCODE_MFMSR_TO_GPR0, NULL};
                //2.get MSR value from SCR0
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
            }
            else if(i_reg_num == RAM_REG_FPSCR)
            {
                //1.backup FPR0
                opcodes[0] = {NULL, OPCODE_MFFPRD_FROM_FPR0_TO_GPR0, NULL};
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_fpr0};
                //2.create mffs opcode, ram into thread
                opcodes[2] = {NULL, OPCODE_MFFS, NULL};
                //3.get FPSCR value from SCR0
                opcodes[3] = {NULL, OPCODE_MFFPRD_FROM_FPR0_TO_GPR0, NULL};
                opcodes[4] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
                //4.restore FPR0
                opcodes[5] = {&l_backup_fpr0, OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
                opcodes[6] = {NULL, OPCODE_MTFPRD_FROM_GPR0_TO_FPR0, NULL};
            }
            else if(i_reg_num == RAM_REG_VSCR)
            {
                //1.backup VR0
                opcodes[0] = {NULL, OPCODE_MFVSRD_FROM_VSR32_TO_GPR0, NULL};
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_vr0_dw0};
                opcodes[2] = {NULL, OPCODE_MFVSRLD_FROM_VSR32_TO_GPR0, NULL};
                opcodes[3] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_vr0_dw1};
                //2.create mfvscr opcode, ram into thread
                opcodes[4] = {NULL, OPCODE_MFVSCR_TO_VR0, NULL};
                //3.get VSCR value from SCR0
                opcodes[5] = {NULL, OPCODE_MFVSRLD_FROM_VSR32_TO_GPR0, NULL};
                opcodes[6] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
                //4.restore VR0
                opcodes[7] = {&l_backup_vr0_dw1, OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
                opcodes[8] = {&l_backup_vr0_dw0, OPCODE_MFSPR_FROM_SPRD_TO_GPR0 + (1 << 21), NULL};
                opcodes[9] = {NULL, OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR32, NULL};
            }
            else if(i_reg_num == RAM_REG_SLBE)
            {
                opcodes[0] = {NULL, OPCODE_SLBMFEE, NULL};
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
            }
            else if(i_reg_num == RAM_REG_SLBV)
            {
                opcodes[0] = {NULL, OPCODE_SLBMFEV, NULL};
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
            }
            else
            {
                //1.create mfspr<gpr0, i_reg_num> opcode, ram into thread
                l_spr_regnum_lo = i_reg_num & 0x0000001F;
                l_spr_regnum_hi = i_reg_num & 0x000003E0;
                opcodes[0] = {NULL, OPCODE_MFSPR_FROM_SPR0_TO_GPR0 + (l_spr_regnum_lo << 16) + (l_spr_regnum_hi << 6), NULL};
                //2.create mtsprd<gpr0> opcode, ram into thread
                //3.get GPR value from SCR0
                opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};
            }

            array_op = true;
        }
    }
    else if(i_type == REG_FPR)
    {
        //1.create mffprd<gpr0, i_reg_num>#SX=0 opcode, ram into thread
        opcodes[0] = {NULL, OPCODE_MFFPRD_FROM_FPR0_TO_GPR0 + (i_reg_num << 21), NULL};
        //2.create mtsprd<gpr0> opcode, ram into thread
        //3.get GPR value from SCR0
        opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &o_buffer[0]};

        array_op = true;
    }

#ifndef __PPE__
    else if(i_type == REG_VSR)
    {
        //1.create mfvsrd<gpr0, i_reg_num> opcode, ram into thread to get dw0
        if(i_reg_num < 32)
        {
            l_opcode = OPCODE_MFVSRD_FROM_VSR0_TO_GPR0;
            l_opcode += (i_reg_num << 21);
        }
        else
        {
            l_opcode = OPCODE_MFVSRD_FROM_VSR32_TO_GPR0;
            l_opcode += ((i_reg_num - 32) << 21);
        }

        FAPI_TRY(ram_opcode(l_opcode, true));

        //2.create mtsprd<gpr0> opcode, ram into thread
        l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
        FAPI_TRY(ram_opcode(l_opcode, true));

        //3.get VSR dw0 value from SCR0
        FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, o_buffer[0]));

        //4.create mfvrld<gpr0, i_reg_num> opcode, ram into thread to get dw1
        if(i_reg_num < 32)
        {
            l_opcode = OPCODE_MFVSRLD_FROM_VSR0_TO_GPR0;
            l_opcode += (i_reg_num << 21);
        }
        else
        {
            l_opcode = OPCODE_MFVSRLD_FROM_VSR32_TO_GPR0;
            l_opcode += ((i_reg_num - 32) << 21);
        }

        FAPI_TRY(ram_opcode(l_opcode, true));

        //5.create mtsprd<gpr0> opcode, ram into thread
        l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
        FAPI_TRY(ram_opcode(l_opcode, true));

        //6.get VSR dw1 value from SCR0
        FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, o_buffer[1]));
    }

#endif
    else
    {
        FAPI_ASSERT(false,
                    fapi2::P9_RAM_INVALID_REG_TYPE_ACCESS_ERR()
                    .set_REGTYPE(i_type),
                    "Type of reg (0x%.8X) is not supported", i_type);
    }

    if(array_op)
    {
        // Looping through the opcode structures
        for(size_t i = 0; i < MAX_OPCODES; i++)
        {
            if(opcodes[i].prePutScom)
            {
                FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, *opcodes[i].prePutScom));
            }

            if(opcodes[i].opCode)
            {
                FAPI_TRY(ram_opcode(opcodes[i].opCode, true));
            }
            else
            {
                // Will break after encountering first empty opcode,
                // as the array will be empty after this
                break;
            }

            if(opcodes[i].postGetScom)
            {
                fapi2::buffer<uint64_t> scomData;
                FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, scomData));
                *opcodes[i].postGetScom = scomData;
            }
        }
    }

    //restore GPR0 if necessary
    if(iv_write_gpr0)
    {
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, l_backup_gpr0));
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR0;
        FAPI_TRY(ram_opcode(l_opcode, true));
    }

    // ram_cleanup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_cleanup());
    }

fapi_try_exit:
    // Error happened and it's not ram error, call ram_cleanup to restore the backup registers
    // If it is ram error, do not call ram_cleanup, so that no new ramming will be executed
    // Do not use "FAPI_TRY" to avoid endless loop
    fapi2::ReturnCode first_err = fapi2::current_err;

    if((fapi2::current_err != fapi2::FAPI2_RC_SUCCESS) && !iv_ram_err && iv_ram_setup)
    {
        ram_cleanup();
    }

    FAPI_DBG("Exiting get register");
    return first_err;
}

//-----------------------------------------------------------------------------------
fapi2::ReturnCode RamCore::put_reg(const Enum_RegType i_type,
                                   const uint32_t i_reg_num,
                                   const fapi2::buffer<uint64_t>* i_buffer,
                                   const bool i_allow_mult)
{
    FAPI_DBG("Start put register");
    uint32_t l_opcode = 0;
    uint32_t l_spr_regnum_lo = 0;
    uint32_t l_spr_regnum_hi = 0;
    bool l_write_gpr0 = false;
    fapi2::buffer<uint64_t> l_backup_lr   = 0;
    fapi2::buffer<uint64_t> l_backup_gpr0 = 0;
    fapi2::buffer<uint64_t> l_backup_gpr1 = 0;
    fapi2::buffer<uint64_t> l_backup_fpr0 = 0;
    fapi2::buffer<uint64_t> l_backup_vr0_dw0 = 0;
    fapi2::buffer<uint64_t> l_backup_vr0_dw1 = 0;
    static const size_t MAX_OPCODES = 10;
    putreg_op_t opcodes[MAX_OPCODES] = {};
    bool array_op = false;

    // ram_setup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_setup());
    }

    FAPI_ASSERT(iv_ram_setup,
                fapi2::P9_RAM_NOT_SETUP_ERR()
                .set_CORE_TARGET(iv_target),
                "Attempting to cleanup ram without setup before");

    //backup GPR0 if it is written
    if(iv_write_gpr0)
    {
        l_opcode = OPCODE_MTSPR_FROM_GPR0_TO_SPRD;
        FAPI_TRY(ram_opcode(l_opcode, true));
        FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, l_backup_gpr0));
    }

#ifndef __PPE__

    //backup GPR1 if it is written
    if(iv_write_gpr1 && ((i_type == REG_VSR) || ((i_type == REG_SPR) && (i_reg_num == RAM_REG_VSCR))))
    {
        l_opcode = OPCODE_MTSPR_FROM_GPR1_TO_SPRD;
        FAPI_TRY(ram_opcode(l_opcode, true));
        FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, l_backup_gpr1));
    }

#endif

    // put register value
    if(i_type == REG_GPR)
    {
        //1.put GPR value into SCR0
        //2.create mfsprd<i_reg_num> opcode, ram into thread
        opcodes[0] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0 + (i_reg_num << 21), NULL};

        if(i_reg_num == 0)
        {
            iv_write_gpr0 = true;
            l_write_gpr0  = true;
        }

        if(i_reg_num == 1)
        {
            iv_write_gpr1 = true;
        }

        array_op = true;
    }
    else if(i_type == REG_SPR)
    {
        if(i_reg_num == RAM_REG_NIA)
        {
            //1.backup LR
            opcodes[0] = {NULL, OPCODE_MFSPR_FROM_LR_TO_GPR0, NULL};
            opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_lr};
            //2.put NIA value into LR
            opcodes[2] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[3] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_LR, NULL};
            //3.ram MTNIA_LR opcode
            opcodes[4] = {NULL, OPCODE_MTNIA_LR, NULL};
            //4.restore LR
            opcodes[5] = {&l_backup_lr, OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[6] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_LR, NULL};
        }
        else if(i_reg_num == RAM_REG_MSR)
        {
            //1.put SPR value into SCR0
            //2.create mfsprd<gpr0> opcode, ram into thread
            opcodes[0] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            //3.create mtmsrd opcode, ram into thread
            opcodes[1] = {NULL, OPCODE_MTMSRD_L0, NULL};
        }
        else if(i_reg_num == RAM_REG_CR)
        {
            //1.put SPR value into SCR0
            //2.create mfsprd<gpr0> opcode, ram into thread
            opcodes[0] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            //3.create mtcrf opcode, ram into thread
            opcodes[1] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 0) & 0x000FF000), NULL};
            opcodes[2] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 1) & 0x000FF000), NULL};
            opcodes[3] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 2) & 0x000FF000), NULL};
            opcodes[4] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 3) & 0x000FF000), NULL};
            opcodes[5] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 4) & 0x000FF000), NULL};
            opcodes[6] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 5) & 0x000FF000), NULL};
            opcodes[7] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 6) & 0x000FF000), NULL};
            opcodes[8] = {NULL, OPCODE_MTOCRF_FROM_GPR0 | ((0x00080000 >> 7) & 0x000FF000), NULL};
        }
        else if(i_reg_num == RAM_REG_FPSCR)
        {
            //1.backup FPR0
            opcodes[0] = {NULL, OPCODE_MFFPRD_FROM_FPR0_TO_GPR0, NULL};
            opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_fpr0};
            //2.put SPR value into FPR0
            opcodes[2] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[3] = {NULL, OPCODE_MTFPRD_FROM_GPR0_TO_FPR0, NULL};
            //3.create mtfsf opcode, ram into thread
            opcodes[4] = {NULL, OPCODE_MTFSF_FROM_FPR0, NULL};
            //4.restore FPR0
            opcodes[5] = {&l_backup_fpr0, OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[6] = {NULL, OPCODE_MTFPRD_FROM_GPR0_TO_FPR0, NULL};
        }
        else if(i_reg_num == RAM_REG_VSCR)
        {
            //1.backup VR0
            opcodes[0] = {NULL, OPCODE_MFVSRD_FROM_VSR32_TO_GPR0, NULL};
            opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_vr0_dw0};
            opcodes[2] = {NULL, OPCODE_MFVSRLD_FROM_VSR32_TO_GPR0, NULL};
            opcodes[3] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPRD, &l_backup_vr0_dw1};
            //2.put SPR value into VR0
            opcodes[4] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[5] = {NULL, OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR32, NULL};
            //3.create mtvscr opcode, ram into thread
            opcodes[6] = {NULL, OPCODE_MTVSCR_FROM_VR0, NULL};
            opcodes[7] = {&l_backup_vr0_dw1, OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
            opcodes[8] = {&l_backup_vr0_dw0, OPCODE_MFSPR_FROM_SPRD_TO_GPR0 + (1 << 21), NULL};
            opcodes[9] = {NULL, OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR32, NULL};
        }
        else
        {
            //1.put SPR value into SCR0
            //2.create mfsprd<gpr0> opcode, ram into thread
            opcodes[0] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};

            //3.create mtspr<i_reg_num, gpr0> opcode, ram into thread
            l_spr_regnum_lo = i_reg_num & 0x0000001F;
            l_spr_regnum_hi = i_reg_num & 0x000003E0;
            opcodes[1] = {NULL, OPCODE_MTSPR_FROM_GPR0_TO_SPR0 + (l_spr_regnum_lo << 16) + (l_spr_regnum_hi << 6), NULL};
        }

        array_op = true;
    }
    else if(i_type == REG_FPR)
    {
        //1.put FPR value into SCR0
        //2.create mfsprd<gpr0> opcode, ram into thread
        opcodes[0] = {const_cast<fapi2::buffer<uint64_t>* >(&i_buffer[0]), OPCODE_MFSPR_FROM_SPRD_TO_GPR0, NULL};
        //3.create mtfprd<i_reg_num, gpr0>#TX=0 opcode, ram into thread
        opcodes[1] = {NULL, OPCODE_MTFPRD_FROM_GPR0_TO_FPR0 + (i_reg_num << 21), &l_backup_vr0_dw0};

        array_op = true;
    }

#ifndef __PPE__
    else if(i_type == REG_VSR)
    {
        //1.put VSR dw1 value into SCR0
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, i_buffer[1]));

        //2.create mfsprd<gpr0> opcode, ram into thread
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR0;
        FAPI_TRY(ram_opcode(l_opcode, true));

        //3.put VSR dw0 value into SCR0
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, i_buffer[0]));

        //4.create mfsprd<gpr1> opcode, ram into thread
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR0;
        l_opcode += (1 << 21);
        FAPI_TRY(ram_opcode(l_opcode, true));

        //5.create mtvsrdd<i_reg_num, gpr0, gpr1> opcode, ram into thread
        if(i_reg_num < 32)
        {
            l_opcode = OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR0;
            l_opcode += (i_reg_num << 21);
        }
        else
        {
            l_opcode = OPCODE_MTVSRDD_FROM_GPR1_0_TO_VSR32;
            l_opcode += ((i_reg_num - 32) << 21);
        }

        FAPI_TRY(ram_opcode(l_opcode, true));
    }

#endif
    else
    {
        FAPI_ASSERT(false,
                    fapi2::P9_RAM_INVALID_REG_TYPE_ACCESS_ERR()
                    .set_REGTYPE(i_type),
                    "Type of reg (0x%.8X) is not supported", i_type);
    }

    if(array_op)
    {
        // Looping through the opcode structures
        for(size_t i = 0; i < MAX_OPCODES; i++)
        {
            if(opcodes[i].prePutScom)
            {
                FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, *opcodes[i].prePutScom));
            }

            if(opcodes[i].opCode)
            {
                FAPI_TRY(ram_opcode(opcodes[i].opCode, true));
            }
            else
            {
                // Will break after encountering first empty opcode,
                // as the array will be empty after this
                break;
            }

            if(opcodes[i].postGetScom)
            {
                fapi2::buffer<uint64_t> scomData;
                FAPI_TRY(fapi2::getScom(iv_target, C_SCR0, scomData));
                *opcodes[i].postGetScom = scomData;
            }
        }
    }


    //restore GPR0 if necessary
    if(iv_write_gpr0 && !l_write_gpr0)
    {
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, l_backup_gpr0));
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR0;
        FAPI_TRY(ram_opcode(l_opcode, true));
    }

#ifndef __PPE__

    //restore GPR1 if necessary
    if(iv_write_gpr1 && ((i_type == REG_VSR) || ((i_type == REG_SPR) && (i_reg_num == RAM_REG_VSCR))))
    {
        FAPI_TRY(fapi2::putScom(iv_target, C_SCR0, l_backup_gpr1));
        l_opcode = OPCODE_MFSPR_FROM_SPRD_TO_GPR1;
        FAPI_TRY(ram_opcode(l_opcode, true));
    }

#endif

    // ram_cleanup
    if(!i_allow_mult)
    {
        FAPI_TRY(ram_cleanup());
    }

fapi_try_exit:
    // Error happened and it's not ram error, call ram_cleanup to restore the backup registers
    // If it is ram error, do not call ram_cleanup, so that no new ramming will be executed
    // Do not use "FAPI_TRY" to avoid endless loop
    fapi2::ReturnCode first_err = fapi2::current_err;

    if((fapi2::current_err != fapi2::FAPI2_RC_SUCCESS) && !iv_ram_err && iv_ram_setup)
    {
        ram_cleanup();
    }

    FAPI_DBG("Exiting put register");
    return first_err;
}
