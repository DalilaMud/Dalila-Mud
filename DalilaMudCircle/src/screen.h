/* ************************************************************************
*   File: screen.h                                      Part of CircleMUD *
*  Usage: header file with ANSI color codes for online color              *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"
#define KBLK  "\033[30m"
#define KBLD  "\033[1m"
#define KBLN  "\033[5m"
#define KNUL  ""
#define KCLR  "\033[2J"
#define KUND  "\033[4m"
#define KDAR  "\033[2m"

#define KBRED  "\033[41m"
#define KBGRN  "\033[42m"
#define KBYEL  "\033[43m"
#define KBBLU  "\033[44m"
#define KBMAG  "\033[45m"
#define KBCYN  "\033[46m"
#define KBWHT  "\033[47m"
#define KBBLK  "\033[40m"

/* conditional color.  pass it a pointer to a char_data and a color level. */
#define C_OFF   0
#define C_SPR   1
#define C_NRM   2
#define C_CMP   3
#define _clrlevel(ch) ((PRF_FLAGGED((ch), PRF_COLOR_1) ? 1 : 0) + \
                       (PRF_FLAGGED((ch), PRF_COLOR_2) ? 2 : 0))
#define clr(ch,lvl) (_clrlevel(ch) >= (lvl))
#define CCNRM(ch,lvl)  (clr((ch),(lvl))?KNRM:KNUL)
#define CCBLK(ch,lvl)  (clr((ch),(lvl))?KBLK:KNUL)
#define CCRED(ch,lvl)  (clr((ch),(lvl))?KRED:KNUL)
#define CCGRN(ch,lvl)  (clr((ch),(lvl))?KGRN:KNUL)
#define CCYEL(ch,lvl)  (clr((ch),(lvl))?KYEL:KNUL)
#define CCBLU(ch,lvl)  (clr((ch),(lvl))?KBLU:KNUL)
#define CCMAG(ch,lvl)  (clr((ch),(lvl))?KMAG:KNUL)
#define CCCYN(ch,lvl)  (clr((ch),(lvl))?KCYN:KNUL)
#define CCWHT(ch,lvl)  (clr((ch),(lvl))?KWHT:KNUL)
#define CCBLD(ch,lvl)  (clr((ch),(lvl))?KBLD:KNUL)
#define CCBLN(ch,lvl)  (clr((ch),(lvl))?KBLN:KNUL)
#define CCCLR(ch,lvl)  (clr((ch),(lvl))?KCLR:KNUL)
#define CCUND(ch,lvl)  (clr((ch),(lvl))?KUND:KNUL)
#define CCDAR(ch,lvl)  (clr((ch),(lvl))?KDAR:KNUL)

#define CCBBLK(ch,lvl)  (clr((ch),(lvl))?KBBLK:KNUL)
#define CCBRED(ch,lvl)  (clr((ch),(lvl))?KBRED:KNUL)
#define CCBGRN(ch,lvl)  (clr((ch),(lvl))?KBGRN:KNUL)
#define CCBYEL(ch,lvl)  (clr((ch),(lvl))?KBYEL:KNUL)
#define CCBBLU(ch,lvl)  (clr((ch),(lvl))?KBBLU:KNUL)
#define CCBMAG(ch,lvl)  (clr((ch),(lvl))?KBMAG:KNUL)
#define CCBCYN(ch,lvl)  (clr((ch),(lvl))?KBCYN:KNUL)
#define CCBWHT(ch,lvl)  (clr((ch),(lvl))?KBWHT:KNUL)

#define COLOR_LEV(ch) (_clrlevel(ch))

#define QNRM CCNRM(ch,C_SPR)
#define QBLK CCBLK(ch,C_SPR)
#define QRED CCRED(ch,C_SPR)
#define QGRN CCGRN(ch,C_SPR)
#define QYEL CCYEL(ch,C_SPR)
#define QBLU CCBLU(ch,C_SPR)
#define QMAG CCMAG(ch,C_SPR)
#define QCYN CCCYN(ch,C_SPR)
#define QWHT CCWHT(ch,C_SPR)
#define QBLD CCBLD(ch,C_SPR)
#define QBLN CCBLN(ch,C_SPR)
#define QCLR CCBLN(ch,C_SPR)
#define QUND CCUND(ch,C_SPR)
#define QDAR CCDAR(ch,C_SPR)
#define QBBLK CCBBLK(ch,C_SPR)
#define QBRED CCBRED(ch,C_SPR)
#define QBGRN CCBGRN(ch,C_SPR)
#define QBYEL CCBYEL(ch,C_SPR)
#define QBBLU CCBBLU(ch,C_SPR)
#define QBMAG CCBMAG(ch,C_SPR)
#define QBCYN CCBCYN(ch,C_SPR)
#define QBWHT CCBWHT(ch,C_SPR)
















