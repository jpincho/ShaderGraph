#pragma once
#include <Platform/Platform.h>
#include <Platform/PointerArray.h>
#include <Platform/Logger.h>
#include "ShaderGraph.h"
#include "StringBuilder.h"
#if defined ( PLATFORM_COMPILER_GNU )
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif

#define SG_MAX_INPUTS 8
#define SG_MAX_OUTPUTS 8
#define SG_MAX_NODE_NAME_LENGTH 64
#define SG_MAX_PIN_NAME_LENGTH 64

typedef struct sgNode sgNode;
typedef struct sgConnection sgConnection;

typedef enum
	{
	sgStage_Vertex = 0,
	sgStage_Fragment = 1
	} sgShaderStage;

typedef struct sgInternalPin
	{
	char PinName[SG_MAX_PIN_NAME_LENGTH];
	sgValueType ValueType;
	sgConnection *InputConnection;
	PointerArray OutputConnections;
	} sgInternalPin;

struct sgNode
	{
	char Name[SG_MAX_NODE_NAME_LENGTH];
	unsigned NodeID;

	sgNodeType Type;
	sgInternalPin Inputs[SG_MAX_INPUTS];
	sgInternalPin Outputs[SG_MAX_OUTPUTS];
	unsigned InputCount, OutputCount;
	unsigned ArrayCount;
	char Swizzle[8];

	union
		{
		struct
			{
			unsigned LightCount;
			} BlinnPhong;
		struct
			{
			unsigned BoneCount;
			} Skinning;
		sgValue ConstantValue;
		};
	bool UsedInShaderStage[2];// Indexed with sgShaderStage enum values
	char TopoColor; // 0 = unvisited, 1 = visiting, 2 = visited
	};

typedef struct
	{
	PointerArray Nodes;
	PointerArray Connections;
	sgNodeHandle VertexPositionNode, FragmentColorNode;
	unsigned NextNodeID;
	} sgGraph;

struct sgConnection
	{
	sgNode *OutputNode, *InputNode;
	unsigned OutputPinIndex, InputPinIndex;
	};

static inline sgGraph *GetGraphFromHandle ( const sgGraphHandle GraphHandle )
	{
	return ( sgGraph* ) GraphHandle;
	}

static inline sgGraphHandle GetHandleFromGraph ( const sgGraph *Graph )
	{
	return ( const sgGraphHandle ) Graph;
	}

static inline sgNode *GetNodeFromHandle ( const sgNodeHandle NodeHandle )
	{
	return ( sgNode* ) NodeHandle;
	}

static inline sgNodeHandle GetHandleFromNode ( const sgNode *Node )
	{
	return ( const sgNodeHandle ) Node;
	}

void CopyString ( char *Dest, const unsigned DestSize, const char *Src );
bool IsValidNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );
bool IsTakenShaderName ( const sgGraph *Graph, const char *Name, const sgNode *Except );
void SetPin ( sgInternalPin *Pin, const char *Name, const sgValueType Type );
sgNode *CreateNode ( sgGraph *Graph, const char *Name, const sgNodeType Type );
bool CopyValue ( const sgValue *Origin, sgValue *Destination );
void DestroyNodePinArrays ( sgNode *Node );
void RemoveConnectionAtIndex ( sgGraph *Graph, const unsigned Index );
void RemoveAllConnectionsAtNode ( sgGraph *Graph, const sgNode *Target );
int CanReachForward ( const sgNode *From, const sgNode *Target, PointerArray *Visited );
int WouldCreateCycle ( const sgNode *Source, const sgNode *Destination );
bool NodeInputsConnected ( const sgNode *Node );
bool ValidateReachableSubgraph ( const sgNode *Node, PointerArray *Visiting, PointerArray *Visited );