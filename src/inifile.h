//--------------------------------------------------------------------
//Class:				TIniFile
//Author:				Oleg M. Telegin
//Additional functions:	Konstantin Utkin
//--------------------------------------------------------------------
#ifndef TINIFILE_H
#define TINIFILE_H

#include "ini_local_conf.h"

//--------------------------------------------------------------------
#define INI_REMOVE_CR
#ifdef _LINUX_
#define DONT_HAVE_STRUPR
#endif
//--------------------------------------------------------------------
#define tpNULL       0
#define tpSECTION    1
#define tpKEYVALUE   2
#define tpCOMMENT    3
//--------------------------------------------------------------------
struct entry_t
{
   char Type;
   char* pText;
   struct entry_t* pPrev;
   struct entry_t* pNext;
};
//--------------------------------------------------------------------
typedef struct
{
   struct entry_t* pSec;
   struct entry_t* pKey;
   char KeyText [128];
   char ValText [128];
   char Comment [255];
}EFIND;
//--------------------------------------------------------------------
//+uki
#include <vector>
#include <string>
//-uki
class TIniFile
{
public:
	TIniFile(void);
    ~TIniFile(void);
	bool OpenIniFile(const char* pFileName);
	bool ReadBool(const char* pSection, const char* pKey, bool Default);
	int ReadInt(const char* pSection, const char* pKey, int Default);
	float ReadFloat(const char* pSection, const char* pKey, float Default);
	unsigned short ReadShortHex(const char* pSection, const char* pKey, unsigned short Default);
	unsigned short ReadHex32bit(const char* pSection, const char* pKey, unsigned long Default);
	void WriteShortHex(const char* pSection, const char* pKey, unsigned short Value);
#ifdef USE_BOOST
	int ReadMultipleInt(const char* pSection, const char* pKey, std::vector<int>* dst_values);
	//! strings should be divided by ','
	//! other symbols would be counted as part of name
	int ReadMultipleString(const char* pSection, const char* pKey, std::vector<std::string>* dst_values);
#endif
	const char* ReadString(const char* pSection, const char* pKey, const char* pDefault);
	void WriteBool(const char* pSection, const char* pKey, bool Value);
	void WriteInt(const char* pSection, const char* pKey, int Value);
	void WriteFloat(const char* pSection, const char* pKey, float Value);
	void WriteString(const char* pSection, const char* pKey, const char* pValue);
	void CloseIniFile();
	bool WriteIniFile(const char* pFileName);
	bool DeleteKey(const char* pSection, const char* pKey);
protected:
	struct entry_t* m_pEntry;
	struct entry_t* m_pCurEntry;
	char m_result[255];
	FILE* m_pIniFile;
	void AddKey(struct entry_t* pEntry, const char* pKey, const char* pValue);
	bool AddItem(char Type, const char* pText);
	bool AddItemAt(struct entry_t* pEntryAt, char Mode, const char* pText);
	void FreeMem(void *pPtr);
	void FreeAllMem(void);
	bool FindKey(const char* pSection, const char* pKey, EFIND* pList);
	bool AddSectionAndKey(const char* pSection, const char* pKey, const char* pValue);
	struct entry_t* MakeNewEntry(void);
	struct entry_t* FindSection(const char* pSection);
};
//--------------------------------------------------------------------
#endif
