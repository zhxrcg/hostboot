/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/common/plat/explorer/prdfExplorerPlugins_common.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2019                             */
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

// Framework includes
#include <iipServiceDataCollector.h>
#include <prdfExtensibleChip.H>
#include <prdfPluginMap.H>

// Platform includes
#include <prdfMemDbUtils.H>
#include <prdfMemEccAnalysis.H>
#include <prdfMemUtils.H>
#include <prdfPlatServices.H>

using namespace TARGETING;

namespace PRDF
{

using namespace PlatServices;

namespace explorer_ocmb
{

//##############################################################################
//
//                             Special plugins
//
//##############################################################################

/**
 * @brief  Plugin that initializes the data bundle.
 * @param  i_chip An OCMB chip.
 * @return SUCCESS
 */
int32_t Initialize( ExtensibleChip * i_chip )
{
    i_chip->getDataBundle() = new OcmbDataBundle( i_chip );
    return SUCCESS;
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, Initialize );

/**
 * @brief  Plugin function called after analysis is complete but before PRD
 *         exits.
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @note   This is especially useful for any analysis that still needs to be
 *         done after the framework clears the FIR bits that were at attention.
 * @return SUCCESS.
 */
int32_t PostAnalysis( ExtensibleChip * i_chip, STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::PostAnalysis] "

    #ifdef __HOSTBOOT_RUNTIME

    // If the IUE threshold in our data bundle has been reached, we trigger
    // a port fail. Once we trigger the port fail, the system may crash
    // right away. Since PRD is running in the hypervisor, it is possible we
    // may not get the error log. To better our chances, we trigger the port
    // fail here after the error log has been committed.
    if ( MemEcc::queryIueTh<TYPE_OCMB_CHIP>(i_chip, io_sc) )
    {
        if ( SUCCESS != MemEcc::triggerPortFail<TYPE_OCMB_CHIP>(i_chip) )
        {
            PRDF_ERR( PRDF_FUNC "triggerPortFail(0x%08x) failed",
            i_chip->getHuid() );
        }
    }

    #endif // __HOSTBOOT_RUNTIME

    // Cleanup processor FIR bits on the other side of the channel.
    MemUtils::cleanupChnlAttns<TYPE_OCMB_CHIP>( i_chip, io_sc );

    return SUCCESS;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, PostAnalysis );


//##############################################################################
//
//                               OCMB_LFIR
//
//##############################################################################

/**
 * @brief  OCMB_LFIR[38] - DDR4 PHY interrupt
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t Ddr4PhyInterrupt( ExtensibleChip * i_chip,
                          STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::Ddr4PhyInterrupt] "

    SCAN_COMM_REGISTER_CLASS * rdffir = i_chip->getRegister( "RDFFIR" );

    // If Mainline UE (RDFFIR[14]) or Maint UE (RDFFIR[34]) are on at the same
    // time as this:
    if ( rdffir->IsBitSet(14) || rdffir->IsBitSet(34) )
    {
        // callout Explorer on 1st
        io_sc.service_data->SetThresholdMaskId(0);

        // mask maint and mainline UE which are assumed to be side-effects
        SCAN_COMM_REGISTER_CLASS * rdffir_mask_or =
            i_chip->getRegister( "RDFFIR_MASK_OR" );

        rdffir_mask_or->SetBit(14);
        rdffir_mask_or->SetBit(34);

        if ( SUCCESS != rdffir_mask_or->Write() )
        {
            PRDF_ERR( PRDF_FUNC "Write() failed on RDFFIR_MASK_OR: 0x%08x",
                      i_chip->getHuid() );
        }
    }
    else
    {
        //TODO RTC 200583
        // callout Explorer on threshold (5/day)
        // NOTE: in this case we will have to clear both hw driven checkers
        // manually before clearing the FIR
    }

    return SUCCESS;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, Ddr4PhyInterrupt );

//------------------------------------------------------------------------------

/**
 * @brief  OCMB_LFIR[39:46] - Foxhound Fatal
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t FoxhoundFatal( ExtensibleChip * i_chip, STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::FoxhoundFatal] "

    //TODO RTC 200583

    return SUCCESS;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, FoxhoundFatal );

//##############################################################################
//
//                               OMIDLFIR
//
//##############################################################################

/**
 * @brief  OMIDLFIR[0] - OMI-DL0 Fatal Error
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return PRD_SCAN_COMM_REGISTER_ZERO for the bus callout, else SUCCESS
 */
int32_t DlFatalError( ExtensibleChip * i_chip, STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::DlFatalError] "

    int32_t rc = SUCCESS;

    do
    {
        // Check DL0_ERROR_HOLD[52:63] to determine callout
        SCAN_COMM_REGISTER_CLASS * dl0_error_hold =
            i_chip->getRegister( "DL0_ERROR_HOLD" );

        if ( SUCCESS != dl0_error_hold->Read() )
        {
            PRDF_ERR( PRDF_FUNC "Read() Failed on DL0_ERROR_HOLD: "
                      "i_chip=0x%08x", i_chip->getHuid() );
            break;
        }

        if ( dl0_error_hold->IsBitSet(53) ||
             dl0_error_hold->IsBitSet(55) ||
             dl0_error_hold->IsBitSet(57) ||
             dl0_error_hold->IsBitSet(58) ||
             dl0_error_hold->IsBitSet(59) ||
             dl0_error_hold->IsBitSet(60) ||
             dl0_error_hold->IsBitSet(62) ||
             dl0_error_hold->IsBitSet(63) )
        {
            // callout OCMB
            io_sc.service_data->SetCallout( i_chip->getTrgt() );
        }
        else if ( dl0_error_hold->IsBitSet(54) ||
                  dl0_error_hold->IsBitSet(56) ||
                  dl0_error_hold->IsBitSet(61) )
        {
            // callout the OMI target, the OMI bus, and the OCMB.
            // Return PRD_SCAN_COMM_REGISTER_ZERO so the rule code knows to
            // make the correct callout.
            rc = PRD_SCAN_COMM_REGISTER_ZERO;
        }

    }while(0);

    return rc;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, DlFatalError );

//##############################################################################
//
//                               RDFFIR
//
//##############################################################################

/**
 * @brief  Adds all attached DIMMs at HIGH priority.
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t CalloutAttachedDimmsHigh( ExtensibleChip * i_chip,
                                  STEP_CODE_DATA_STRUCT & io_sc )
{
    for ( auto & dimm : getConnected(i_chip->getTrgt(), TYPE_DIMM) )
        io_sc.service_data->SetCallout( dimm, MRU_HIGH );

    return SUCCESS; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, CalloutAttachedDimmsHigh );

//------------------------------------------------------------------------------

/**
 * @brief  RDF RCD Parity Error
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t RdfRcdParityError( ExtensibleChip * i_chip,
                           STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::RdfRcdParityError] "

    do
    {
        SCAN_COMM_REGISTER_CLASS * rdffir = i_chip->getRegister( "RDFFIR" );
        if ( SUCCESS != rdffir->Read() )
        {
            PRDF_ERR( PRDF_FUNC "Read() Failed on RDFFIR: "
                      "i_chip=0x%08x", i_chip->getHuid() );
            break;
        }

        // If RDFFIR[40] on at the same time, this is 'missing rddata valid'
        // case, which returns SUE
        if ( rdffir->IsBitSet(40) )
        {
            // callout MEM_PORT on 1st occurrence
            TargetHandle_t memPort =
                getConnectedChild( i_chip->getTrgt(), TYPE_MEM_PORT, 0 );
            io_sc.service_data->SetCallout( memPort );
        }
        // Else this is 'confirmed RCD parity error' case
        else
        {
            // callout DIMM high priority, MEM_PORT low on 1st occurrence
            CalloutAttachedDimmsHigh( i_chip, io_sc );
            TargetHandle_t memPort =
                getConnectedChild( i_chip->getTrgt(), TYPE_MEM_PORT, 0 );
            io_sc.service_data->SetCallout( memPort, MRU_LOW );
        }

        // Mask bit 40 as well
        SCAN_COMM_REGISTER_CLASS * rdffir_mask_or =
            i_chip->getRegister( "RDFFIR_MASK_OR" );

        rdffir_mask_or->SetBit(40);
        if ( SUCCESS != rdffir_mask_or->Write() )
        {
            PRDF_ERR( PRDF_FUNC "Write() Failed on RDFFIR_MASK_OR: "
                      "i_chip=0x%08x", i_chip->getHuid() );
            break;
        }

    }while(0);

    return SUCCESS;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, RdfRcdParityError );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[0:7] - Mainline MPE.
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
#define PLUGIN_FETCH_MPE_ERROR( RANK ) \
int32_t AnalyzeFetchMpe_##RANK( ExtensibleChip * i_chip, \
                                STEP_CODE_DATA_STRUCT & io_sc ) \
{ \
    MemRank rank ( RANK ); \
    MemEcc::analyzeFetchMpe<TYPE_OCMB_CHIP>( i_chip, rank, io_sc ); \
    return SUCCESS; \
} \
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeFetchMpe_##RANK );

PLUGIN_FETCH_MPE_ERROR( 0 )
PLUGIN_FETCH_MPE_ERROR( 1 )
PLUGIN_FETCH_MPE_ERROR( 2 )
PLUGIN_FETCH_MPE_ERROR( 3 )
PLUGIN_FETCH_MPE_ERROR( 4 )
PLUGIN_FETCH_MPE_ERROR( 5 )
PLUGIN_FETCH_MPE_ERROR( 6 )
PLUGIN_FETCH_MPE_ERROR( 7 )

#undef PLUGIN_FETCH_MPE_ERROR

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[8:9] - Mainline NCE and/or TCE.
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t AnalyzeFetchNceTce( ExtensibleChip * i_chip,
                            STEP_CODE_DATA_STRUCT & io_sc )
{
    MemEcc::analyzeFetchNceTce<TYPE_OCMB_CHIP>( i_chip, io_sc );
    return SUCCESS; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeFetchNceTce );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[14] - Mainline UE.
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t AnalyzeFetchUe( ExtensibleChip * i_chip,
                        STEP_CODE_DATA_STRUCT & io_sc )
{
    MemEcc::analyzeFetchUe<TYPE_OCMB_CHIP>( i_chip, io_sc );
    return SUCCESS; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeFetchUe );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[17] - Mainline read IUE.
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return PRD_NO_CLEAR_FIR_BITS if IUE threshold is reached, else SUCCESS.
 */
int32_t AnalyzeMainlineIue( ExtensibleChip * i_chip,
                            STEP_CODE_DATA_STRUCT & io_sc )
{
    int32_t rc = SUCCESS;
    MemEcc::analyzeMainlineIue<TYPE_OCMB_CHIP>( i_chip, io_sc );

    #ifdef __HOSTBOOT_MODULE

    if ( MemEcc::queryIueTh<TYPE_OCMB_CHIP>(i_chip, io_sc) )
        rc = PRD_NO_CLEAR_FIR_BITS;

    #endif

    return rc; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeMainlineIue );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[37] - Maint IUE.
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return PRD_NO_CLEAR_FIR_BITS if IUE threshold is reached, else SUCCESS.
 */
int32_t AnalyzeMaintIue( ExtensibleChip * i_chip,
                         STEP_CODE_DATA_STRUCT & io_sc )
{
    int32_t rc = SUCCESS;
    MemEcc::analyzeMaintIue<TYPE_OCMB_CHIP>( i_chip, io_sc );

    #ifdef __HOSTBOOT_MODULE

    if ( MemEcc::queryIueTh<TYPE_OCMB_CHIP>(i_chip, io_sc) )
        rc = PRD_NO_CLEAR_FIR_BITS;

    #endif

    return rc; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeMaintIue );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[19,39] - Mainline and Maint IMPE
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t AnalyzeImpe( ExtensibleChip * i_chip, STEP_CODE_DATA_STRUCT & io_sc )
{
    MemEcc::analyzeImpe<TYPE_OCMB_CHIP>( i_chip, io_sc );
    return SUCCESS; // nothing to return to rule code
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeImpe );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[13,16] - Mainline AUE and IAUE
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t AnalyzeFetchAueIaue( ExtensibleChip * i_chip,
                             STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::AnalyzeFetchAueIaue] "

    MemAddr addr;
    if ( SUCCESS != getMemReadAddr<TYPE_OCMB_CHIP>(i_chip,
                                                   MemAddr::READ_AUE_ADDR,
                                                   addr) )
    {
        PRDF_ERR( PRDF_FUNC "getMemReadAddr(0x%08x,READ_AUE_ADDR) failed",
                  i_chip->getHuid() );
    }
    else
    {
        MemRank rank = addr.getRank();
        MemoryMru mm { i_chip->getTrgt(), rank, MemoryMruData::CALLOUT_RANK };
        io_sc.service_data->SetCallout( mm, MRU_HIGH );
    }

    return SUCCESS; // nothing to return to rule code

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeFetchAueIaue );

//------------------------------------------------------------------------------

/**
 * @brief  RDFFIR[33] - Maintenance AUE
 * @param  i_chip OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t AnalyzeMaintAue( ExtensibleChip * i_chip,
                         STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::AnalyzeMaintAue] "

    MemAddr addr;
    if ( SUCCESS != getMemMaintAddr<TYPE_OCMB_CHIP>(i_chip, addr) )
    {
        PRDF_ERR( PRDF_FUNC "getMemMaintAddr(0x%08x) failed",
                  i_chip->getHuid() );
    }
    else
    {
        MemRank rank = addr.getRank();
        MemoryMru mm { i_chip->getTrgt(), rank, MemoryMruData::CALLOUT_RANK };
        io_sc.service_data->SetCallout( mm, MRU_HIGH );
    }

    return SUCCESS; // nothing to return to rule code

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, AnalyzeMaintAue );


//##############################################################################
//
//                               TLXFIR
//
//##############################################################################

/**
 * @brief  Clear/Mask TLXFIR[9]
 * @param  i_chip An OCMB chip.
 * @param  io_sc  The step code data struct.
 * @return SUCCESS
 */
int32_t clearAndMaskTlxtRe( ExtensibleChip * i_chip,
                           STEP_CODE_DATA_STRUCT & io_sc )
{
    #define PRDF_FUNC "[explorer_ocmb::clearAndMaskTlxtRe] "

    do
    {
        // If we are at threshold, mask TLXFIR[9].
        if ( io_sc.service_data->IsAtThreshold() )
        {
            SCAN_COMM_REGISTER_CLASS * tlxfir_mask_or =
                i_chip->getRegister( "TLXFIR_MASK_OR" );

            tlxfir_mask_or->SetBit(9);
            if ( SUCCESS != tlxfir_mask_or->Write() )
            {
                PRDF_ERR( PRDF_FUNC "Write() Failed on TLXFIR_MASK_OR: "
                          "i_chip=0x%08x", i_chip->getHuid() );
                break;
            }
        }

        // Clear TLXFIR[9]
        SCAN_COMM_REGISTER_CLASS * tlxfir_and =
            i_chip->getRegister( "TLXFIR_AND" );
        tlxfir_and->setAllBits();

        tlxfir_and->ClearBit(9);
        if ( SUCCESS != tlxfir_and->Write() )
        {
            PRDF_ERR( PRDF_FUNC "Write() Failed on TLXFIR_AND: "
                      "i_chip=0x%08x", i_chip->getHuid() );
            break;
        }
    }while(0);

    return SUCCESS;

    #undef PRDF_FUNC
}
PRDF_PLUGIN_DEFINE( explorer_ocmb, clearAndMaskTlxtRe );

} // end namespace explorer_ocmb

} // end namespace PRDF

