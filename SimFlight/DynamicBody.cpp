#include "DynamicBody.h"
#include "FlightGear/tiny_xdr.hxx"
#include "FGData.h"

using namespace ND;

DynamicBody::DynamicBody()
{
	NodeGeneric emptyNode;
	
	for (auto& prop : FGData::Properties)
	{
		emptyNode.id = prop.id;
		emptyNode.valueType = prop.type;
		emptyNode.value = prop.defaultValue;
		nodeList.push_back(emptyNode);
	}
}

DynamicBody::~DynamicBody()
{
	Clear();
}

bool DynamicBody::SetNode(unsigned int id, GenericValue value)
{
	for (auto& node : nodeList)
	{
		if (node.id == id)
		{
			if (node.valueType == ValueType::VT_String)
				return false;

			node.value = value;
			return true;
		}
	}

	//NOTE: do not allow to add unknown values atm
	return false;

	NodeGeneric node;
	node.id = id;
	node.value = value;

	nodeList.push_back(node);
	return true;
}

bool DynamicBody::SetNode(unsigned int id, const char* pValue, unsigned int valueSize)
{
	for (auto& node : nodeList)
	{
		if (node.id == id)
		{
			if (node.valueType != ValueType::VT_String)
				return false;

			if (valueSize > node.pValue2Capacity)
			{
				if (node.pValue2)
					delete[] node.pValue2;

				node.pValue2 = new unsigned char[valueSize];
				node.pValue2Capacity = valueSize;
			}
			
			memcpy(node.pValue2, pValue, valueSize);
			node.value = (int)valueSize;

			return true;
		}
	}
	
	//NOTE: do not allow to add unknown values atm
	return false;

	NodeGeneric node;
	node.id = id;

	if (valueSize)
		node.pValue2 = new unsigned char[valueSize];
	node.pValue2Capacity = valueSize;

	memcpy(node.pValue2, pValue, valueSize);
	node.value = (int)valueSize;
	
	nodeList.push_back(node);
	return true;
}

void DynamicBody::RemoveNode(unsigned int id)
{
	for (auto i = nodeList.begin(); i != nodeList.end(); ++i)
	{
		if (i->id == id)
		{
			delete[] i->pValue2;
			nodeList.erase(i, i);
			break;
		}
	}
}

struct NodeConst
{
	xdr_data_t id;
	xdr_data_t value;

	NodeConst(NodeGeneric& node)
	{
		id = XDR_encode_uint32(node.id);
		value = XDR_encode_uint32(node.value);
	}
};

int DynamicBody::WriteBody(unsigned char* pBuff, int lenght)
{
	int pointer = 0;

	for (auto& node : nodeList)
	{
		if (pointer + sizeof(NodeConst) > lenght)
			return -1;

		auto& header = *((NodeConst*)(pBuff + pointer));
		header = node;
		pointer += sizeof(NodeConst);

		if (node.valueType == ValueType::VT_String && node.value.uintVal > 0)
		{
			//NOTE: encode string
			unsigned int size = node.value.uintVal * sizeof(xdr_data_t);
			if (pointer + size > lenght)
				return -1;

			auto* pTable = (xdr_data_t*)(pBuff + pointer);
			for (int i = 0; i < node.value.uintVal; ++i)
			{
				pTable[i] = XDR_encode_int8(node.pValue2[i]);
			}
			pointer += size;

			//NOTE: add string padding
			int padding = 4 - (node.value.uintVal % 4);
			size = padding * sizeof(xdr_data_t);
			if (pointer + size > lenght)
				return -1;

			auto zero = XDR_encode_int8(0);
			pTable += node.value.uintVal;
			for (int i = 0; i < padding; ++i)
			{
				pTable[i] = zero;
			}
			pointer += size;
		}
	}

	return pointer;
}

void DynamicBody::Clear()
{
	for (auto& node : nodeList)
	{
		delete[] node.pValue2;
	}
	nodeList.clear();
}
