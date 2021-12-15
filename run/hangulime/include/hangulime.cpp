/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  hangulime.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2018/06/13
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 SHI
** Created date:               2018/06/13
** Descriptions:               
** 
*********************************************************************************************************/
#include "hangulime.h"

#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

namespace HANGULIME
{
    enum HangulMedialIndex
    {                                 // VIndex    Letter Jungseong   Double Jamo
        // ----------------------------------------------------------------------
        HANGUL_MEDIAL_A,              //      0      314F      1161
        HANGUL_MEDIAL_AE,             //      1      3150      1162
        HANGUL_MEDIAL_YA,             //      2      3151      1163
        HANGUL_MEDIAL_YAE,            //      3      3152      1164
        HANGUL_MEDIAL_EO,             //      4      3153      1165
        HANGUL_MEDIAL_E,              //      5      3154      1166
        HANGUL_MEDIAL_YEO,            //      6      3155      1167
        HANGUL_MEDIAL_YE,             //      7      3156      1168
        HANGUL_MEDIAL_O,              //      8      3157      1169
        HANGUL_MEDIAL_WA,             //      9      3158      116A     3157+314F
        HANGUL_MEDIAL_WAE,            //     10      3159      116B     3157+3150
        HANGUL_MEDIAL_OE,             //     11      315A      116C     3157+3163
        HANGUL_MEDIAL_YO,             //     12      315B      116D
        HANGUL_MEDIAL_U,              //     13      315C      116E
        HANGUL_MEDIAL_WEO,            //     14      315D      116F     315C+3153
        HANGUL_MEDIAL_WE,             //     15      315E      1170     315C+3154
        HANGUL_MEDIAL_WI,             //     16      315F      1171     315C+3163
        HANGUL_MEDIAL_YU,             //     17      3160      1172
        HANGUL_MEDIAL_EU,             //     18      3161      1173
        HANGUL_MEDIAL_YI,             //     19      3162      1174     3161+3163
        HANGUL_MEDIAL_I               //     20      3163      1175
    };

    enum HangulFinalIndex
    {                                 // TIndex    Letter Jongseong   Double Jamo
        // ----------------------------------------------------------------------
        HANGUL_FINAL_NONE,            //      0       n/a       n/a
        HANGUL_FINAL_KIYEOK,          //      1      3131      11A8
        HANGUL_FINAL_SSANGKIYEOK,     //      2      3132      11A9
        HANGUL_FINAL_KIYEOK_SIOS,     //      3      3133      11AA     3131+3145
        HANGUL_FINAL_NIEUN,           //      4      3134      11AB
        HANGUL_FINAL_NIEUN_CIEUC,     //      5      3135      11AC     3134+3148
        HANGUL_FINAL_NIEUN_HIEUH,     //      6      3136      11AD     3134+314E
        HANGUL_FINAL_TIKEUT,          //      7      3137      11AE
        HANGUL_FINAL_RIEUL,           //      8      3139      11AF
        HANGUL_FINAL_RIEUL_KIYEOK,    //      9      313A      11B0     3139+3131
        HANGUL_FINAL_RIEUL_MIEUM,     //     10      313B      11B1     3139+3141
        HANGUL_FINAL_RIEUL_PIEUP,     //     11      313C      11B2     3139+3142
        HANGUL_FINAL_RIEUL_SIOS,      //     12      313D      11B3     3139+3145
        HANGUL_FINAL_RIEUL_THIEUTH,   //     13      313E      11B4     3139+314C
        HANGUL_FINAL_RIEUL_PHIEUPH,   //     14      313F      11B5     3139+314D
        HANGUL_FINAL_RIEUL_HIEUH,     //     15      3140      11B6     3139+314E
        HANGUL_FINAL_MIEUM,           //     16      3141      11B7
        HANGUL_FINAL_PIEUP,           //     17      3142      11B8
        HANGUL_FINAL_PIEUP_SIOS,      //     18      3144      11B9     3142+3145
        HANGUL_FINAL_SIOS,            //     19      3145      11BA
        HANGUL_FINAL_SSANGSIOS,       //     20      3146      11BB     3145+3145
        HANGUL_FINAL_IEUNG,           //     21      3147      11BC
        HANGUL_FINAL_CIEUC,           //     22      3148      11BD
        HANGUL_FINAL_CHIEUCH,         //     23      314A      11BE
        HANGUL_FINAL_KHIEUKH,         //     24      314B      11BF
        HANGUL_FINAL_THIEUTH,         //     25      314C      11C0
        HANGUL_FINAL_PHIEUPH,         //     26      314D      11C1
        HANGUL_FINAL_HIEUH            //     27      314E      11C2
    };

    unsigned short _initials[] = 
    {
        0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141,
        0x3142, 0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149,
        0x314A, 0x314B, 0x314C, 0x314D, 0x314E
    };
    vector<unsigned short> initials(_initials, _initials + sizeof(_initials) / sizeof(unsigned short));

    unsigned short _finals[] =
    {
        0x0000, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136,
        0x3137, 0x3139, 0x313A, 0x313B, 0x313C, 0x313D, 0x313E,
        0x313F, 0x3140, 0x3141, 0x3142, 0x3144, 0x3145, 0x3146,
        0x3147, 0x3148, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
    };
    vector<unsigned short> finals(_finals, _finals + sizeof(_finals) / sizeof(unsigned short));

    map<unsigned short, HangulMedialIndex> doubleMedialMap;
    map<unsigned short, HangulFinalIndex> doubleFinalMap;

    const int SBase = 0xAC00;
    const int LBase = 0x1100;
    const int VBase = 0x314F;
    const int TBase = 0x11A7;
    const int LCount = 19;
    const int VCount = 21;
    const int TCount = 28;
    const int NCount = VCount * TCount; // 588
    const int SCount = LCount * NCount; // 11172

    void initDoubleMap()
    {
        doubleMedialMap[8] = HANGUL_MEDIAL_WA;
        doubleMedialMap[264] = HANGUL_MEDIAL_WAE;
        doubleMedialMap[5128] = HANGUL_MEDIAL_OE;
        doubleMedialMap[1037] = HANGUL_MEDIAL_WEO;
        doubleMedialMap[1293] = HANGUL_MEDIAL_WE;
        doubleMedialMap[5133] = HANGUL_MEDIAL_WI;
        doubleMedialMap[5138] = HANGUL_MEDIAL_YI;

        doubleFinalMap[4865] = HANGUL_FINAL_KIYEOK_SIOS;
        doubleFinalMap[5636] = HANGUL_FINAL_NIEUN_CIEUC;
        doubleFinalMap[6916] = HANGUL_FINAL_NIEUN_HIEUH;
        doubleFinalMap[264] = HANGUL_FINAL_RIEUL_KIYEOK;
        doubleFinalMap[4104] = HANGUL_FINAL_RIEUL_MIEUM;
        doubleFinalMap[4360] = HANGUL_FINAL_RIEUL_PIEUP;
        doubleFinalMap[4872] = HANGUL_FINAL_RIEUL_SIOS;
        doubleFinalMap[6408] = HANGUL_FINAL_RIEUL_THIEUTH;
        doubleFinalMap[6664] = HANGUL_FINAL_RIEUL_PHIEUPH;
        doubleFinalMap[6920] = HANGUL_FINAL_RIEUL_HIEUH;
        doubleFinalMap[4881] = HANGUL_FINAL_PIEUP_SIOS;
        doubleFinalMap[4883] = HANGUL_FINAL_SSANGSIOS;
    }

    bool isMedial(HangulMedialIndex vowel)
    {
        return vowel >= HANGUL_MEDIAL_A && vowel <= HANGUL_MEDIAL_I;
    }

    bool isFinal(HangulFinalIndex consonant)
    {
        return consonant >= HANGUL_FINAL_KIYEOK && consonant <= HANGUL_FINAL_HIEUH;
    }

    unsigned short findDoubleMedial(HangulMedialIndex vowel)
    {
        unsigned short key = 0;
        map<unsigned short, HangulMedialIndex>::iterator it;
        for(it = doubleMedialMap.begin(); it != doubleMedialMap.end(); it++)
        {
            if(it->second == vowel)
                key = it->first;
        }
        return key;
    }

    unsigned short findDoubleFinal(HangulFinalIndex consonant)
    {
        unsigned short key = 0;
        map<unsigned short, HangulFinalIndex>::iterator it;
        for(it = doubleFinalMap.begin(); it != doubleFinalMap.end(); it++)
        {
            if(it->second == consonant)
                key = it->first;
        }
        return key;
    }

    unsigned short packDoubleMedial(HangulMedialIndex a, HangulMedialIndex b)
    {
        return (unsigned short)a | ((unsigned short)b << 8);
    }

    unsigned short packDoubleFinal(HangulFinalIndex a, HangulFinalIndex b)
    {
        return (unsigned short)a | ((unsigned short)b << 8);
    }

    void unpackDoubleMedial(unsigned short key, HangulMedialIndex &a, HangulMedialIndex &b)
    {
        a = (HangulMedialIndex)(key & 0xFF);
        b = (HangulMedialIndex)(key >> 8);
    }

    void unpackDoubleFinal(unsigned short key, HangulFinalIndex &a, HangulFinalIndex &b)
    {
        a = (HangulFinalIndex)(key & 0xFF);
        b = (HangulFinalIndex)(key >> 8);
    }

    list<unsigned short> compose(list<unsigned short> source)
    {
        list<unsigned short> result;
        const int len = source.size();
        if (len == 0)
            return result;

        initDoubleMap();

        // Always add the initial character into buffer.
        // The last character will serve as the current
        // Hangul Syllable.
        unsigned short last = source.front();
        result.push_back(last);
        source.pop_front();

        // Go through the input buffer starting at next character
        list<unsigned short>::iterator it;
        for (it = source.begin(); it != source.end(); it++)
        {
            // Check to see if the character is Hangul Compatibility Jamo
            unsigned short unicode = (*it);
            if (isJamo(unicode))
            {
                // Check to see if the character is syllable
                unsigned short lastUnicode = last;
                int SIndex = (int)lastUnicode - SBase;
                if (SIndex >= 0 && SIndex < SCount)
                {
                    // Check to see if the syllable type is LV or LV+T
                    int TIndex = SIndex % TCount;
                    if (TIndex == 0)
                    {
                        // If the current character is final consonant, then
                        // make syllable of form LV+T
                        vector<unsigned short>::iterator it = find(finals.begin(), finals.end(), unicode);
                        if (it != finals.end())
                        {
                            TIndex = distance(finals.begin(),it);
                            last = lastUnicode + TIndex;
                            result.pop_back();
                            result.push_back(last);
                            continue;
                        }

                        // Check to see if the current character is vowel
                        HangulMedialIndex VIndexB = (HangulMedialIndex)((int)unicode - VBase);
                        if (isMedial(VIndexB))
                        {
                            // Some medial Jamos do not exist in the keyboard layout as is.
                            // Such Jamos can only be formed by combining the two specific Jamos,
                            // aka the double Jamos.
                            HangulMedialIndex VIndexA = (HangulMedialIndex)((SIndex % NCount) / TCount);
                            if (isMedial(VIndexA))
                            {
                                // Search the double medial map if such a combination exists
                                unsigned short key = packDoubleMedial(VIndexA, VIndexB);
                                map<unsigned short, HangulMedialIndex>::iterator it = doubleMedialMap.find(key);
                                if (doubleMedialMap.end() != it)
                                {
                                    // Update syllable by adding the difference between
                                    // the vowels indices
                                    HangulMedialIndex VIndexD = it->second;
                                    int VDiff = (int)VIndexD - (int)VIndexA;
                                    last = lastUnicode + VDiff * TCount;
                                    result.pop_back();
                                    result.push_back(last);
                                    continue;
                                }
                            }
                        }
                    }
                    else
                    {
                        // Check to see if current jamo is vowel
                        int VIndex = (int)unicode - VBase;
                        if (VIndex >= 0 && VIndex < VCount)
                        {
                            // Since some initial and final consonants use the same
                            // Unicode values, we need to check whether the previous final
                            // Jamo is actually an initial Jamo of the next syllable.
                            //
                            // Consider the following scenario:
                            //      LVT+V == not possible
                            //      LV, L+V == possible
                            int LIndex;
                            vector<unsigned short>::iterator it = find(initials.begin(), initials.end(), finals[TIndex]);
                            if (it != initials.end())
                            {
                                LIndex = distance(initials.begin(),it);
                                if (LIndex >= 0 && LIndex < LCount)
                                {
                                    // Remove the previous final jamo from the syllable,
                                    // making the current syllable of form LV
                                    last = (int)lastUnicode - TIndex;
                                    result.pop_back();
                                    result.push_back(last);

                                    // Make new syllable of form LV
                                    last = SBase + (LIndex * VCount + VIndex) * TCount;
                                    result.push_back(last);
                                    continue;
                                }
                            }
                            // Check to see if the current final Jamo is double consonant.
                            // In this scenario, the double consonant is split into parts
                            // and the second part is removed from the current syllable.
                            // Then the second part is joined with the current vowel making
                            // the new syllable of form LV.
                            unsigned short key = findDoubleFinal((HangulFinalIndex)TIndex);
                            if (key)
                            {
                                // Split the consonant into two jamos and remove the
                                // second jamo B from the current syllable
                                HangulFinalIndex TIndexA, TIndexB;
                                unpackDoubleFinal(key, TIndexA, TIndexB);
                                last = (int)lastUnicode - TIndex + (int)TIndexA;
                                result.pop_back();
                                result.push_back(last);

                                // Add new syllable by combining the initial jamo
                                // and the current vowel
                                vector<unsigned short>::iterator it = find(initials.begin(), initials.end(), finals[TIndexB]);
                                if (it != initials.end())
                                {
                                    LIndex = distance(initials.begin(),it);
                                }
                                else
                                {
                                    LIndex = -1;
                                }
                                last = SBase + (LIndex * VCount + VIndex) * TCount;
                                result.push_back(last);
                                continue;
                            }
                        }
                        // Check whether the current consonant can connect to current
                        // consonant forming a double final consonant
                        HangulFinalIndex TIndexA = (HangulFinalIndex)TIndex;
                        if (isFinal(TIndexA))
                        {
                            HangulFinalIndex TIndexB;
                            vector<unsigned short>::iterator it = find(finals.begin(), finals.end(), unicode);
                            if (it != finals.end()) // 找到了
                            {
                                TIndexB = (HangulFinalIndex)distance(finals.begin(),it);
                            }
                            if (isFinal(TIndexB))
                            {
                                // Search the double final map if such a combination exists
                                unsigned short key = packDoubleFinal(TIndexA, TIndexB);
                                map<unsigned short, HangulFinalIndex>::iterator it = doubleFinalMap.find(key);
                                if (doubleFinalMap.end() != it)
                                {
                                    // Update syllable by adding the difference between
                                    // the consonant indices
                                    HangulFinalIndex TIndexD = it->second;
                                    int TDiff = (int)TIndexD - (int)TIndexA;
                                    last = (int)lastUnicode + TDiff;
                                    result.pop_back();
                                    result.push_back(last);
                                    continue;
                                }
                            }
                        }
                    }
                }
                else
                {
                    // The last character is not syllable.
                    // Check to see if the last character is an initial consonant
                    int LIndex = -1;
                    vector<unsigned short>::iterator it = find(initials.begin(), initials.end(), lastUnicode);
                    if (it != initials.end())
                    {
                        LIndex = distance(initials.begin(),it);
                    }
                    if (LIndex != -1) {

                        // If the current character is medial vowel,
                        // make syllable of form LV
                        int VIndex = (int)unicode - VBase;
                        if (VIndex >= 0 && VIndex < VCount)
                        {
                            last = SBase + (LIndex * VCount + VIndex) * TCount;
                            result.pop_back();
                            result.push_back(last);
                            continue;
                        }
                    }
                }
            }
            // Otherwise, add the character into buffer
            last = unicode;
            result.push_back(unicode);
        }
        return result;
    }

}

int isJamo(const unsigned short keycode)
{
    return keycode >= 0x3131 && keycode <= 0x3163;
}


int hangulIME(unsigned short *in, unsigned short *out)
{
    int ret = 0;
    if (isJamo(in[1]))
    {
        list<unsigned short> source;
        list<unsigned short> dst;

        if (in[0] != 0)
            source.push_back(in[0]);
        source.push_back(in[1]);
  
        dst = HANGULIME::compose(source);
        ret = dst.size();
        for (int i = 0; i < ret; ++i)
        {
            out[i] = dst.front();
            dst.pop_front();
        }
    }

    return ret;
}
/*********************************************************************************************************
** End of file
*********************************************************************************************************/
