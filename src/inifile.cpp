//--------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "inifile.h"

#ifdef USE_BOOST
#include "regex.hpp"
#include "lexical_cast.hpp"
#endif
//--------------------------------------------------------------------
#ifdef DONT_HAVE_STRUPR
void strupr(char *str)
{
    while (*str != 0)
    {
        if (islower(*str ))
	    *str = toupper(*str);
        str++;
    }
}
#endif
//--------------------------------------------------------------------
TIniFile::TIniFile (void)
{
    m_pEntry = NULL;
    m_pCurEntry = NULL;
    m_result[0] = 0;
    m_pIniFile = NULL;
}
//--------------------------------------------------------------------
TIniFile::~TIniFile(void)
{
    FreeAllMem ();
}
//--------------------------------------------------------------------
bool TIniFile::OpenIniFile(const char* FileName)
{
    char Str[255];
    char* pStr;
    struct entry_t* pEntry;

    FreeAllMem();

    if (FileName == NULL)
	return false;
    if ((m_pIniFile = fopen(FileName, "r")) == NULL) { 
	printf("OpenIniFile %s fails with %d\n",FileName,errno);
	return false;
    }

    while (fgets(Str, 255, m_pIniFile) != NULL)
    {
	pStr = strchr(Str, '\n');
	if (pStr != NULL) 
	    *pStr = 0;
	pEntry = MakeNewEntry();
	if (pEntry == NULL) 
	    return false;
#ifdef INI_REMOVE_CR
	int Len = strlen(Str);
	if (Len > 0)
	    if (Str[Len-1] == '\r')
		Str[Len-1] = '\0';
#endif
	pEntry->pText = (char *)malloc(strlen(Str)+1);
	if (pEntry->pText == NULL)
	{
	    FreeAllMem();
	return false;
	}
	strcpy(pEntry->pText, Str);
	pStr = strchr(Str,';');
	if (pStr != NULL) 
	    *pStr = 0;
	if ((strstr(Str, "[") > 0) && (strstr(Str, "]") > 0))
	    pEntry->Type = tpSECTION;
	else
	{
	    if (strstr (Str, "=") > 0)
		pEntry->Type = tpKEYVALUE;
	    else
		pEntry->Type = tpCOMMENT;
	}
	m_pCurEntry = pEntry;
    }
    fclose (m_pIniFile);
    m_pIniFile = NULL;
    return true;
}
//--------------------------------------------------------------------
void TIniFile::CloseIniFile(void)
{
    FreeAllMem();
    if (m_pIniFile != NULL)
    {
	fclose(m_pIniFile);
	m_pIniFile = NULL;
    }
}
//--------------------------------------------------------------------
bool TIniFile::WriteIniFile (const char* pFileName)
{
    struct entry_t* pEntry = m_pEntry;
    if (m_pIniFile != NULL)
	fclose(m_pIniFile);
    if ((m_pIniFile = fopen(pFileName, "wb")) == NULL)
    {
    	FreeAllMem ();
	return false;
    }
    while (pEntry != NULL)
    {
	if (pEntry->Type != tpNULL)
	{
#ifdef INI_REMOVE_CR
	    fprintf(m_pIniFile, "%s\n", pEntry->pText);
#else
	    fprintf(m_pIniFile, "%s\r\n", pEntry->pText);
#endif
	}
	pEntry = pEntry->pNext;
    }
    fclose(m_pIniFile);
    m_pIniFile = NULL;
    return true;
}
//--------------------------------------------------------------------
void TIniFile::WriteString(const char* pSection, const char* pKey, const char* pValue)
{
    EFIND List;
    char Str[255];
    memset(&Str[0],0,255);
    
    if (((pSection != NULL) && (pKey != NULL) && (pValue != NULL)) == false)
	return;
    if (FindKey(pSection, pKey, &List) == true)
    {
	sprintf(Str, "%s=%s%s", List.KeyText, pValue, List.Comment);
	FreeMem(List.pKey->pText);
	List.pKey->pText = (char *)malloc(strlen (Str)+1);
	strcpy(List.pKey->pText, Str);
    }
    else
    {
	if ((List.pSec != NULL) && (List.pKey == NULL))
	    AddKey(List.pSec, pKey, pValue);
	else
	    AddSectionAndKey(pSection, pKey, pValue);
    }
}
//--------------------------------------------------------------------
void TIniFile::WriteBool(const char* pSection, const char* pKey, bool Value)
{
    char Val[2] = {'0',0};
    if (Value != 0) 
        Val [0] = '1';
    WriteString(pSection, pKey, Val);
}
//--------------------------------------------------------------------
void TIniFile::WriteInt(const char* pSection, const char* pKey, int Value)
{
    char Val[12];
    sprintf(Val, "%d", Value);
    WriteString(pSection, pKey, Val);
}
//--------------------------------------------------------------------
void TIniFile::WriteFloat(const char* pSection, const char* pKey, float Value)
{
    char Val[32];
    sprintf(Val, "%1.10lE", Value);
    WriteString(pSection, pKey, Val);
}
//--------------------------------------------------------------------
const char* TIniFile::ReadString(const char* pSection, const char* pKey, const char* pDefault)
{
    EFIND List;
    if (((pSection != NULL) && (pKey != NULL) && (pDefault != NULL)) == false)
	return pDefault;
    if (FindKey(pSection, pKey, &List) == true)
    {
	strcpy(m_result, List.ValText);
	return m_result;
    }
    return pDefault;
}
//--------------------------------------------------------------------
bool TIniFile::ReadBool(const char* pSection, const char* pKey, bool Default)
{
    char Val[2] = {"0"};
    if (Default != 0)
	Val [0] = '1';
    return (atoi(ReadString(pSection, pKey, Val))?1:0);
}
//--------------------------------------------------------------------
int TIniFile::ReadInt(const char* pSection, const char* pKey, int Default)
{
    char Val[12];
    sprintf(Val,"%d", Default);
    return (atoi(ReadString(pSection, pKey, Val)));
}
//--------------------------------------------------------------------
float TIniFile::ReadFloat(const char* pSection, const char* pKey, float Default)
{
    double Val;
    sprintf(m_result, "%1.10lE", Default);
    sscanf(ReadString(pSection, pKey, m_result), "%lE", &Val);
    return Val;
}
//--------------------------------------------------------------------

unsigned short TIniFile::ReadShortHex(const char* pSection, const char* pKey, unsigned short Default)
{
    unsigned long Val;
    sprintf(m_result,"%04x",Default);
    sscanf(ReadString(pSection, pKey, m_result),"%04x",&Val);
    return Val;
}
unsigned short TIniFile::ReadHex32bit(const char* pSection, const char* pKey, unsigned long Default)
{
    unsigned long Val;
    sprintf(m_result,"%08x",Default);
    sscanf(ReadString(pSection, pKey, m_result),"%08x",&Val);
    return Val;
}
void TIniFile::WriteShortHex(const char* pSection, const char* pKey, unsigned short Value)
{
    char Val[5];
    memset(&Val[0],0,5);
    sprintf(Val, "%04x", Value);
    WriteString(pSection, pKey, Val);
}

#ifdef USE_BOOST
int TIniFile::ReadMultipleInt(const char* pSection, const char* pKey, std::vector<int>* dst_values) {
	if (dst_values == NULL) return -EINVAL;

	std::string source = ReadString(pSection,pKey,"");
	if (source.size() < 1) return 0;

    boost::regex expression("(\\d+)");

    std::string::const_iterator start, end;
    start = source.begin();
    end = source.end();

    boost::smatch what;

    boost::match_flag_type match_flags = boost::match_default;
    while(boost::regex_search(start, end, what, expression, match_flags))
    {
    	//printf("result %s\n",what.str().c_str());
    	dst_values->push_back(boost::lexical_cast<int>(std::string(what[1])));
    	start = what[1].second;
    }

	return 0;
}

int TIniFile::ReadMultipleString(const char* pSection, const char* pKey, std::vector<std::string>* dst_values) {
	if (dst_values == NULL) return -EINVAL;

	std::string source = ReadString(pSection,pKey,"");
	if (source.size() < 1) return 0;

    boost::regex expression("([^,]+)");

    std::string::const_iterator start, end;
    start = source.begin();
    end = source.end();

    boost::smatch what;

    boost::match_flag_type match_flags = boost::match_default;
    while(boost::regex_search(start, end, what, expression, match_flags))
    {
    	dst_values->push_back(std::string(what[1]));
    	start = what[1].second;
    }

	return 0;
}
#endif

bool TIniFile::DeleteKey(const char* pSection, const char* pKey)
{
    EFIND List;
    struct entry_t* pPrev;
    struct entry_t* pNext;
    if (FindKey(pSection, pKey, &List) == true)
    {
	pPrev = List.pKey->pPrev;
	pNext = List.pKey->pNext;
	if (pPrev)
	    pPrev->pNext=pNext;
	if (pNext)
	    pNext->pPrev=pPrev;
	FreeMem (List.pKey->pText);
	FreeMem (List.pKey);
	return true;
    }
    return false;
}
//--------------------------------------------------------------------
void TIniFile::FreeMem(void* pPtr)
{
    if (pPtr != NULL)
	free (pPtr);
}
//--------------------------------------------------------------------
void TIniFile::FreeAllMem(void)
{
    struct entry_t* pEntry;
    struct entry_t* pNextEntry;
    pEntry = m_pEntry;
    while (1)
    {
	if (pEntry == NULL)
	    break;
	pNextEntry = pEntry->pNext;
	FreeMem(pEntry->pText);
	FreeMem(pEntry);
	pEntry = pNextEntry;
    }
    m_pEntry = NULL;
    m_pCurEntry = NULL;
}
//--------------------------------------------------------------------
struct entry_t* TIniFile::FindSection(const char* pSection)
{
    char Sec[130];
    char iSec[130];
    struct entry_t* pEntry;
    sprintf(Sec, "[%s]", pSection);
    strupr(Sec);
    pEntry = m_pEntry;
    while (pEntry != NULL)
    {
	if (pEntry->Type == tpSECTION)
	{
	    strcpy(iSec, pEntry->pText);
	    strupr(iSec);
	    if (strcmp(Sec, iSec) == 0)
		return pEntry;
	}
	pEntry = pEntry->pNext;
    }
    return NULL;
}
//--------------------------------------------------------------------
bool TIniFile::FindKey(const char* pSection, const char* pKey, EFIND* pList)
{
    char Search[130];
    char Found[130];
    char Text[255];
    char *pText;
    struct entry_t* pEntry;
    pList->pSec = NULL;
    pList->pKey = NULL;
    pEntry = FindSection(pSection);
    if (pEntry == NULL)
	return false;
    pList->pSec = pEntry;
    pList->KeyText[0] = 0;
    pList->ValText[0] = 0;
    pList->Comment[0] = 0;
    pEntry = pEntry->pNext;
    if (pEntry == NULL)
	return false;
    sprintf(Search, "%s",pKey);
    strupr(Search);
    while (pEntry != NULL)
    {
	if ((pEntry->Type == tpSECTION) || (pEntry->Type == tpNULL))
	    return false;
	if (pEntry->Type == tpKEYVALUE)
	{
	    strcpy(Text, pEntry->pText);
	    pText = strchr(Text, ';');
	    if (pText != NULL)
	    {
		strcpy(pList->Comment, pText);
		*pText = 0;
	    }
	    pText = strchr(Text, '=');
	    if (pText != NULL)
	    {
		*pText = 0;
		strcpy(pList->KeyText, Text);
		strcpy(Found, Text);
		*pText = '=';
		strupr(Found);
		if (strcmp(Found,Search) == 0)
		{
		    strcpy (pList->ValText, pText+1);
		    pList->pKey = pEntry;
		    return true;
		}
	    }
	}
	pEntry = pEntry->pNext;
    }
    return false;
}
//--------------------------------------------------------------------
bool TIniFile::AddItem(char Type, const char* pText)
{
    struct entry_t* pEntry = MakeNewEntry();
    if (pEntry == NULL)
	return false;
    pEntry->Type = Type;
    pEntry->pText = (char*)malloc(strlen(pText) +1);
    if (pEntry->pText == NULL)
    {
	free(pEntry);
	return false;
    }
    strcpy(pEntry->pText, pText);
    pEntry->pNext = NULL;
    if (m_pCurEntry != NULL)
	m_pCurEntry->pNext = pEntry;
    m_pCurEntry = pEntry;
    return true;
}
//--------------------------------------------------------------------
bool TIniFile::AddItemAt(struct entry_t* pEntryAt, char Mode, const char* pText)
{
    struct entry_t* pNewEntry;
    if (pEntryAt == NULL)
	return false;
    pNewEntry = (struct entry_t*)malloc(sizeof(entry_t));
    if (pNewEntry == NULL)
	return false;
    pNewEntry->pText = (char*)malloc(strlen(pText)+1);
    if (pNewEntry->pText == NULL)
    {
	free(pNewEntry);
	return false;
    }
    strcpy(pNewEntry->pText, pText);
    if (pEntryAt->pNext == NULL)
    {
	pEntryAt->pNext = pNewEntry;
	pNewEntry->pNext = NULL;
    }
    else
    {
	pNewEntry->pNext = pEntryAt->pNext;
	pEntryAt->pNext = pNewEntry;
    }
    pNewEntry->pPrev = pEntryAt;
    pNewEntry->Type = Mode;
    return true;
}
//--------------------------------------------------------------------
bool TIniFile::AddSectionAndKey(const char* pSection, const char* pKey, const char* pValue)
{
    char Text[255];
    sprintf(Text, "[%s]", pSection);
    if (AddItem(tpSECTION, Text) == false) 
	return false;
    sprintf(Text, "%s=%s", pKey, pValue);
    return AddItem(tpKEYVALUE, Text)?1:0;
}
//--------------------------------------------------------------------
void TIniFile::AddKey(struct entry_t* pSecEntry, const char* pKey, const char* pValue)
{
    char Text[255];
    sprintf(Text, "%s=%s", pKey, pValue);
    AddItemAt(pSecEntry, tpKEYVALUE, Text);
}
//--------------------------------------------------------------------
struct entry_t* TIniFile::MakeNewEntry(void)
{
    struct entry_t* pEntry;
    pEntry = (struct entry_t*)malloc(sizeof(entry_t));
    if (pEntry == NULL)
    {
	FreeAllMem();
	return NULL;
    }
    if (m_pEntry == NULL)
	m_pEntry = pEntry;
    pEntry->Type  = tpNULL;
    pEntry->pPrev = m_pCurEntry;
    pEntry->pNext = NULL;
    pEntry->pText = NULL;
    if (m_pCurEntry != NULL)
	m_pCurEntry->pNext = pEntry;
    return pEntry;
}
//--------------------------------------------------------------------
