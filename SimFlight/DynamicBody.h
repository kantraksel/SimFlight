#pragma once
#include <list>
#include "NodeDef.h"

class DynamicBody
{
	private:
		std::list<ND::NodeGeneric> nodeList;

	public:
		DynamicBody();
		~DynamicBody();

		bool SetNode(unsigned int id, ND::GenericValue value);
		bool SetNode(unsigned int id, const char* pValue, unsigned int valueSize);
		void RemoveNode(unsigned int id);

		int WriteBody(unsigned char* pBuff, int lenght);
		void Clear();
};
