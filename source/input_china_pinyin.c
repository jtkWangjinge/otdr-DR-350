/*
 * input_china_pinyin.c
 *
 *  Created on: 
 *      Author: 
 */

#include "input_china_pinyin.h"
#include "app_global.h"

static INPUTDB *pInpDb = NULL;


static char TableName[26][9] = {
        "pinyin_a",
        "pinyin_b",
        "pinyin_c",
        "pinyin_d",
        "pinyin_e",
        "pinyin_f",
        "pinyin_g",
        "pinyin_h",
        "",
        "pinyin_j",
        "pinyin_k",
        "pinyin_l",
        "pinyin_m",
        "pinyin_n",
        "pinyin_o",
        "pinyin_p",
        "pinyin_q",
        "pinyin_r",
        "pinyin_s",
        "pinyin_t",
        "",
        "",
        "pinyin_w",
        "pinyin_x",
        "pinyin_y",
        "pinyin_z"
};


//
UCS2* Utf8ToUcs2(UTF8 *pUtf8Code)
{
    UCS2 *pUcs2 = NULL;
    char *pUtf8 = (char *) pUtf8Code;
    int iLen;

    iLen = strlen(pUtf8);
    if (iLen > UTF8_VALID_MAXLEN)
    {
        return NULL;    //UTF8 code length invalid
    }

    pUcs2 = malloc(sizeof(UCS2));
    if (NULL == pUcs2)
    {
        return NULL;
    }

    switch (iLen)
    {
    case 3:     //UTF8(UCS2:0x800~0xFFFF):1110xxxx 10xxxxxx 10xxxxxx
        if (0xE0 == (pUtf8Code[0] & 0xF0) &&
            0x80 == (pUtf8Code[1] & 0xC0) &&
            0x80 == (pUtf8Code[2] & 0xC0))
        {
            *pUcs2 = (UCS2) (((pUtf8Code[0] & 0x0F) << 12) |
                             ((pUtf8Code[1] & 0x3F) << 6) |
                              (pUtf8Code[2] & 0x3F));
        }
        else    //UTF8(UCS2:0x800~0xFFFF) format invalid
        {
            free(pUcs2);
            return NULL;
        }
        break;
    case 2:     //UTF8(UCS2:0x80~0x7FF):110xxxxx 10xxxxxx
        if (0xC0 == (pUtf8Code[0] & 0xE0) &&
            0x80 == (pUtf8Code[1] & 0xC0))
        {
            *pUcs2 = (UCS2) (((pUtf8Code[0] & 0x1F) << 6) |
                              (pUtf8Code[1] & 0x3F));
        }
        else    //UTF8(UCS2:0x80~0x7FF) format invalid
        {
            free(pUcs2);
            return NULL;
        }
        break;
    default:    //UTF8(UCS2:0~0x7F):0xxxxxxx
        *pUcs2 = (UCS2) (pUtf8Code[0] & 0x7F);
        break;
    }

    return pUcs2;
}


//
UTF8* Ucs2ToUtf8(UCS2 *pUcs2Code)
{
    return NULL;
}


//
char* StrToLower(char *pConvStr)
{
    char *p = pConvStr;

    while (*p)
    {
        if (*p >= 'A' && *p <= 'Z')
        {
            *p += 'a' - 'A';
        }
        p++;
    }

    return pConvStr;
}


//UCS2:0xFC(ü) to ASCII:0x76(v)
char* TransCharU(char *pPinyinStr)
{
    unsigned char *p = (unsigned char *) pPinyinStr;

    while (*p)
    {
        if (0xFC == *p)
        {
            *p = 'v';
        }
        p++;
    }

    return pPinyinStr;
}


//ASCII:0x76(v) to UCS2:0xFC(ü)
char* TransCharV(char *pPinyinStr)
{
    unsigned char *p = (unsigned char *) pPinyinStr;

    while (*p)
    {
        if ('v' == *p)
        {
            *p = 0xFC;
        }
        p++;
    }

    return pPinyinStr;
}


//
int OpenInputDb(char *pDbName)
{
    int iErr = 0;
    char *pErrMsg = NULL;

    iErr = sqlite3_open(pDbName, &pInpDb);
    if (SQLITE_OK != iErr)
    {
        LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
    }

    return iErr;
}


//
int CloseInputDb(void)
{
    int iErr = 0;
    char *pErrMsg = NULL;

    if (NULL != pInpDb)
    {
        iErr = sqlite3_close(pInpDb);
        if (SQLITE_OK != iErr)
        {
            LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
        }
    }

    return iErr;
}


//
int ShowSingleRecord(void *pInpPara,
                     int iFieldNumber, char **ppFieldValue, char **ppFieldName)
{
    int i, n, t;
    char *p;

    for (i = 0; i < iFieldNumber; i++)
    {
        LOG(LOG_INFO, "FIELD:%s \t>>>> \tVALUE:%s", ppFieldName[i], ppFieldValue[i]);

        if (i == 0)
        {
            printf("(");
            n = strlen(ppFieldValue[i]);
            p = ppFieldValue[i];
            for (t = 0; t < n; t++)
            {
                printf(" 0x%2X", (unsigned char) *p);
                p++;
            }
            printf(" )\n");
        }
        else
            printf("\n");
    }
    printf("----------------------------------------\n");

    return 0;
}


#define SQL_SENTENCE_MAXLEN     80


//
WordList* QueryWordPinyin(char *pWordPinyin, int iViewLen)
{
    int iErr = 0;
    char Pinyin[PINYIN_STORE_MAXLEN];
    char Sql[SQL_SENTENCE_MAXLEN];
    char *pErrMsg = NULL;
    int iCount = 0;
    WordList *pWord = NULL;

    if (strlen(pWordPinyin) > PINYIN_VALID_MAXLEN)
    {
        return NULL;
    }

    strcpy(Pinyin, pWordPinyin);
    StrToLower(Pinyin);
    TransCharU(Pinyin);

    if (NULL != pInpDb)
    {
        if ((Pinyin[0] >= 'a' && Pinyin[0] <= 'z') &&
            (Pinyin[0] != 'i' && Pinyin[0] != 'u' && Pinyin[0] != 'v'))
        {
            sprintf(Sql, "select count(word) from %s where pinyin = '%s'",
                    TableName[Pinyin[0] - 'a'], Pinyin);
            iErr = sqlite3_exec(pInpDb, Sql, GetWordCount, &iCount, &pErrMsg);
            if (SQLITE_OK != iErr)
            {
                LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
            }
            if (!iCount)
            {
                return NULL;
            }

            pWord = malloc(sizeof(WordList));
            if (NULL == pWord)
            {
                return NULL;
            }
            pWord->pWordCode = malloc(iCount * sizeof(UCS2));
            if (NULL == pWord->pWordCode)
            {
                free(pWord);
                return NULL;
            }
            else
            {
                strcpy(pWord->Pinyin, Pinyin);
                pWord->iWordCount = iCount;
                pWord->iAddCount = 0;
                pWord->iViewLen = iViewLen;
                pWord->iHeadPos = 0;
                pWord->iTailPos = iViewLen < iCount ? iViewLen : iCount;
                pWord->iSelOffset = 0;
            }

            sprintf(Sql, "select word from %s where pinyin = '%s'",
                    TableName[Pinyin[0] - 'a'], Pinyin);
            iErr = sqlite3_exec(pInpDb, Sql, GetWordCode, pWord, &pErrMsg);
            if (SQLITE_OK != iErr)
            {
                LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
            }
            if (pWord->iAddCount < pWord->iWordCount)
            {
                LOG(LOG_ERROR, "SQL exec fault: [%s]\n", Sql);
                free(pWord->pWordCode);
                free(pWord);
                return NULL;
            }
        }
    }

    return pWord;
}

//
int GetWordCount(void *pInpPara,
                 int iFieldNumber, char **ppFieldValue, char **ppFieldName)
{
    int iCount;

    iCount = atoi(ppFieldValue[0]);
    *((int *) pInpPara) = iCount;

    return 0;
}


//
int GetWordCode(void *pInpPara,
                int iFieldNumber, char **ppFieldValue, char **ppFieldName)
{
    WordList *pList = pInpPara;
    UTF8 Utf8[UTF8_STORE_MAXLEN];
    UCS2 *pUcs2 = NULL;

    if (NULL == pList)
    {
        LOG(LOG_ERROR, "WordList obj invaild! ['Obj is null']\n");
        return 1;
    }

    if (strlen(ppFieldValue[0]) > UTF8_VALID_MAXLEN)
    {
        LOG(LOG_ERROR, "Word code invalid! ['Word > 3']\n");
        return 2;
    }

    strcpy((char *) Utf8, ppFieldValue[0]);
    pUcs2 = Utf8ToUcs2(Utf8);
    if (NULL != pUcs2)
    {
        if (pList->iAddCount < pList->iWordCount)
        {
            pList->pWordCode[pList->iAddCount] = *pUcs2;
            pList->iAddCount++;
        }
        else
        {
            LOG(LOG_ERROR, "WordList buff is full!\n");
        }
        free(pUcs2);
    }

    return 0;
}


#define IMPORT_LINE_MAXLEN      64
#define IMPORT_WORD_MAXLEN      UTF8_STORE_MAXLEN   //use utf8, only UCS2
#define INPORT_PINYIN_MAXLEN    PINYIN_STORE_MAXLEN

//
int CheckPinyinFile(char *pFileName)
{
    FILE *pFile = NULL;
    char Buff[IMPORT_LINE_MAXLEN];
    char Word[IMPORT_WORD_MAXLEN];
    char Pinyin[INPORT_PINYIN_MAXLEN];
    char *pSrc = NULL;
    char *pDest = NULL;
    int iWord;
    int iPinyin;

    pFile = fopen(pFileName, "rt");
    if (NULL == pFile)
    {
        LOG(LOG_ERROR, "Import file open failed!\n");
        return 1;
    }

    while (!feof(pFile))
    {
        if (NULL == fgets(Buff, IMPORT_LINE_MAXLEN, pFile))
        {
            break;
        }
    
        pSrc = Buff;
        pDest = Word;
        iWord = 0;
        iPinyin = 0;
        while (*pSrc)   //Extract word & pinyin
        {
            switch (*pSrc)
            {
            case '\t':  //import line format: "$word'\t'$pinyin'\n'"
                iWord = 1;
                *pDest = '\0';
                pDest = Pinyin;
                break;
            case '\n':  //import line format: "$word'\t'$pinyin'\n'"
                iPinyin = 1;
                *pDest = '\0';
                break;
            default:
                if (!iWord)
                {
                    if ((pDest - Word) > UTF8_VALID_MAXLEN)
                    {
                        fclose(pFile);
                        LOG(LOG_ERROR, "File invalid! ['Word > 3' or 'Lost \t']\n");
                        return 2;
                    }
                }
                if (!iPinyin)
                {
                    if ((pDest - Pinyin) > PINYIN_VALID_MAXLEN)
                    {
                        fclose(pFile);
                        LOG(LOG_ERROR, "File invalid! ['Pinyin > 6']\n");
                        return 3;
                    }
                }
                *pDest = *pSrc;
                pDest++;
                break;
            }
            pSrc++;
        }
        if (!iWord || !iPinyin)
        {
            if (!feof(pFile))
            {
                fclose(pFile);
                LOG(LOG_ERROR, "File invalid! [Blank line]\n");
                return 4;
            }
        }
    }

    fclose(pFile);

    return 0;
}


//
int ImportPinyinFile(char *pFileName)
{
    FILE *pFile = NULL;
    char Buff[IMPORT_LINE_MAXLEN];
    char Word[IMPORT_WORD_MAXLEN];
    char Pinyin[INPORT_PINYIN_MAXLEN];
    char *pSrc = NULL;
    char *pDest = NULL;
    int iWord;
    int iPinyin;

    if (CheckPinyinFile(pFileName))
    {
        return 1;
    }

    pFile = fopen(pFileName, "rt");
    if (NULL == pFile)
    {
        LOG(LOG_ERROR, "Import file open failed!\n");
        return 2;
    }

    ClearWordPinyin();  //Clear all older record

    while (!feof(pFile))
    {
        if (NULL == fgets(Buff, IMPORT_LINE_MAXLEN, pFile))
        {
            break;
        }

        pSrc = Buff;
        pDest = Word;
        iWord = 0;
        iPinyin = 0;
        while (*pSrc)   //Extract word & pinyin
        {
            switch (*pSrc)
            {
            case '\t':  //import line format: "$word'\t'$pinyin'\n'"
                iWord = 1;
                *pDest = '\0';
                pDest = Pinyin;
                break;
            case '\n':  //import line format: "$word'\t'$pinyin'\n'"
                iPinyin = 1;
                *pDest = '\0';
                break;
            default:
                *pDest = *pSrc;
                pDest++;
                break;
            }
            pSrc++;
        }
        if (iWord && iPinyin)
        {
            StrToLower(Pinyin);
            InsertWordPinyin(Word, Pinyin); //Execute insert sentence
        }
    }
    fclose(pFile);

    return 0;
}


//
int ClearWordPinyin(void)
{
    int iErr = 0;
    int i;
    char Sql[SQL_SENTENCE_MAXLEN];
    char *pErrMsg = NULL;

    for (i = 0; i < 26; i++)    //'a' ~ 'z'
    {
        if (strlen(TableName[i]))
        {
            sprintf(Sql, "delete from %s", TableName[i]);
            LOG(LOG_INFO, "SQL:[ %s ]\n", Sql);

            iErr = sqlite3_exec(pInpDb, Sql, NULL, NULL, &pErrMsg);
            if (SQLITE_OK != iErr)
            {
                LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
            }
        }
    }

    return iErr;
}


//
int InsertWordPinyin(char *pWordVal, char *pPinyinVal)
{
    int iErr = 0;
    int iTable = 0;
    char Sql[SQL_SENTENCE_MAXLEN];
    char *pErrMsg = NULL;

    if (!iErr)
    {
        if (NULL == pInpDb)
        {
            iErr = 1;
        }
        if (NULL == pWordVal || NULL == pPinyinVal)
        {
            iErr = 1;
        }
    }

    if (!iErr)
    {
        iTable = (int) (*pPinyinVal - 'a');
        if (strlen(TableName[iTable]))  //skip 'i*' 'v*' 'u*'
        {
            sprintf(Sql, "insert into %s values('%s', '%s')",
                    TableName[iTable], pWordVal, pPinyinVal);
            LOG(LOG_INFO, "SQL:[ %s ]\n", Sql);

            iErr = sqlite3_exec(pInpDb, Sql, NULL, NULL, &pErrMsg);
            if (SQLITE_OK != iErr)
            {
                LOG(LOG_ERROR, "*** SQLite3 ERR%d: [%s]\n", iErr, pErrMsg);
            }
        }
    }

    return iErr;
}

