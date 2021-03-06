/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/htmgt/htmgt_reasoncodes.H $                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2014,2019                        */
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
#ifndef __HTMGT_REASONCODES_H
#define __HTMGT_REASONCODES_H

#include <hbotcompid.H>

namespace HTMGT
{
    enum htmgtModuleId
    {
        HTMGT_MOD_LOAD_START_STATUS     = 0x00,
        HTMGT_MOD_WAIT_FOR_OCC_ACTIVE   = 0x01,
        HTMGT_MOD_WAIT_FOR_OCC_READY    = 0x02,
        HTMGT_MOD_MEMTHROTTLE           = 0x03,
        HTMGT_MOD_OCCMGR_SET_STATE      = 0x04,
        HTMGT_MOD_OCC_SET_STATE         = 0x05,
        HTMGT_MOD_PROCESS_OCC_RESET     = 0x06,
        HTMGT_MOD_OCC_RESET             = 0x07,
        HTMGT_MOD_BUILD_OCCS            = 0x08,
        HTMGT_MOD_PASS_THRU             = 0x09,
        HTMGT_MOD_CFG_FREQ_POINTS       = 0x0A,
        HTMGT_MOD_WAIT_FOR_CHECKPOINT   = 0x0B,
        HTMGT_MODID_PROCESS_ELOG        = 0x10,
        HTMGT_MOD_OCC_POLL              = 0x76,
        HTMGT_MOD_READ_OCC_SRAM         = 0x85,
        HTMGT_MOD_SEND_OCC_CMD          = 0x90,
        HTMGT_MOD_WRITE_OCC_CMD         = 0x91,
        HTMGT_MOD_CHECK_OCC_RSP         = 0x92,
        HTMGT_MOD_PARSE_OCC_RSP         = 0x94,
        HTMGT_MOD_HANLDE_OCC_EXCEPTION  = 0xE0,
        HTMGT_MOD_ENABLE_OCC_ACTUATION  = 0xE1,
    };

    enum htmgtReasonCode
    {
        HTMGT_RC_NO_ERROR              = 0x00,
        HTMGT_RC_INTERNAL_ERROR        = HTMGT_COMP_ID | 0x01,
        HTMGT_RC_THROTTLE_UTIL_ERROR   = HTMGT_COMP_ID | 0x02,
        HTMGT_RC_THROTTLE_INVALID_N    = HTMGT_COMP_ID | 0x03,
        HTMGT_RC_OT_THROTTLE_INVALID_N = HTMGT_COMP_ID | 0x04,
        HTMGT_RC_OCC_NOT_READY         = HTMGT_COMP_ID | 0x05,
        HTMGT_RC_ATTRIBUTE_ERROR       = HTMGT_COMP_ID | 0x06,
        HTMGT_RC_OCC_EXCEPTION         = HTMGT_COMP_ID | 0x0E,
        HTMGT_RC_NO_SUPPORT            = HTMGT_COMP_ID | 0x0F,
        HTMGT_RC_OCC_RESET             = HTMGT_COMP_ID | 0x15,
        HTMGT_RC_OCC_CRIT_FAILURE      = HTMGT_COMP_ID | 0x16,
        HTMGT_RC_SEND_FAIL             = HTMGT_COMP_ID | 0x18,
        HTMGT_RC_CHECKSUM_FAIL         = HTMGT_COMP_ID | 0x19,
        HTMGT_RC_OCC_CMD_FAIL          = HTMGT_COMP_ID | 0x1B,
        HTMGT_RC_INVALID_LENGTH        = HTMGT_COMP_ID | 0x1C,
        HTMGT_RC_OCC_ERROR_LOG         = HTMGT_COMP_ID | 0x1D,
        HTMGT_RC_INVALID_ROLE          = HTMGT_COMP_ID | 0x22,
        HTMGT_RC_INVALID_DATA          = HTMGT_COMP_ID | 0x23,
        HTMGT_RC_OCC_UNAVAILABLE       = HTMGT_COMP_ID | 0x24,
        HTMGT_RC_OCC_START_FAIL        = HTMGT_COMP_ID | 0x25,
        HTMGT_RC_OCC_UNEXPECTED_STATE  = HTMGT_COMP_ID | 0x26,
        HTMGT_RC_PRESENT_STATE_PROHIBITS = HTMGT_COMP_ID | 0x28,
        HTMGT_RC_PARAMETER_OUT_OF_RANGE  = HTMGT_COMP_ID | 0x29,
        HTMGT_RC_OVERSUBSCRIBED        = HTMGT_COMP_ID | 0x2D,
        HTMGT_RC_TIMEOUT               = HTMGT_COMP_ID | 0x35,
        HTMGT_RC_COMMAND_IN_PROGRESS   = HTMGT_COMP_ID | 0x36,
        HTMGT_RC_INVALID_PARAMETER     = HTMGT_COMP_ID | 0x45,
        HTMGT_RC_MISSING_DATA          = HTMGT_COMP_ID | 0x4F,
        HTMGT_RC_ECMD_DBUF_COPY_FAIL   = HTMGT_COMP_ID | 0x53,
        HTMGT_RC_TARGET_NOT_FUNCTIONAL = HTMGT_COMP_ID | 0x54,
        HTMGT_RC_OCC_MASTER_NOT_FOUND  = HTMGT_COMP_ID | 0x55,
        HTMGT_RC_INVALID_OCC_ELOG      = HTMGT_COMP_ID | 0x63,
        HTMGT_RC_BAD_FRU_CALLOUTS      = HTMGT_COMP_ID | 0x7D,
        HTMGT_RC_MISMATCHING_SEVERITY  = HTMGT_COMP_ID | 0x7F,
        HTMGT_RC_PCAP_CALC_COMPLETE    = HTMGT_COMP_ID | 0xB9,
        HTMGT_RC_ENERGYSCALE_FFDC      = HTMGT_COMP_ID | 0xFD,
    };
};

#endif
