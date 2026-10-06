#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include <Platform/Platform.h>
#include <Platform/PointerArray.h>
#include <Platform/Logger.h>

sgNode *CreateNode ( sgGraph *Graph, const char *Name, const sgNodeType Type )
	{
	if ( Graph == NULL )
		return NULL;
	sgNode *NewNode = calloc ( 1, sizeof ( sgNode ) );
	if ( NewNode == NULL )
		return NULL;
	NewNode->Type = Type;
	NewNode->NodeID = Graph->NextNodeID++;
	CopyString ( NewNode->Name, sizeof ( NewNode->Name ), Name );
	if ( PointerArray_AddAtEnd ( &Graph->Nodes, NewNode ) == false )
		{
		free ( NewNode );
		return NULL;
		}
	return NewNode;
	}

static void DestroyNodePinArrays ( sgNode *Node )
	{
	if ( Node == NULL )
		return;
	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		PointerArray_Destroy ( &Node->Inputs[PinIndex].OutputConnections );
	for ( unsigned PinIndex = 0; PinIndex < Node->OutputCount; ++PinIndex )
		PointerArray_Destroy ( &Node->Outputs[PinIndex].OutputConnections );
	}

static void RemoveConnectionAtIndex ( sgGraph *Graph, const unsigned Index )
	{
	sgConnection *Connection = PointerArray_Get ( &Graph->Connections, Index );
	sgNode *OutputNode = Connection->OutputNode;
	sgNode *InputNode = Connection->InputNode;

	if ( ( InputNode != NULL ) && ( Connection->InputPinIndex < InputNode->InputCount ) &&
	        ( InputNode->Inputs[Connection->InputPinIndex].InputConnection == Connection ) )
		{
		InputNode->Inputs[Connection->InputPinIndex].InputConnection = NULL;
		}

	if ( ( OutputNode != NULL ) && ( Connection->OutputPinIndex < OutputNode->OutputCount ) )
		{
		const int ConnectionIndex = PointerArray_Find ( &OutputNode->Outputs[Connection->OutputPinIndex].OutputConnections, Connection );
		if ( ConnectionIndex != -1 )
			PointerArray_RemoveAt ( &OutputNode->Outputs[Connection->OutputPinIndex].OutputConnections, ( unsigned ) ConnectionIndex );
		}

	free ( Connection );
	PointerArray_RemoveAt ( &Graph->Connections, Index );
	}

static void RemoveAllConnectionsAtNode ( sgGraph *Graph, const sgNode *Target )
	{
	for ( unsigned Index = 0; Index < Graph->Connections.Count; ++Index )
		{
		sgConnection *Connection = PointerArray_Get ( &Graph->Connections, Index );
		if ( ( Connection->OutputNode == Target ) || ( Connection->InputNode == Target ) )
			{
			RemoveConnectionAtIndex ( Graph, Index );
			--Index;
			}
		}
	}

static bool CanReachForward ( const sgNode *From, const sgNode *Target, PointerArray *Visited )
	{
	if ( ( From == NULL ) || ( Target == NULL ) )
		return false;
	if ( From == Target )
		return true;
	if ( PointerArray_Find ( Visited, ( void * ) From ) != -1 )
		return false;
	if ( PointerArray_AddAtEnd ( Visited, ( void * ) From ) == false )
		return false;

	for ( unsigned PinIndex = 0; PinIndex < From->OutputCount; ++PinIndex )
		{
		const PointerArray *Outputs = &From->Outputs[PinIndex].OutputConnections;
		for ( unsigned ConnIndex = 0; ConnIndex < Outputs->Count; ++ConnIndex )
			{
			const sgConnection *Connection = PointerArray_Get ( Outputs, ConnIndex );
			if ( CanReachForward ( Connection->InputNode, Target, Visited ) )
				return true;
			}
		}
	return false;
	}

static bool WouldCreateCycle ( const sgNode *Source, const sgNode *Destination )
	{
	if ( Source == Destination )
		return true;
	PointerArray Visited;
	PointerArray_Initialize ( &Visited );
	const bool Reachable = CanReachForward ( Destination, Source, &Visited );
	PointerArray_Destroy ( &Visited );
	return Reachable;
	}

static bool NodeInputsConnected ( const sgNode *Node )
	{
	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		{
		if ( Node->Inputs[PinIndex].InputConnection == NULL )
			return false;
		}
	return true;
	}

static bool ValidateReachableSubgraph ( const sgNode *Node, PointerArray *Visiting, PointerArray *Visited )
	{
	if ( Node == NULL )
		return false;
	if ( PointerArray_Find ( Visited, ( void * ) Node ) != -1 )
		return true;
	if ( PointerArray_Find ( Visiting, ( void * ) Node ) != -1 )
		{
		LOG_ERROR ( "Cycle detected involving node '%s'", Node->Name );
		return false;
		}
	if ( PointerArray_AddAtEnd ( Visiting, ( void * ) Node ) == false )
		return false;

	if ( NodeInputsConnected ( Node ) == false )
		{
		LOG_ERROR ( "Node '%s' has unconnected inputs", Node->Name );
		PointerArray_RemoveAt ( Visiting, Visiting->Count - 1 );
		return false;
		}

	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		{
		const sgConnection *Connection = Node->Inputs[PinIndex].InputConnection;
		if ( ValidateReachableSubgraph ( Connection->OutputNode, Visiting, Visited ) == false )
			{
			PointerArray_RemoveAt ( Visiting, Visiting->Count - 1 );
			return false;
			}
		}

	PointerArray_RemoveAt ( Visiting, Visiting->Count - 1 );
	if ( PointerArray_AddAtEnd ( Visited, ( void * ) Node ) == false )
		return false;
	return true;
	}

sgGraphHandle sgCreate ( void )
	{
	sgGraph *Graph = calloc ( 1, sizeof ( sgGraph ) );
	if ( Graph == NULL )
		return sgGraphHandle_Invalid;
	PointerArray_Initialize ( &Graph->Nodes );
	PointerArray_Initialize ( &Graph->Connections );
	Graph->NextNodeID = 1;
	return GetHandleFromGraph ( Graph );
	}

void sgClear ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return;

	for ( unsigned Index = 0; Index < Graph->Connections.Count; ++Index )
		{
		sgConnection *Connection = PointerArray_Get ( &Graph->Connections, Index );
		free ( Connection );
		}
	PointerArray_Clear ( &Graph->Connections );

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		DestroyNodePinArrays ( Node );
		free ( Node );
		}
	PointerArray_Clear ( &Graph->Nodes );

	Graph->VertexPositionNode = sgNodeHandle_Invalid;
	Graph->FragmentColorNode = sgNodeHandle_Invalid;
	Graph->NextNodeID = 1;
	}

void sgDestroy ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return;
	sgClear ( GraphHandle );
	PointerArray_Destroy ( &Graph->Nodes );
	PointerArray_Destroy ( &Graph->Connections );
	free ( Graph );
	}

bool sgRemoveNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	RemoveAllConnectionsAtNode ( Graph, Node );
	if ( Graph->VertexPositionNode == NodeHandle )
		Graph->VertexPositionNode = sgNodeHandle_Invalid;
	if ( Graph->FragmentColorNode == NodeHandle )
		Graph->FragmentColorNode = sgNodeHandle_Invalid;
	const int Index = PointerArray_Find ( &Graph->Nodes, Node );
	PointerArray_RemoveAt ( &Graph->Nodes, ( unsigned ) Index );
	DestroyNodePinArrays ( Node );
	free ( Node );
	return true;
	}

unsigned sgGetNodeCount ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return 0;
	return PointerArray_GetSize ( &Graph->Nodes );
	}

sgNodeHandle sgFindNodeByIndex ( const sgGraphHandle GraphHandle, const unsigned Index )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Index >= Graph->Nodes.Count ) )
		return sgNodeHandle_Invalid;
	return GetHandleFromNode ( PointerArray_Get ( &Graph->Nodes, Index ) );
	}

sgNodeHandle sgFindNodeById ( const sgGraphHandle GraphHandle, const unsigned ID )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( Node->NodeID == ID )
			return GetHandleFromNode ( Node );
		}
	return sgNodeHandle_Invalid;
	}

unsigned sgGetNodeId ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return 0;
	return GetNodeFromHandle ( NodeHandle )->NodeID;
	}

sgNodeType sgGetNodeType ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return sgNodeType_Invalid;
	return GetNodeFromHandle ( NodeHandle )->Type;
	}

bool sgSetNodeName ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const char *Name )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node == NULL )
		return false;
	CopyString ( Node->Name, sizeof ( Node->Name ), Name );
	return true;
	}

const char *sgGetNodeName ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return "";
	return GetNodeFromHandle ( NodeHandle )->Name;
	}

bool sgAddConnection ( const sgGraphHandle GraphHandle, const sgNodeHandle SourceNodeHandle, const unsigned SourcePinIndex, const sgNodeHandle DestinationNodeHandle, const unsigned DestinationPinIndex )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return false;
	if ( IsValidNode ( GraphHandle, SourceNodeHandle ) == false )
		return false;
	if ( IsValidNode ( GraphHandle, DestinationNodeHandle ) == false )
		return false;
	sgNode *SourceNode = GetNodeFromHandle ( SourceNodeHandle );
	sgNode *DestinationNode = GetNodeFromHandle ( DestinationNodeHandle );

	if ( SourcePinIndex >= SourceNode->OutputCount )
		{
		LOG_ERROR ( "SourcePinIndex %u is out of range for SourceNode with %u outputs", SourcePinIndex, SourceNode->OutputCount );
		return false;
		}
	if ( DestinationPinIndex >= DestinationNode->InputCount )
		{
		LOG_ERROR ( "DestinationPinIndex %u is out of range for DestinationNode with %u inputs", DestinationPinIndex, DestinationNode->InputCount );
		return false;
		}
	if ( SourceNode->Outputs[SourcePinIndex].ValueType != DestinationNode->Inputs[DestinationPinIndex].ValueType )
		{
		LOG_ERROR ( "Incompatible pin types between SourceNode and DestinationNode" );
		return false;
		}
	if ( DestinationNode->Inputs[DestinationPinIndex].InputConnection != NULL )
		{
		LOG_ERROR ( "Destination pin is already connected" );
		return false;
		}
	if ( WouldCreateCycle ( SourceNode, DestinationNode ) )
		{
		LOG_ERROR ( "Connection would create a cycle" );
		return false;
		}

	sgConnection *Connection = calloc ( 1, sizeof ( sgConnection ) );
	if ( Connection == NULL )
		return false;
	Connection->OutputNode = SourceNode;
	Connection->InputNode = DestinationNode;
	Connection->OutputPinIndex = SourcePinIndex;
	Connection->InputPinIndex = DestinationPinIndex;
	if ( PointerArray_AddAtEnd ( &Graph->Connections, Connection ) == false )
		{
		free ( Connection );
		return false;
		}
	if ( PointerArray_AddAtEnd ( &SourceNode->Outputs[SourcePinIndex].OutputConnections, Connection ) == false )
		{
		PointerArray_RemoveAt ( &Graph->Connections, Graph->Connections.Count - 1 );
		free ( Connection );
		return false;
		}
	DestinationNode->Inputs[DestinationPinIndex].InputConnection = Connection;
	return true;
	}

bool sgRemoveConnection ( const sgGraphHandle GraphHandle, const sgNodeHandle SourceNodeHandle, const unsigned SourcePinIndex, const sgNodeHandle DestinationNodeHandle, const unsigned DestinationPinIndex )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return false;
	if ( IsValidNode ( GraphHandle, SourceNodeHandle ) == false )
		return false;
	if ( IsValidNode ( GraphHandle, DestinationNodeHandle ) == false )
		return false;
	sgNode *SourceNode = GetNodeFromHandle ( SourceNodeHandle );
	sgNode *DestinationNode = GetNodeFromHandle ( DestinationNodeHandle );

	for ( unsigned Index = 0; Index < Graph->Connections.Count; ++Index )
		{
		sgConnection *Connection = PointerArray_Get ( &Graph->Connections, Index );
		if ( ( Connection->OutputNode == SourceNode ) && ( Connection->OutputPinIndex == SourcePinIndex ) &&
		        ( Connection->InputNode == DestinationNode ) && ( Connection->InputPinIndex == DestinationPinIndex ) &&
		        ( Connection == DestinationNode->Inputs[DestinationPinIndex].InputConnection ) )
			{
			RemoveConnectionAtIndex ( Graph, Index );
			return true;
			}
		}
	return false;
	}

bool sgValidateGraph ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		{
		LOG_ERROR ( "GraphHandle is NULL" );
		return false;
		}

	bool IsValid = true;
	if ( Graph->VertexPositionNode == sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Graph is missing VertexPosition node" );
		IsValid = false;
		}
	if ( Graph->FragmentColorNode == sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Graph is missing FragmentColor node" );
		IsValid = false;
		}
	if ( IsValid == false )
		return false;

	for ( unsigned Index = 0; Index < Graph->Connections.Count; ++Index )
		{
		sgConnection *Connection = PointerArray_Get ( &Graph->Connections, Index );
		if ( ( Connection->OutputNode == NULL ) || ( Connection->InputNode == NULL ) )
			{
			LOG_ERROR ( "Connection at index %u has NULL nodes", Index );
			IsValid = false;
			continue;
			}
		if ( Connection->OutputPinIndex >= Connection->OutputNode->OutputCount )
			{
			LOG_ERROR ( "Connection at index %u has invalid OutputPinIndex %u", Index, Connection->OutputPinIndex );
			IsValid = false;
			}
		if ( Connection->InputPinIndex >= Connection->InputNode->InputCount )
			{
			LOG_ERROR ( "Connection at index %u has invalid InputPinIndex %u", Index, Connection->InputPinIndex );
			IsValid = false;
			}
		if ( ( Connection->InputPinIndex < Connection->InputNode->InputCount ) &&
		        ( Connection != Connection->InputNode->Inputs[Connection->InputPinIndex].InputConnection ) )
			{
			LOG_ERROR ( "Connection at index %u is not registered in the input node", Index );
			IsValid = false;
			}
		if ( ( Connection->OutputPinIndex < Connection->OutputNode->OutputCount ) &&
		        ( PointerArray_Find ( &Connection->OutputNode->Outputs[Connection->OutputPinIndex].OutputConnections, Connection ) == -1 ) )
			{
			LOG_ERROR ( "Connection at index %u is not registered in the output node", Index );
			IsValid = false;
			}
		if ( ( Connection->OutputPinIndex < Connection->OutputNode->OutputCount ) &&
		        ( Connection->InputPinIndex < Connection->InputNode->InputCount ) &&
		        ( Connection->OutputNode->Outputs[Connection->OutputPinIndex].ValueType !=
		          Connection->InputNode->Inputs[Connection->InputPinIndex].ValueType ) )
			{
			LOG_ERROR ( "Connection at index %u has mismatched pin types", Index );
			IsValid = false;
			}
		}
	if ( IsValid == false )
		return false;

	PointerArray Visiting;
	PointerArray Visited;
	PointerArray_Initialize ( &Visiting );
	PointerArray_Initialize ( &Visited );

	if ( ValidateReachableSubgraph ( GetNodeFromHandle ( Graph->VertexPositionNode ), &Visiting, &Visited ) == false )
		IsValid = false;
	PointerArray_Clear ( &Visiting );
	PointerArray_Clear ( &Visited );
	if ( ValidateReachableSubgraph ( GetNodeFromHandle ( Graph->FragmentColorNode ), &Visiting, &Visited ) == false )
		IsValid = false;

	PointerArray_Destroy ( &Visiting );
	PointerArray_Destroy ( &Visited );
	return IsValid;
	}

const char *sgStringifyNodeType ( const sgNodeType Value )
	{
#define STRINGIFY(X) case sgNodeType_##X:return #X;
	switch ( Value )
		{
			STRINGIFY ( Invalid );
			STRINGIFY ( Attribute );
			STRINGIFY ( Uniform );
			STRINGIFY ( Constant );
			STRINGIFY ( TextureSample );
			STRINGIFY ( Multiply );
			STRINGIFY ( Add );
			STRINGIFY ( Normalize );
			STRINGIFY ( ComposeVec2 );
			STRINGIFY ( ComposeVec3 );
			STRINGIFY ( ComposeVec4 );
			STRINGIFY ( Swizzle );
			STRINGIFY ( Mat3Cast );
			STRINGIFY ( Skinning );
			STRINGIFY ( TBN );
			STRINGIFY ( NormalMap );
			STRINGIFY ( BlinnPhong );
			STRINGIFY ( VertexPosition );
			STRINGIFY ( FragmentColor );
		}
#undef STRINGIFY
	return "Unknown sgNodeType";
	}

const char *sgStringifyValueType ( const sgValueType Value )
	{
#define STRINGIFY(X) case sgValueType_##X:return #X;
	switch ( Value )
		{
			STRINGIFY ( Float );
			STRINGIFY ( Vec2 );
			STRINGIFY ( Vec3 );
			STRINGIFY ( Vec4 );
			STRINGIFY ( Unsigned );
			STRINGIFY ( UVec2 );
			STRINGIFY ( UVec3 );
			STRINGIFY ( UVec4 );
			STRINGIFY ( Mat3 );
			STRINGIFY ( Mat4 );
			STRINGIFY ( Sampler2D );
			STRINGIFY ( Void );
		}
#undef STRINGIFY
	return "Unknown sgValueType";
	}
