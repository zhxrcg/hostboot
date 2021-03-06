/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/explorer/procedures/hwp/memory/lib/exp_draminit_utils.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018,2020                        */
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
/// @file exp_draminit_utils.C
/// @brief Procedure definition to initialize DRAM
///
// *HWP HWP Owner: Andre Marin <aamarin@us.ibm.com>
// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#include <generic/memory/lib/utils/c_str.H>
#include <lib/exp_draminit_utils.H>
#include <lib/phy/exp_train_display.H>
#include <lib/phy/exp_train_handler.H>
#include <exp_inband.H>

namespace mss
{
namespace exp
{

///
/// @brief Check that the rsp_data size returned from the PHY_INIT command matches the expected size
///
/// @param[in] i_target OCMB target
/// @param[in] i_actual_size size enum expected for the given phy init mode
/// @param[in] i_mode phy init mode. Expected to be a valid enum value since we asserted as such in exp_draminit.C
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff matching, else MSS_EXP_INVALID_PHY_INIT_RSP_DATA_LENGTH
///
fapi2::ReturnCode check_rsp_data_size(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint16_t i_actual_size,
    const phy_init_mode i_mode)
{
    uint16_t l_expected_size = 0;

    switch (i_mode)
    {
        case phy_init_mode::NORMAL:
            l_expected_size = sizeof(user_response_msdg_t);
            break;

        case phy_init_mode::EYE_CAPTURE_STEP_1:
            l_expected_size = sizeof(user_2d_eye_response_1_msdg_t);
            break;

        case phy_init_mode::EYE_CAPTURE_STEP_2:
            l_expected_size = sizeof(user_2d_eye_response_2_msdg_t);
            break;

        default:
            // This really can't occur since we asserted phy_init_mode was valid in exp_draminit.C
            // We have bigger problems if we get here, implying somehow this bad value was passed to explorer
            FAPI_ASSERT(false,
                        fapi2::MSS_EXP_UNKNOWN_PHY_INIT_MODE()
                        .set_TARGET(i_target)
                        .set_VALUE(i_mode),
                        "%s Value for phy init mode for exp_draminit is unknown: %u expected 0 (NORMAL), 1 (EYE_CAPTURE_STEP_1), 2 (EYE_CAPTURE_STEP_2)",
                        mss::c_str(i_target), i_mode);
            break;
    }

    FAPI_ASSERT(l_expected_size == i_actual_size,
                fapi2::MSS_EXP_INVALID_PHY_INIT_RSP_DATA_LENGTH()
                .set_OCMB_TARGET(i_target)
                .set_PHY_INIT_MODE(i_mode)
                .set_EXPECTED_LENGTH(l_expected_size)
                .set_ACTUAL_LENGTH(i_actual_size),
                "%s PHY INIT response data buffer size 0x%x did not match expected size 0x%x for phy_init_mode %u",
                mss::c_str(i_target), i_actual_size, l_expected_size, i_mode);

    return fapi2::FAPI2_RC_SUCCESS;
fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Perform normal host FW phy init
///
/// @param[in] i_target OCMB target
/// @param[in] i_crc CRC value
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode host_fw_phy_normal_init(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint32_t i_crc)
{
    fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;
    host_fw_command_struct l_cmd;
    std::vector<uint8_t> l_rsp_data;

    // Issue full boot mode cmd though EXP-FW REQ buffer
    FAPI_TRY(send_host_phy_init_cmd(i_target, i_crc, phy_init_mode::NORMAL, l_cmd));
    FAPI_TRY(mss::exp::check_host_fw_response(i_target, l_cmd, l_rsp_data, l_rc));

    FAPI_TRY(check_rsp_data_size(i_target, l_rsp_data.size(), phy_init_mode::NORMAL));
    FAPI_TRY(mss::exp::read_and_display_normal_training_repsonse(i_target, l_rsp_data, l_rc));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Perform host FW phy init with eye capture
///
/// @param[in] i_target OCMB target
/// @param[in] i_crc CRC value
/// @param[in] i_phy_params PHY params struct
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
/// @note the goal here is to attempt to send both phy_inits even in the event of a bad return code from the read & display
///
fapi2::ReturnCode host_fw_phy_init_with_eye_capture(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint32_t i_crc,
        const user_input_msdg& i_phy_params)
{
    fapi2::ReturnCode l_check_response_1_rc = fapi2::FAPI2_RC_SUCCESS;
    fapi2::ReturnCode l_read_display_response_1_rc = fapi2::FAPI2_RC_SUCCESS;
    fapi2::ReturnCode l_check_response_2_rc = fapi2::FAPI2_RC_SUCCESS;
    fapi2::ReturnCode l_read_display_response_2_rc = fapi2::FAPI2_RC_SUCCESS;

    user_2d_eye_response_1_msdg l_response_1;
    user_2d_eye_response_2_msdg l_response_2;

    std::vector<uint8_t> l_rsp_data;

    // First, step 1
    {
        host_fw_command_struct l_cmd;
        FAPI_TRY(send_host_phy_init_cmd(i_target, i_crc, phy_init_mode::EYE_CAPTURE_STEP_1, l_cmd));

        // Return code output param is that of check::response
        // A fail of getRSP will go to fapi_try_exit
        FAPI_TRY(mss::exp::check_host_fw_response(i_target, l_cmd, l_rsp_data, l_check_response_1_rc));

        l_read_display_response_1_rc = check_rsp_data_size(i_target,
                                       l_rsp_data.size(),
                                       phy_init_mode::EYE_CAPTURE_STEP_1);

        // If the data is the right size, we can read and display it. Otherwise, skip the reading and try step 2
        // Check the return codes at the end
        if (l_read_display_response_1_rc == fapi2::FAPI2_RC_SUCCESS)
        {
            l_read_display_response_1_rc = mss::exp::read_and_display_user_2d_eye_response(i_target, l_rsp_data, l_response_1);
        }
    }
    l_rsp_data.clear();

    // Next, step 2
    {
        host_fw_command_struct l_cmd;
        uint32_t l_crc = 0;

        // Put user_input_msdg again for step 2 (overwritten by data buffer from step 1)
        FAPI_TRY( mss::exp::ib::putUserInputMsdg(i_target, i_phy_params, l_crc),
                  "Failed putUserInputMsdg() for %s", mss::c_str(i_target) );

        // Send cmd
        FAPI_TRY(send_host_phy_init_cmd(i_target, l_crc, phy_init_mode::EYE_CAPTURE_STEP_2, l_cmd));

        // Return code output param is that of check::response
        // A fail of getRSP will go to fapi_try_exit
        FAPI_TRY(mss::exp::check_host_fw_response(i_target, l_cmd, l_rsp_data, l_check_response_2_rc));

        l_read_display_response_2_rc = check_rsp_data_size(i_target,
                                       l_rsp_data.size(),
                                       phy_init_mode::EYE_CAPTURE_STEP_2);

        // If the data is the right size, we can read and display it. Otherwise, skip the reading.
        // Next, we will check these return codes
        if (l_read_display_response_2_rc == fapi2::FAPI2_RC_SUCCESS)
        {
            l_read_display_response_2_rc = mss::exp::read_and_display_user_2d_eye_response(i_target, l_rsp_data, l_response_2);
        }
    }

    // Check the return codes
    FAPI_TRY(process_eye_capture_return_codes(i_target,
             l_response_1,
             l_response_2,
             l_check_response_1_rc,
             l_check_response_2_rc));

    // Finally, check the display response return codes
    FAPI_TRY(l_read_display_response_1_rc, "Error ocurred reading/displaying eye capture response 1 of %s",
             mss::c_str(i_target));
    FAPI_TRY(l_read_display_response_2_rc, "Error ocurred reading/displaying eye capture response 2 of %s",
             mss::c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Process return codes from PHY init with eye capture operations
///
/// @param[in] i_target OCMB target
/// @param[in] i_response_1 response struct for EYE_CAPTURE_STEP_1
/// @param[in] i_response_2 response struct for EYE_CAPTURE_STEP_2
/// @param[in] i_response_1_rc response from check_host_fw_response from EYE_CAPTURE_STEP_1
/// @param[in] i_response_2_rc response from check_host_fw_response from EYE_CAPTURE_STEP_2
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else an error from above as defined in the function algorithm
/// @note return codes are passed by value, caller should not expect these to change
///
fapi2::ReturnCode process_eye_capture_return_codes(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const user_2d_eye_response_1_msdg& i_response_1,
        const user_2d_eye_response_2_msdg& i_response_2,
        fapi2::ReturnCode i_response_1_rc,
        fapi2::ReturnCode i_response_2_rc)
{
    const bool l_response_1_failed = i_response_1_rc != fapi2::FAPI2_RC_SUCCESS;
    const bool l_response_2_failed = i_response_2_rc != fapi2::FAPI2_RC_SUCCESS;

    if (l_response_2_failed)
    {
        FAPI_ERR("%s check_fw_host_response() for %s returned error code 0x%016llu",
                 mss::c_str(i_target), "EYE_CAPTURE_STEP_2", uint64_t(i_response_2_rc));

        mss::exp::bad_bit_interface<user_2d_eye_response_2_msdg> l_interface_2(i_response_2);
        FAPI_TRY(mss::record_bad_bits<mss::mc_type::EXPLORER>(i_target, l_interface_2));

        if (l_response_1_failed)
        {
            // Log response 2's error, and let's return response 1
            fapi2::logError(i_response_2_rc, fapi2::FAPI2_ERRL_SEV_RECOVERED);

            // logError sets the return code& to NULL. Set to FAPI2_RC_SUCCESS in case of use
            i_response_2_rc = fapi2::FAPI2_RC_SUCCESS;
        }

        FAPI_TRY(i_response_2_rc);
    }

    if (l_response_1_failed)
    {
        FAPI_ERR("%s check_fw_host_response() for %s returned error code 0x%016llu",
                 mss::c_str(i_target), "EYE_CAPTURE_STEP_1", uint64_t(i_response_1_rc));

        mss::exp::bad_bit_interface<user_2d_eye_response_1_msdg> l_interface_1(i_response_1);
        FAPI_TRY(mss::record_bad_bits<mss::mc_type::EXPLORER>(i_target, l_interface_1));

        return i_response_1_rc;
    }

    // Else, we did not see errors!
    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Send PHY init command given the provided phy mode and CRC
///
/// @param[in] i_target OCMB target
/// @param[in] i_crc CRC field
/// @param[in] i_phy_init_mode normal / eye capture step 1 or 2
/// @param[out] host_fw_command_struct used for initialization
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode send_host_phy_init_cmd(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint32_t i_crc,
        const uint8_t i_phy_init_mode,
        host_fw_command_struct& o_cmd)
{
    host_fw_command_struct l_cmd;

    // Issue full boot mode cmd though EXP-FW REQ buffer
    FAPI_TRY(setup_cmd_params(i_target, i_crc, sizeof(user_input_msdg), i_phy_init_mode, l_cmd));
    FAPI_TRY(mss::exp::ib::putCMD(i_target, l_cmd),
             "Failed putCMD() for  %s", mss::c_str(i_target));

    // Wait a bit for the command (and training) to complete
    // Value based on initial Explorer hardware in Cronus in i2c mode.
    // Training takes ~10ms with no trace, ~450ms with Explorer UART debug
    FAPI_TRY(fapi2::delay((mss::DELAY_1MS * 8), 200));

    o_cmd = l_cmd;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Get and check the host fw response from the explorer
///
/// @param[in] i_target OCMB chip
/// @param[in] i_cmd host_fw_command_struct used to generate the response
/// @param[out] o_rsp_data response data
/// @param[out] o_rc return code from mss::exp::check::response()
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode check_host_fw_response(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        host_fw_command_struct& i_cmd,
        std::vector<uint8_t>& o_rsp_data,
        fapi2::ReturnCode& o_rc)
{
    host_fw_response_struct l_response;

    FAPI_TRY(mss::exp::ib::getRSP(i_target, l_response, o_rsp_data),
             "Failed getRSP() for  %s", mss::c_str(i_target));

    o_rc = mss::exp::check::response(i_target, l_response, i_cmd);

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Reads and displays the normal draminit training response
///
/// @param[in] i_target OCMB target
/// @param[in] i_resp_data RESP data
/// @param[in] i_rc return code from checking response
/// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode read_and_display_normal_training_repsonse(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const std::vector<uint8_t> i_resp_data,
    const fapi2::ReturnCode i_rc)
{
    user_response_msdg l_train_response;

    // Proccesses the response data
    FAPI_TRY( mss::exp::read_normal_training_response(i_target, i_resp_data, l_train_response),
              "Failed read_normal_training_response for %s", mss::c_str(i_target));

    // Displays the training response
    FAPI_INF("%s displaying user response data version %u", mss::c_str(i_target), l_train_response.version_number)
    FAPI_TRY( mss::exp::train::display_normal_info(i_target, l_train_response));

    if(i_rc != fapi2::FAPI2_RC_SUCCESS)
    {
        mss::exp::bad_bit_interface<user_response_msdg> l_interface(l_train_response);
        FAPI_TRY(mss::record_bad_bits<mss::mc_type::EXPLORER>(i_target, l_interface));
        FAPI_TRY(i_rc, "mss::exp::check::response failed for %s", mss::c_str(i_target));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief host_fw_phy_init_command_struct structure setup
///
/// @param[in] i_target OCMB target
/// @param[in] i_cmd_data_crc the command data CRC
/// @param[in] i_cmd_length the length of the command present in the data buffer (if any)
/// @param[in] i_phy_init_mode PHY init mode
/// @param[out] o_cmd the command parameters to set
///
fapi2::ReturnCode setup_cmd_params(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint32_t i_cmd_data_crc,
    const uint32_t i_cmd_length,
    const uint8_t i_phy_init_mode,
    host_fw_command_struct& o_cmd)
{
    memset(&o_cmd, 0, sizeof(host_fw_command_struct));
    // Issue full boot mode cmd though EXP-FW REQ buffer
    // Explicit with all of these (including 0 values) to avoid ambiguity
    o_cmd.cmd_id = mss::exp::omi::EXP_FW_DDR_PHY_INIT;

    // Retrieve a unique sequence id for this transaction
    uint32_t l_counter = 0;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_OCMB_COUNTER, i_target, l_counter));
    o_cmd.request_identifier = l_counter;

    // With cmd_length > 0, data exists in the extended data buffer. Must set cmd_flags to 1. However,
    // eye capture step 2 needs cmd_flags to be set to zero as defined in MCHP spec section 5.4.3 Eye Capture
    // despite having data in the extended data buffer
    o_cmd.cmd_flags = ((i_cmd_length > 0) && (i_phy_init_mode != phy_init_mode::EYE_CAPTURE_STEP_2)) ? 1 : 0;

    o_cmd.cmd_length = i_cmd_length;
    o_cmd.cmd_crc = i_cmd_data_crc;
    o_cmd.host_work_area = 0;
    o_cmd.cmd_work_area = 0;

    // According to the spec Table 5-2, phy_init_mode takes the place of command_argument[0]
    o_cmd.command_argument[0] = i_phy_init_mode;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief user_input_msdg structure setup
/// @tparam T the fapi2 TargetType
/// @param[in] i_target the fapi2 target
/// @param[out] o_phy_params the phy params data struct
/// @return FAPI2_RC_SUCCESS iff okay
///
fapi2::ReturnCode setup_phy_params(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                   user_input_msdg& o_phy_params)
{
    for (const auto l_port : mss::find_targets<fapi2::TARGET_TYPE_MEM_PORT>(i_target))
    {
        fapi2::ReturnCode l_rc;
        const phy_params l_set_phy_params(l_port, l_rc);
        FAPI_TRY(l_rc, "Unable to instantiate phy_params for target %s", mss::c_str(i_target));

        // Set the params by fetching them from the attributes
        FAPI_TRY(l_set_phy_params.set_version_number(o_phy_params));
        FAPI_TRY(l_set_phy_params.setup_DimmType(o_phy_params));
        FAPI_TRY(l_set_phy_params.setup_CsPresent(o_phy_params));
        FAPI_TRY(l_set_phy_params.setup_DramDataWidth(o_phy_params));
        FAPI_TRY(l_set_phy_params.setup_Height3DS(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ActiveDBYTE(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ActiveNibble(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_AddrMirror(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ColumnAddrWidth(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RowAddrWidth(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_SpdCLSupported(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_SpdtAAmin(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_Rank4Mode(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_EncodedQuadCs(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_DDPCompatible(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_TSV8HSupport(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_MRAMSupport(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_MDSSupport(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_NumPStates(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_Frequency(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_PhyOdtImpedance(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_PhyDrvImpedancePU(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_PhyDrvImpedancePD(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_PhySlewRate(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ATxImpedance(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ATxSlewRate(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_CKTxImpedance(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_CKTxSlewRate(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_AlertOdtImpedance(o_phy_params));

        // TK to use the rank API once it's available
        // For now we are assuming ranks 2 and 3 are on DIMM1 for RttNom, RttWr and RttPark
        FAPI_TRY(l_set_phy_params.set_RttNom(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RttWr(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RttPark(o_phy_params));

        FAPI_TRY(l_set_phy_params.set_DramDic(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_DramWritePreamble(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_DramReadPreamble(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_PhyEqualization(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_InitVrefDQ(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_InitPhyVref(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_OdtWrMapCs(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_OdtRdMapCs(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_Geardown(o_phy_params));

        // TK need to check if this also includes RC0E
        FAPI_TRY(l_set_phy_params.set_CALatencyAdder(o_phy_params));

        FAPI_TRY(l_set_phy_params.set_BistCALMode(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_BistCAParityLatency(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RcdDic(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RcdVoltageCtrl(o_phy_params));

        // TK check bit ordering here for RcdIBTCtrl and RcdDBDic
        FAPI_TRY(l_set_phy_params.set_RcdIBTCtrl(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_RcdDBDic(o_phy_params));

        FAPI_TRY(l_set_phy_params.set_RcdSlewRate(o_phy_params));

        FAPI_TRY(l_set_phy_params.set_DFIMRL_DDRCLK(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ATxDly_A(o_phy_params));
        FAPI_TRY(l_set_phy_params.set_ATxDly_B(o_phy_params));
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

namespace check
{

///
/// @brief Checks explorer response argument for a successful command
/// @param[in] i_target OCMB target
/// @param[in] i_rsp response command
/// @return FAPI2_RC_SUCCESS iff okay
///
fapi2::ReturnCode response(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                           const host_fw_response_struct& i_rsp,
                           const host_fw_command_struct& i_cmd)
{
    fapi2::buffer<uint32_t> l_error_code;

    l_error_code.insertFromRight<0, BITS_PER_BYTE>(i_rsp.response_argument[4]).
    insertFromRight<BITS_PER_BYTE, BITS_PER_BYTE>(i_rsp.response_argument[3]).
    insertFromRight<2 * BITS_PER_BYTE, BITS_PER_BYTE>(i_rsp.response_argument[2]).
    insertFromRight<3 * BITS_PER_BYTE, BITS_PER_BYTE>(i_rsp.response_argument[1]);

    // Check if cmd was successful
    FAPI_ASSERT(i_rsp.response_argument[0] == omi::response_arg::SUCCESS &&
                i_rsp.request_identifier == i_cmd.request_identifier,
                fapi2::MSS_EXP_RSP_ARG_FAILED().
                set_TARGET(i_target).
                set_RSP_ID(i_rsp.response_id).
                set_ERROR_CODE(l_error_code).
                set_EXPECTED_REQID(i_cmd.request_identifier).
                set_ACTUAL_REQID(i_rsp.request_identifier),
                "Failed to initialize the PHY for %s, response=0x%X, error_code=0x%08X "
                "RSP RQ ID: %u CMD RQ ID: %u",
                mss::c_str(i_target), i_rsp.response_argument[0], l_error_code,
                i_rsp.request_identifier, i_cmd.request_identifier);

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

}//check

}// exp
}// mss
