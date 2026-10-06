#pragma once
#include <Platform/Platform.h>
#include "ShaderGraph.h"
#include <Platform/PointerArray.h>
#if defined ( PLATFORM_COMPILER_GNU )
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif

#define SG_MAX_INPUTS 8
#define SG_MAX_OUTPUTS 8
#define SG_MAX_CONSTANT_VALUES 16
#define SG_MAX_NODE_NAME_LENGTH 64
#define SG_MAX_PIN_NAME_LENGTH 64

typedef struct sgNode sgNode;
typedef struct sgConnection sgConnection;

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
	float ConstantValues[SG_MAX_CONSTANT_VALUES];
	unsigned ConstantComponentCount;
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

typedef enum
	{
	sgStage_Vertex = 0,
	sgStage_Fragment = 1
	} sgShaderStage;

static inline sgGraph *sgGraphFromHandle ( const sgGraphHandle GraphHandle )
	{
	return ( sgGraph * ) GraphHandle;
	}

static inline sgNode *sgNodeFromHandle ( const sgNodeHandle NodeHandle )
	{
	return ( sgNode * ) NodeHandle;
	}

static inline sgGraph *GetGraphFromHandle ( const sgGraphHandle GraphHandle )
	{
	return sgGraphFromHandle ( GraphHandle );
	}

static inline sgGraphHandle GetHandleFromGraph ( const sgGraph *Graph )
	{
	return ( sgGraphHandle ) Graph;
	}

static inline sgNode *GetNodeFromHandle ( const sgNodeHandle NodeHandle )
	{
	return sgNodeFromHandle ( NodeHandle );
	}

static inline sgNodeHandle GetHandleFromNode ( const sgNode *Node )
	{
	return ( sgNodeHandle ) Node;
	}

static void CopyString ( char *Dest, const unsigned DestSize, const char *Src )
	{
	if ( ( Dest == NULL ) || ( DestSize == 0 ) )
		return;
	if ( Src == NULL )
		{
		Dest[0] = '\0';
		return;
		}
	strncpy ( Dest, Src, DestSize - 1 );
	Dest[DestSize - 1] = '\0';
	}

static bool IsValidNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( ( Graph == NULL ) || ( Node == NULL ) )
		return false;
	return PointerArray_Find ( &Graph->Nodes, Node ) != -1;
	}

static void SetPin ( sgInternalPin *Pin, const char *Name, const sgValueType Type )
	{
	memset ( Pin, 0, sizeof ( *Pin ) );
	CopyString ( Pin->PinName, sizeof ( Pin->PinName ), Name );
	Pin->ValueType = Type;
	}

sgNode *CreateNode ( sgGraph *Graph, const char *Name, const sgNodeType Type );