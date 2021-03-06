/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/lib/p9_hcd_memmap_occ_sram.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2019                        */
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
///
/// @file   p9_hcd_memmap_occ_sram.H
/// @brief  defines region constants of occ sram.
///

// *HWP HWP Owner:        David Du   <daviddu@us.ibm.com>
// *HWP Backup HWP Owner: Greg Still <stillgs@us.ibm.com>
// *HWP FW Owner:         Prem S Jha <premjha2@in.ibm.com>
// *HWP Team:             PM
// *HWP Level:            2
// *HWP Consumed by:      PM:Hostboot: Phyp

#ifndef __P9_HCD_MEMMAP_OCC_SRAM_H__
#define __P9_HCD_MEMMAP_OCC_SRAM_H__

#include <p9_hcd_header_defs.H>
#include <p9_hcd_memmap_base.H>

// -------------------------------------------------------------------
// Note: There can be NO semicolons(";") at end of macros in this file
//       There can ONLY have HCD_CONST/HCD_CONST64 macros in this file
// -------------------------------------------------------------------

/// OCC SRAM

HCD_CONST(OCC_SRAM_BASE_ADDR,                   0xFFF00000)
HCD_CONST(GPE0_SRAM_BASE_ADDR,                  0xFFF01000)
HCD_CONST(GPE1_SRAM_BASE_ADDR,                  0xFFF10000)

/// Base Addresses for various debug/trace regions in OCC SRAM
HCD_CONST(OCC_SRAM_TRACE_BUF_BASE_ERR,          0xFFFB4000)
HCD_CONST(OCC_SRAM_TRACE_BUF_BASE_INF,          0xFFFB6000)
HCD_CONST(OCC_SRAM_TRACE_BUF_BASE_IMP,          0xFFFB8000)
HCD_CONST(OCC_SRAM_TRACE_BUF_BASE_SSX_PTR,      0xFFF40824)

// Offset to trace buf ptr and trace buffer size from base
HCD_CONST(GPE_DEBUG_PTR_OFFSET,                      0x180)

// Size of various traces regions in OCC SRAM
HCD_CONST(OCC_SRAM_TRACE_BUF_SSX_SIZE_PTR,      0xFFF40828)
HCD_CONST(OCC_SRAM_TRACE_BUF_ERR_SIZE,          (8 * ONE_KB))
HCD_CONST(OCC_SRAM_TRACE_BUF_INF_SIZE,          (8 * ONE_KB))
HCD_CONST(OCC_SRAM_TRACE_BUF_IMP_SIZE,          (8 * ONE_KB))

HCD_CONST(OCC_SRAM_IPC_REGION_SIZE,             (4 * ONE_KB))
HCD_CONST(OCC_SRAM_GPE0_REGION_SIZE,            (60 * ONE_KB))
HCD_CONST(OCC_SRAM_GPE1_REGION_SIZE,            (64 * ONE_KB))
HCD_CONST(OCC_SRAM_PGPE_REGION_SIZE,            (PGPE_IMAGE_SIZE + PGPE_AUX_TASK_SIZE + PGPE_OCC_SHARED_SRAM_SIZE ))
HCD_CONST(OCC_SRAM_SGPE_REGION_SIZE,            SGPE_IMAGE_SIZE)
HCD_CONST(OCC_SRAM_OCC_REGION_SIZE,             (512 * ONE_KB))

HCD_CONST(OCC_SRAM_BEFORE_PGPE_REGION_SIZE_TOTAL,
          (OCC_SRAM_IPC_REGION_SIZE + OCC_SRAM_GPE0_REGION_SIZE + OCC_SRAM_GPE1_REGION_SIZE))


//--------------------------------------------------------------------------------------

/// PGPE Base

HCD_CONST(OCC_SRAM_PGPE_BASE_ADDR,
          (OCC_SRAM_BASE_ADDR + OCC_SRAM_BEFORE_PGPE_REGION_SIZE_TOTAL))
HCD_CONST(OCC_SRAM_PGPE_END_ADDR,
          (OCC_SRAM_PGPE_BASE_ADDR + OCC_SRAM_PGPE_REGION_SIZE))
HCD_CONST(OCC_SRAM_PGPE_HCODE_RESET_ADDR,
          (OCC_SRAM_PGPE_BASE_ADDR + PGPE_HCODE_RESET_ADDR_VAL))
HCD_CONST(OCC_SRAM_PGPE_HEADER_ADDR,
          (OCC_SRAM_PGPE_BASE_ADDR + PGPE_INT_VECTOR_SIZE))


/// PGPE Boot

HCD_CONST(OCC_SRAM_PGPE_COPY_BOOT_LOADER_SIZE,  ONE_KB)
HCD_CONST(OCC_SRAM_PGPE_COPY_PPMR_HEADER_SIZE,  ONE_KB)
HCD_CONST(OCC_SRAM_PGPE_BOOT_LOADER_ADDR,
          (OCC_SRAM_PGPE_END_ADDR - OCC_SRAM_PGPE_COPY_BOOT_LOADER_SIZE))
HCD_CONST(OCC_SRAM_PGPE_BOOT_LOADER_RESET_ADDR,
          (OCC_SRAM_PGPE_BOOT_LOADER_ADDR + PGPE_BOOT_LOADER_RESET_ADDR_VAL))
HCD_CONST(OCC_SRAM_PGPE_PPMR_HEADER_ADDR,
          (OCC_SRAM_PGPE_BOOT_LOADER_ADDR - OCC_SRAM_PGPE_COPY_PPMR_HEADER_SIZE))
HCD_CONST(OCC_SRAM_AUX_TASK_ADDR,
          (OCC_SRAM_PGPE_PPMR_HEADER_ADDR - PGPE_AUX_TASK_SIZE))
HCD_CONST(OCC_SRAM_PGPE_OPTRACE_ADDR, OCC_SRAM_PGPE_BOOT_LOADER_ADDR)
HCD_CONST(OCC_SRAM_PGPE_OPTRACE_SIZE, OCC_SRAM_PGPE_COPY_BOOT_LOADER_SIZE)

/// PGPE Copy

HCD_CONST(OCC_SRAM_PGPE_HCODE_OFFSET_ADDR,
          (OCC_SRAM_PGPE_PPMR_HEADER_ADDR + PPMR_PGPE_HCODE_OFFSET_BYTE))
HCD_CONST(OCC_SRAM_PGPE_HCODE_LENGTH_ADDR,
          (OCC_SRAM_PGPE_PPMR_HEADER_ADDR + PPMR_PGPE_HCODE_LENGTH_BYTE))
HCD_CONST(OCC_SRAM_PGPE_IMAGE_LENGTH_ADDR,
          (OCC_SRAM_PGPE_PPMR_HEADER_ADDR + PPMR_PGPE_SRAM_IMAGE_SIZE_BYTE))

// Misc constants used in PGPE boot loader and boot copier.
HCD_CONST(PGPE_BOOT_COPY_SUCCESS,   0x42432d53 ) // ASCII code for BC-S
HCD_CONST(PGPE_BOOT_COPIER_FAIL,    0x42432d46 ) // ASCII code for BC-F
HCD_CONST(PGPE_BOOT_LOADER_SUCCESS, 0x424c2d53 ) // ASCII code for BL-S
HCD_CONST(PGPE_BOOT_LOADER_FAIL,    0x424c2d46 ) // ASCII code for BL-F

//--------------------------------------------------------------------------------------

// Misc constants used in SGPE boot loader and boot copier.
HCD_CONST(DIVDE_BY_8,  3)
HCD_CONST(DOUBLE_WORD_SIZE,  8)
HCD_CONST(SGPE_BOOT_PROG_CODE_POS,  QPMR_SGPE_BOOT_PROG_CODE)
HCD_CONST(SGPE_SRAM_IMG_SIZE_POS,   QPMR_SGPE_IMAGE_SIZE)
HCD_CONST(SGPE_IMG_OFFSET_POS,      40)
HCD_CONST(BOOT_COPIER_LEN_ZERO,     0)
HCD_CONST(ENABLE_TRAP,              0)
HCD_CONST(SGPE_BOOT_COPY_SUCCESS,   0x42432d53 ) // ASCII code for BC-S
HCD_CONST(SGPE_BOOT_COPIER_FAIL,    0x42432d46 ) // ASCII code for BC-F
HCD_CONST(SGPE_BOOT_LOADER_SUCCESS, 0x424c2d53 ) // ASCII code for BL-S
HCD_CONST(SGPE_BOOT_LOADER_FAIL,    0x424c2d46 ) // ASCII code for BL-F

/// SGPE Base
HCD_CONST(OCC_SRAM_SGPE_BASE_ADDR,
          (OCC_SRAM_BASE_ADDR + OCC_SRAM_BEFORE_PGPE_REGION_SIZE_TOTAL + OCC_SRAM_PGPE_REGION_SIZE))
HCD_CONST(OCC_SRAM_SGPE_END_ADDR,
          (OCC_SRAM_SGPE_BASE_ADDR + OCC_SRAM_SGPE_REGION_SIZE))
HCD_CONST(OCC_SRAM_SGPE_HCODE_RESET_ADDR,
          (OCC_SRAM_SGPE_BASE_ADDR + SGPE_HCODE_RESET_ADDR_VAL))
HCD_CONST(OCC_SRAM_SGPE_HEADER_ADDR,
          (OCC_SRAM_SGPE_BASE_ADDR + SGPE_INT_VECTOR_SIZE))

/// SGPE Persistent Data Area(PDA)

HCD_CONST(OCC_SRAM_SGPE_PDA_SIZE,               ONE_KB)
HCD_CONST(OCC_SRAM_SGPE_PDA_ADDR,
          (OCC_SRAM_SGPE_END_ADDR - OCC_SRAM_SGPE_PDA_SIZE))

/// SGPE Boot

HCD_CONST(OCC_SRAM_SGPE_COPY_BOOT_LOADER_SIZE,  ONE_KB)
HCD_CONST(OCC_SRAM_SGPE_COPY_QPMR_HEADER_SIZE,  ONE_KB)
HCD_CONST(OCC_SRAM_SGPE_BOOT_LOADER_ADDR,
          (OCC_SRAM_SGPE_PDA_ADDR - OCC_SRAM_SGPE_COPY_BOOT_LOADER_SIZE))
HCD_CONST(OCC_SRAM_SGPE_BOOT_LOADER_RESET_ADDR,
          (OCC_SRAM_SGPE_BOOT_LOADER_ADDR + SGPE_BOOT_LOADER_RESET_ADDR_VAL))
HCD_CONST(OCC_SRAM_SGPE_QPMR_HEADER_ADDR,
          (OCC_SRAM_SGPE_BOOT_LOADER_ADDR - OCC_SRAM_SGPE_COPY_QPMR_HEADER_SIZE))


/// SGPE Copy

HCD_CONST(OCC_SRAM_SGPE_HCODE_OFFSET_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_SGPE_HCODE_OFFSET_BYTE))
HCD_CONST(OCC_SRAM_SGPE_HCODE_LENGTH_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_SGPE_HCODE_LENGTH_BYTE))
HCD_CONST(OCC_SRAM_QUAD_COMMON_RINGS_OFFSET_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_QUAD_COMMON_RINGS_OFFSET_BYTE))
HCD_CONST(OCC_SRAM_QUAD_COMMON_RINGS_LENGTH_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_QUAD_COMMON_RINGS_LENGTH_BYTE))
HCD_CONST(OCC_SRAM_QUAD_SPECIFIC_RINGS_OFFSET_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_QUAD_SPECIFIC_RINGS_OFFSET_BYTE))
HCD_CONST(OCC_SRAM_QUAD_SPECIFIC_RINGS_LENGTH_ADDR,
          (OCC_SRAM_SGPE_QPMR_HEADER_ADDR + QPMR_QUAD_SPECIFIC_RINGS_LENGTH_BYTE))


HCD_CONST(OCC_SRAM_SGPE_DASHBOARD_START,
          ( OCC_SRAM_SGPE_HEADER_ADDR + SGPE_HEADER_SIZE + SGPE_DEBUG_PTRS_SIZE - 4 ));   // For 8B alignment
HCD_CONST( OCC_SRAM_SGPE_DASHBOARD_SIZE, 0x134 );
HCD_CONST( OCC_SRAM_SGPE_TRACE_START,
           (OCC_SRAM_SGPE_HEADER_ADDR + SGPE_HEADER_SIZE));
// PGPE
HCD_CONST(OCC_SRAM_PGPE_DASHBOARD_START,
          ( OCC_SRAM_PGPE_HEADER_ADDR + PGPE_HEADER_SIZE + PGPE_DEBUG_PTRS_SIZE - 4 ));   // For 8B alignment
HCD_CONST( OCC_SRAM_PGPE_DASHBOARD_SIZE, 0xfc );
HCD_CONST( OCC_SRAM_PGPE_TRACE_START,
           (OCC_SRAM_PGPE_HEADER_ADDR + PGPE_HEADER_SIZE));


HCD_CONST(OCC_SRAM_SHARED_DATA_BASE_ADDR,
          (OCC_SRAM_PGPE_BASE_ADDR + OCC_SRAM_PGPE_REGION_SIZE - PGPE_OCC_SHARED_SRAM_SIZE))

#endif /* __P9_HCD_MEMMAP_OCC_SRAM_H__ */
