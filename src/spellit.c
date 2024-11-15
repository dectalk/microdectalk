/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    � Digital Equipment Corporation 1995. All rights reserved.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Digital Equipment Corporation and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Digital or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:	spellit.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  Spell vs. Speak rules table.
 *
 ***********************************************************************
 *    Revision History:
 *  Generated from : \dtpc\src\lts\spellit.tab
 *  Written to file : \dtpc\src\lts\spellit.c
 *  Created on : 03/02/93 at 10:27:05
 */
const unsigned char spell_it[26][26] = {
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x01,  0x00,  0x00,
     0x02,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,
     0x03,  0x00,  0x02,  0x03,  0x00,  0x03,  0x03,  0x00,  0x03,
     0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x00,  0x03,
     0x03,  0x03,  0x00,  0x03,  0x03,  0x01,  0x00,  0x03,  0x03,
     0x00,  0x03,  0x02,  0x00,  0x03,  0x03,  0x00,  0x03,  0x03,
     0x00,  0x03,  0x03,  0x03,  0x00,  0x01,  0x00,  0x03,  0x03,
     0x02,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x02,
     0x03,  0x02,  0x00,  0x03,  0x03,  0x00,  0x03,  0x03,  0x00,
     0x03,  0x00,  0x03,  0x00,  0x03,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x02,  0x03,  0x03,  0x00,  0x01,  0x03,  0x00,  0x03,  0x02,
     0x00,  0x03,  0x03,  0x00,  0x03,  0x03,  0x00,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,
     0x02,  0x01,  0x00,  0x03,  0x03,  0x01,  0x03,  0x00,  0x00,
     0x03,  0x03,  0x00,  0x03,  0x03,  0x00,  0x03,  0x01,  0x03,
     0x00,  0x03,  0x00,  0x02,  0x02,  0x03,  0x00,  0x03,  0x02,
     0x03,  0x00,  0x03,  0x02,  0x02,  0x03,  0x03,  0x00,  0x02,
     0x03,  0x03,  0x02,  0x02,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x03,  0x02,  0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00,
     0x03,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00,  0x03,  0x00,
     0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x02,  0x03,  0x02,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,
     0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x00,
     0x03,  0x02,  0x03,  0x00,  0x02,  0x03,  0x01,  0x00,  0x03,
     0x03,  0x00,  0x03,  0x00,  0x00,  0x03,  0x03,  0x00,  0x02,
     0x03,  0x00,  0x03,  0x00,  0x03,  0x00,  0x03,  0x00,  0x03,
     0x03,  0x03,  0x00,  0x03,  0x03,  0x02,  0x00,  0x03,  0x03,
     0x00,  0x03,  0x03,  0x00,  0x03,  0x03,  0x02,  0x03,  0x03,
     0x00,  0x03,  0x02,  0x03,  0x00,  0x03,  0x00,  0x03,  0x01,
     0x03,  0x00,  0x03,  0x02,  0x02,  0x00,  0x03,  0x03,  0x02,
     0x02,  0x01,  0x00,  0x03,  0x03,  0x02,  0x02,  0x03,  0x00,
     0x03,  0x03,  0x03,  0x00,  0x03,  0x00,  0x03,  0x03,  0x03,
     0x00,  0x03,  0x02,  0x02,  0x00,  0x03,  0x03,  0x03,  0x02,
     0x02,  0x00,  0x03,  0x03,  0x02,  0x03,  0x03,  0x00,  0x03,
     0x02,  0x03,  0x00,  0x03,  0x00,  0x00,  0x00,  0x00,  0x02,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x02,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x03,  0x00,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,
     0x03,  0x01,  0x00,  0x03,  0x03,  0x00,  0x02,  0x00,  0x00,
     0x02,  0x03,  0x00,  0x02,  0x01,  0x00,  0x03,  0x03,  0x03,
     0x00,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x03,  0x03,  0x03,  0x03,  0x00,  0x03,
     0x03,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x03,  0x00,  0x03,  0x03,  0x02,  0x00,  0x03,  0x03,  0x01,
     0x00,  0x03,  0x03,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,
     0x00,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,
     0x00,  0x02,  0x00,  0x02,  0x00,  0x02,  0x02,  0x00,  0x00,
     0x02,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x03,  0x02,
     0x02,  0x00,  0x00,  0x00,  0x00,  0x03,  0x00,  0x03,  0x00,
     0x02,  0x02,  0x03,  0x00,  0x02,  0x03,  0x01,  0x00,  0x03,
     0x03,  0x02,  0x00,  0x02,  0x00,  0x02,  0x03,  0x00,  0x00,
     0x02,  0x00,  0x03,  0x00,  0x02,  0x00,  0x03,  0x02,  0x00,
     0x00,  0x00,  0x03,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x02,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x03,  0x00,  0x00,  0x00,  0x03,  0x00,  0x00,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,
     0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x03,  0x03,  0x00,
     0x03,  0x03,  0x03,  0x00,  0x03,  0x00,  0x03,  0x03,  0x02,
     0x00,  0x03,  0x03,  0x01,  0x00,  0x03,  0x03,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x03,  0x00,  0x03,  0x03,  0x00,  0x03,
     0x03,  0x03,  0x00,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x02,  0x03,  0x02,
     0x00,  0x03,  0x03,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x01,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
     0x01,  0x00,  0x00,  0x03,  0x03,  0x02,  0x00,  0x03,  0x03,
     0x03,  0x00,  0x03,  0x03,  0x03,  0x03,  0x02,  0x00,  0x03,
     0x03,  0x03,  0x03,  0x03,  0x00,  0x03,  0x03,  0x03,  0x00,
     0x02
};