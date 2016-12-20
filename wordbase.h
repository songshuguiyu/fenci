#ifndef _WORDBASE_H
#define _WORDBASE_H

#include "nodepool.h"
#include <fstream>

//add by freeeyes
//分词算法

class CWordBase
{
public:
	CWordBase();
	~CWordBase(); 
	
	bool Init(const char* pDictFile, int nPoolSize);
	void Close();
	
	int Add_Word(const char* pWord);
	
	int Cut_Word(const char* pSentence, vector<string>& vecWord);
	int Cut(const char* pSentence, vector<string>& vecWord);
	
private:
	ENUM_WORD_TYPE Get_Rune_From_String(const char* pWord, int nBegin, int nLen, _Rune* pRune);
	_RuneLinkNode* Set_HashMap_Word_Tree(_RuneLinkNode* pRuneNode, size_t stRune);
	
	void DisplayTempNodeList(_RuneLinkNode* pRuneNode, int nLayer);
	
	_RuneLinkNode* Find(_Rune* pRune, _RuneLinkNode* pRuneLinkNode);
	
private:
	_RuneLinkNode*            m_pWordRoot;
	CRunePool                 m_objRunePool;
	CNodePool                 m_objNodePool;
	ENUM_WORD_TYPE            m_emType;        //当前字典字符集，(UTF8或者GBK)
};
#endif
