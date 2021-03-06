/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/vpd/dvpd.H $                                          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2013,2019                        */
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
#ifndef __DVPD_H
#define __DVPD_H


/**
 * @file dvpd.H
 *
 * @brief Provides the interfaces for the DVPD device driver
 *
 */

// ----------------------------------------------
// Includes
// ----------------------------------------------
#include <errl/errlentry.H>
#include <vpd/dvpdenums.H>

#include "ipvpd.H"

namespace DVPD
{

    enum
    {
        // For a given MCS we need selector*2 + MR + MT =
        //     256*2 + 16*128 + 4*128 = 3KB.
        // There can be up to 8 MCS units (2 Nimbus chips) sharing a
        // single eeprom. So we would need 24KB per record.
        MAX_SECTIONS       = 8,
        SECTION_SIZE       = 0x8000,
    };

//TODO RTC 144519 Replace with list of direct memory records and keywords
//                from memory team.
    /**
     * @brief Conversion of DVPD Records to corresponding character
     *        representation.
     */
    const IpVpdFacade::recordInfo dvpdRecords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There is a
        //      testcase that checks this.  When adding new entries to the
        //      list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { VINI, "VINI" },
        { MEMD, "MEMD" },
        { LXR0, "LXR0" },
        { VCEN, "VCEN" },
        { VMSC, "VMSC" },
        { VRTN, "VRTN" },
        { VSRC, "VSRC" },
        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { DVPD_TEST_RECORD, "TEST" },
        // -------------------------------------------------------------------
    };

    /**
     * @brief Conversion of DVPD Keywords to corresponding character
     *        representation.
     */
    const IpVpdFacade::keywordInfo dvpdKeywords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There is a
        //      testcase that checks this.  When adding new entries to
        //      the list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { B3,   "B3" },
        { B4,   "B4" },
        { B7,   "B7" },
        { CC,   "CC" },
        { CE,   "CE" },
        { CT,   "CT" },
        { DR,   "DR" },
        { FC,   "FC" },
        { FN,   "FN" },
        { HE,   "HE" },
        { HW,   "HW" },
        { I2,   "I2" },
        { IN,   "IN" },
        { J0,   "J0" },
        { J1,   "J1" },
        { J2,   "J2" },
        { J3,   "J3" },
        { J4,   "J4" },
        { J5,   "J5" },
        { J6,   "J6" },
        { J7,   "J7" },
        { J8,   "J8" },
        { J9,   "J9" },
        { JA,   "JA" },
        { JB,   "JB" },
        { JC,   "JC" },
        { JD,   "JD" },
        { JE,   "JE" },
        { JF,   "JF" },
        { JG,   "JG" },
        { JH,   "JH" },
        { JI,   "JI" },
        { JJ,   "JJ" },
        { JK,   "JK" },
        { JL,   "JL" },
        { JM,   "JM" },
        { JN,   "JN" },
        { JO,   "JO" },
        { JP,   "JP" },
        { JQ,   "JQ" },
        { JR,   "JR" },
        { JS,   "JS" },
        { JT,   "JT" },
        { JU,   "JU" },
        { JV,   "JV" },
        { JW,   "JW" },
        { JX,   "JX" },
        { JY,   "JY" },
        { JZ,   "JZ" },
        { LX,   "LX" },
        { MR,   "MR" },
        { MT,   "MT" },
        { PF,   "PF" },
        { PN,   "PN" },
        { PR,   "PR" },
        { RB,   "RB" },
        { RG,   "RG" },
        { RT,   "RT" },
        { SE,   "SE" },
        { SN,   "SN" },
        { SO,   "SO" },
        { TM,   "TM" },
        { VM,   "VM" },
        { VZ,   "VZ" },
        { X0,   "X0" },
        { X1,   "X1" },
        { X2,   "X2" },
        { X3,   "X3" },
        { X4,   "X4" },
        { X5,   "X5" },
        { X6,   "X6" },
        { X7,   "X7" },
        { X8,   "X8" },
        { X9,   "X9" },
        { XA,   "XA" },
        { XB,   "XB" },
        { XC,   "XC" },
        { XD,   "XD" },
        { XE,   "XE" },
        { XF,   "XF" },
        { XG,   "XG" },
        { XH,   "XH" },
        { XI,   "XI" },
        { XJ,   "XJ" },
        { XK,   "XK" },
        { XL,   "XL" },
        { XM,   "XM" },
        { XN,   "XN" },
        { XO,   "XO" },
        { XP,   "XP" },
        { XQ,   "XQ" },
        { XR,   "XR" },
        { XS,   "XS" },
        { XT,   "XT" },
        { XU,   "XU" },
        { XV,   "XV" },
        { XW,   "XW" },
        { XX,   "XX" },
        { XY,   "XY" },
        { XZ,   "XZ" },
        { Q0,   "Q0" },
        { Q1,   "Q1" },
        { Q2,   "Q2" },
        { Q3,   "Q3" },
        { Q4,   "Q4" },
        { Q5,   "Q5" },
        { Q6,   "Q6" },
        { Q7,   "Q7" },
        { Q8,   "Q8" },
        { CK,   "CK" },

        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { DVPD_TEST_KEYWORD, "ZZ" },
        // -------------------------------------------------------------------

        //Common ipvpd
        { FULL_RECORD, "FL"},
    };

}; // end DVPD namespace

class DvpdFacade: public IpVpdFacade
{
  public:

    /**
     * @brief Constructor
     */
    DvpdFacade( );

#ifdef CONFIG_SECUREBOOT
    /**
     * @brief Load/unload the appropriate secure section for
     *        an overriden PNOR section
     * @param[in] i_args    Record/keyword
     * @param[in] i_target  Target pointer
     * @param[in] i_load    true=load, false=unload
     * @param[out] o_loaded true=section loaded, false=section not loaded
     * @return Error log
     */
    virtual errlHndl_t loadUnloadSecureSection( input_args_t i_args,
                                                TARGETING::Target* i_target,
                                                bool i_load,
                                                bool& o_loaded );
#endif

  private:

    /**
     * @brief This function returns a primary and an alternate list of records
     *       that should be copied to pnor. The Alternate list is optional.
     *
     * @param[out] o_primaryVpdRecords - Pointer to array of VPD Records to use
     *
     * @param[out] o_primaryRecSize -    Size of o_primaryVpdRecords array
     *
     * @param[out] o_altVpdRecords -     Pointer to array of VPD Records to use
     *
     * @param[out] o_altRecSize -        Size of o_altVpdRecords array
     *
     */
    void getRecordLists(
                const  recordInfo* & o_primaryVpdRecords,
                uint64_t           & o_primaryRecSize,
                const  recordInfo* & o_altVpdRecords,
                uint64_t           & o_altRecSize);

  protected:
    /**
     * @brief Callback function to check for a record override and
     *        set iv_overridePtr appropriately
     * @param[in] i_record  Record name
     * @param[in] i_target  Target pointer
     * @param[out] o_ptr    Pointer to location of record in PNOR,
     *                      ==nullptr if there is no override
     * @return Error log
     */
    virtual errlHndl_t checkForRecordOverride( const char* i_record,
                                               TARGETING::Target* i_target,
                                               uint8_t*& o_ptr );

};
#endif  // __DVPD_H
