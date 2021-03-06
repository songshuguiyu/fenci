#include "TextRank.h"

CTextRank::CTextRank()
{
}

CTextRank::~CTextRank()
{
}

void CTextRank::Filter_Word(vector<_Word_Param>& vecWordList, vector<_Word_Param>& objtagList)
{
	//过滤当前词语中所有的名词和形容词，忽略其他的词
	objtagList.clear();
	for(int i = 0;i < (int)vecWordList.size(); i++)
	{
		//过滤单字
		if(vecWordList[i].m_sWordSize <= 1)
		{
			continue;
		}		
		
		if(strcmp(vecWordList[i].m_szWordSpeech, "n") == 0 ||
			 strcmp(vecWordList[i].m_szWordSpeech, "ns") == 0|| 
			 strcmp(vecWordList[i].m_szWordSpeech, "v") == 0)
		{
			objtagList.push_back(vecWordList[i]);
		}
	}
}

bool CTextRank::Cut_Dict(vector<_Word_Param>& objvecWord)
{
	vector<_Word_Param> objtagList;
	//过滤当前句子中的词语
	Filter_Word(objvecWord, objtagList);
	
	//分析一个句子中包含的所有共生词
	for(int i = 0; i < objtagList.size(); i++)
	{
		for(int j = i + 1; j < objtagList.size(); j++)
		{
			if(objtagList[j].m_sWordSize <= 1)
			{
				continue;
			}
			
			//将共生词记录在字典里
			string strkey = (string)objtagList[i].m_szWord + (string)objtagList[j].m_szWord;
			mapDictInfo::iterator f = m_mapDict.find(strkey);
			if(f != m_mapDict.end())
			{
				//在当前字典中
				_Dict_Info& obj_Dict_Info = (_Dict_Info& )f->second;
				obj_Dict_Info.nWeigth++;
			}
			else
			{
				//不在当前字典中，添加新共生词
				_Dict_Info obj_Dict_Info;
				sprintf(obj_Dict_Info.szBeginWord, "%s", objtagList[i].m_szWord);
				sprintf(obj_Dict_Info.szEndWord, "%s", objtagList[j].m_szWord);
				obj_Dict_Info.nWeigth = 1;
				m_mapDict.insert(mapDictInfo::value_type(strkey, obj_Dict_Info));
			}
		}
	}
	
	return true;
}

bool CTextRank::Rank(vector<_Word_Param> objvecWord, vector<_Word>& vecWordList, int nTopCount)
{
	int nPos = 0;
	int nSentenceID = 1;
	
	m_mapDict.clear();
	vecWordList.clear();
	
	vector<_Word_Param> objvecSent;
	objvecSent.clear();
	
	//生成共生词词典
	for(int i = 0; i < (int)objvecWord.size(); i++)
	{
		/*
		printf("[CTextRank::Rank](%s)m_nSentenceID=%d,nSentenceID=%d.\n", 
			objvecWord[i].m_szWord,
			objvecWord[i].m_nSentenceID,
			nSentenceID);
		*/
		if(objvecWord[i].m_nSentenceID == nSentenceID)
		{
			objvecSent.push_back(objvecWord[i]);
		}
		else
		{
			Cut_Dict(objvecSent);
			objvecSent.clear();
			nSentenceID++;
			i--;
		}
	}
	//最后一句
	Cut_Dict(objvecSent);
	
	//矩阵添加所有的边
	for(mapDictInfo::iterator b = m_mapDict.begin(); b != m_mapDict.end(); b++)
	{
		_Dict_Info& obj_Dict_Info = (_Dict_Info& )b->second;
		/*
		printf("[CTextRank::Rank]%s --> %s (%d).\n", 
						obj_Dict_Info.szBeginWord, 
						obj_Dict_Info.szEndWord,
						obj_Dict_Info.nWeigth);
		*/
		m_WordGraph.addEdge((string)obj_Dict_Info.szBeginWord, 
												(string)obj_Dict_Info.szEndWord,
												obj_Dict_Info.nWeigth);
	}
	
	//计算共生词矩阵
	WordMap objWordMap;
	
	//计算摘要
	m_WordGraph.rank(objWordMap);
	
	for(WordMap::iterator e = objWordMap.begin(); e != objWordMap.end(); e++)
	{
		//_Word& obj_Word = (_Word& )e->second;
		//printf("[CTextRank::Rank]<%s == %f>.\n", obj_Word.word.c_str(), obj_Word.weight);
		vecWordList.push_back(e->second);
	}
	
	//排序
	std::sort(vecWordList.begin(), vecWordList.end(), SortByWord);
	vecWordList.resize(nTopCount);

	return true;
}
