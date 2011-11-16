//  IBM_PROLOG_BEGIN_TAG
//  This is an automatically generated prolog.
//
//  $Source: src/include/string_ext.h $
//
//  IBM CONFIDENTIAL
//
//  COPYRIGHT International Business Machines Corp. 2011
//
//  p1
//
//  Object Code Only (OCO) source materials
//  Licensed Internal Code Source Materials
//  IBM HostBoot Licensed Internal Code
//
//  The source code for this program is not published or other-
//  wise divested of its trade secrets, irrespective of what has
//  been deposited with the U.S. Copyright Office.
//
//  Origin: 30
//
//  IBM_PROLOG_END
#ifndef __STRING_EXT_H
#define __STRING_EXT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Converts lowercase string to uppercase
 *
 * Any characters that cannot be converted are left unchanged
 *
 * @param[in] s Pointer to c-string that is converted to uppercase
 * @return char *. Pointer to beginning of string (same as 's' parameter)
 */
char* strupr(char* s);

#ifdef __cplusplus
};
#endif

#endif
