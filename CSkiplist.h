#pragma once
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <string>
#include "StreamBuf.h"
#include <assert.h>

std::mutex mtx;
std::string delimiter(":");

// Node
template<typename K, typename V>
class CNode {
public:
	CNode() {}
	CNode(K, V, int);
	~CNode();

	K GetKey();
	V GetValue();
	void SetValue(V);
	CNode<K, V>** m_pForward;
	int m_iNodeLevel;
private:
	K m_Key;
	V m_Value;
};

template<typename K, typename V>
CNode<K, V>::CNode(K _k, V _v, int _lel) {
	m_Key = _k;
	m_Value = _v;
	m_iNodeLevel = _lel;

	m_pForward = new CNode<K, V>*[m_iNodeLevel + 1];
	memset(m_pForward, 0, sizeof(CNode<K, V>*) * (m_iNodeLevel + 1));
}

template<typename K, typename V>
CNode<K, V>::~CNode() {
	delete[] m_pForward;
}

template<typename K, typename V>
V CNode<K, V>::GetValue() {
	return m_Value;
}

template<typename K, typename V>
K CNode<K, V>::GetKey() {
	return m_Key;
}

template<typename K, typename V>
void CNode<K, V>::SetValue(V _v) {
	m_Value = _v;
}

// skiplist

template<typename K, typename V>
class CSkipList {
public:
	CSkipList(int, const char*);
	~CSkipList();
	int GetRandomLevel();
	CNode<K, V>* CreatNode(K, V, int);
	int InsertNode(K, V);
	void DisplayList();
	bool SearchNode(K);
	void DeleteNode(K);
	void DumpFile();
	void LoadFile();
	int Size();

private:
	void GetKVFromString(const std::string&, std::string*, std::string*);
	bool IsValidString(const std::string&);

private:
	int m_iMaxLevel;
	int m_iCurLevel;
	CNode<K, V>* m_pHeader;
	std::fstream m_sFileStream;
	int m_iNodeCnt;
	CStreamBuf* m_cBuf;
	const char* m_cFilePath;
};

template<typename K, typename V>
CSkipList<K, V>::CSkipList(int _maxLevel, const char* _path) {
	m_iMaxLevel = _maxLevel;
	m_iCurLevel = 0;
	m_iNodeCnt = 0;
	K* k = new K;
	V* v = new V;
	m_pHeader = new CNode<K, V>(*k, *v, m_iMaxLevel);
	m_cBuf = new CStreamBuf();
	m_cFilePath = _path;
}

template<typename K, typename V>
CSkipList<K, V>::~CSkipList() {
	if (m_sFileStream.is_open()) m_sFileStream.close();
	delete m_pHeader;
}

template<typename K, typename V>
int CSkipList<K, V>::GetRandomLevel() {
	int k = 1;
	while (rand() % 2) k++;
	k = (k < m_iMaxLevel) ? k : m_iMaxLevel;
	return k;
};

template<typename K, typename V>
void CSkipList<K, V>::GetKVFromString(const std::string& _str, std::string* _key, std::string* _value) {

	if (!IsValidString(_str)) return;
	*_key = _str.substr(0, _str.find(delimiter));
	*_value = _str.substr(_str.find(delimiter) + 1, _str.length());
}

template<typename K, typename V>
bool CSkipList<K, V>::IsValidString(const std::string& _str) {

	if (_str.empty()) return false;
	if (_str.find(delimiter) == std::string::npos) return false;
	return true;
}

template<typename K, typename V>
CNode<K, V>* CSkipList<K, V>::CreatNode(const K _k, const V _v, int _level) {
	CNode<K, V>* node = new CNode<K, V>(_k, _v, _level);
	return node;
}

template<typename K, typename V>
int CSkipList<K, V>::Size() {
	return m_iNodeCnt;
}

template<typename K, typename V>
void CSkipList<K, V>::DisplayList() {
	std::cout << "\n*****Skip List*****" << "\n";
	for (int i = 0; i <= m_iCurLevel; ++i) {
		CNode<K, V>* node = m_pHeader->m_pForward[i];
		std::cout << "Level " << i << ": ";
		while (node != nullptr) {
			std::cout << node->GetKey() << ":" << node->GetValue() << ";";
			node = node->m_pForward[i];
		}
		std::cout << std::endl;
	}
}

template<typename K, typename V>
void CSkipList<K, V>::DumpFile() {
	std::cout << "dump_file-----------------" << std::endl;
	m_sFileStream.open(m_cFilePath, std::ofstream::app | std::ios::binary | std::ofstream::out);
	if (!m_sFileStream)
		fprintf(stderr, "can't open file\n");

	CNode<K, V>* node = m_pHeader->m_pForward[0];

	while (node != nullptr) {
		// ��key��value���л��洢
		(*m_cBuf) << node->GetKey() << node->GetValue();
		std::cout << node->GetKey() << ":" << node->GetValue() << ";\n";
		node = node->m_pForward[0];
	}
	for (int i = 0; i < m_cBuf->size(); ++i) {
		m_sFileStream.write(&(*m_cBuf)[i], sizeof(char));
		m_cBuf->Offset(sizeof(char));
	}
	m_cBuf->Reset();
	m_sFileStream.close();
	m_sFileStream.clear();
	return;
}

template<typename K, typename V>
void CSkipList<K, V>::LoadFile() {
	std::cout << "load_file-----------------" << std::endl;
	m_sFileStream.open(m_cFilePath, std::ifstream::in | std::ios::binary);
	if (!m_sFileStream)
		fprintf(stderr, "can't open file\n");
	char* tmp = new char[1];
	while (m_sFileStream.read(tmp, sizeof(char)))
		m_cBuf->Input(tmp, sizeof(char));

	while (!m_cBuf->IsEOF()) {
		K k; V v;
		(*m_cBuf) >> k >> v;
		InsertNode(k, v);
	}
	m_sFileStream.close();
}
/*************************************��ɾ���*****************************************/
// ����ֵ ����ɹ�    0
//        �Ѿ�����    1
template<typename K, typename V>
int CSkipList<K, V>::InsertNode(K _k, V _v) {
	mtx.lock();
	CNode<K, V>* CurNode = m_pHeader;

	CNode<K, V>** updateNodes = new CNode<K, V>*[m_iMaxLevel + 1];
	memset(updateNodes, 0, sizeof(CNode<K, V>*) * (m_iMaxLevel + 1));

	// ���ϲ������ߣ�ÿ�ζ��ҵ�С�ڵ�ǰֵ�����ֵ��Ȼ�������ߣ�ÿһ�㶼��Ҫ��������ڵ�
	for (int i = m_iCurLevel; i >= 0; --i) {
		while (CurNode->m_pForward[i] != nullptr && CurNode->m_pForward[i]->GetKey() < _k)
			CurNode = CurNode->m_pForward[i];
		updateNodes[i] = CurNode;
	}

	// ��CurNode�ƶ���������λ��
	CurNode = CurNode->m_pForward[0];

	// �����ǰ�ڵ���ڲ���ڵ��key
	if (CurNode && CurNode->GetKey() == _k) {
		mtx.unlock();
		return 1;
	}

	// ���CurNode��nullptr����ô˵���ߵ������ݵ�ĩβ����Ҫ�����½ڵ�
	// ���CurNode��ֵ������_k��˵�����˽ڵ���뵽update[0]��CueNode֮��
	if (CurNode == nullptr || CurNode->GetKey() != _k) {
		int randomLevel = GetRandomLevel();
		if (randomLevel > m_iCurLevel) {
			for (int i = m_iCurLevel + 1; i <= randomLevel; ++i) 
				updateNodes[i] = m_pHeader;
			m_iCurLevel = randomLevel;
		}
		CNode<K, V>* insertNode = CreatNode(_k, _v, m_iCurLevel);
		for (int i = 0; i <= m_iCurLevel; ++i) {
			insertNode->m_pForward[i] = updateNodes[i]->m_pForward[i];
			updateNodes[i]->m_pForward[i] = insertNode;
		}
		m_iNodeCnt++;
	}
	mtx.unlock();
	return 0;
}


template<typename K, typename V>
void CSkipList<K, V>::DeleteNode(K _key) {

	mtx.lock();
	CNode<K, V>* curNode = m_pHeader;
	CNode<K, V>** update = new CNode<K, V>*[m_iMaxLevel + 1];
	memset(update, 0, sizeof(CNode<K, V>*) * (m_iMaxLevel + 1));

	for (int i = m_iCurLevel; i >= 0; --i) {
		while (curNode->m_pForward[i] != nullptr && curNode->m_pForward[i]->GetKey() < _key)
			curNode = curNode->m_pForward[i];
		update[i] = curNode;
	}

	curNode = curNode->m_pForward[0];
	if (curNode != nullptr && curNode->GetKey() == _key) {
		for (int i = 0; i <= m_iCurLevel; ++i) {
			if (update[i]->m_pForward[i] != curNode) 
				break;
			update[i]->m_pForward[i] = curNode->m_pForward[i];
		}
		while (m_iCurLevel > 0 && m_pHeader->m_pForward[m_iCurLevel] == 0) 
			m_iCurLevel--;
		std::cout << "Successfully deleted key " << _key << std::endl;
		m_iNodeCnt--;
	}
	mtx.unlock();
	return;
}

template<typename K, typename V>
bool CSkipList<K, V>::SearchNode(K _key) {

	std::cout << "search_element-----------------" << std::endl;
	CNode<K, V>* curNode = m_pHeader;

	for (int i = m_iCurLevel; i >= 0; --i)
		while (curNode->m_pForward[i] && curNode->m_pForward[i]->GetKey() < _key)
			curNode = curNode->m_pForward[i];

	//reached level 0 and advance pointer to right node, which we search
	curNode = curNode->m_pForward[0];

	// if current node have key equal to searched key, we get it
	if (curNode and curNode->GetKey() == _key) {
		std::cout << "Found key: " << _key << ", value: " << curNode->GetValue() << std::endl;
		return true;
	}
	std::cout << "Not Found Key:" << _key << std::endl;
	return false;
}
