#include "ShaderGraphInternal.h"
#include <string.h>

void CopyString ( char *Dest, const unsigned DestSize, const char *Src )
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

bool IsValidNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( ( Graph == NULL ) || ( Node == NULL ) )
		return false;
	return PointerArray_Find ( &Graph->Nodes, Node ) != -1;
	}

bool IsTakenShaderName ( const sgGraph *Graph, const char *Name, const sgNode *Except )
	{
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) )
		return true;
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		const sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( ( Node == Except ) || ( ( Node->Type != sgNodeType_Attribute ) && ( Node->Type != sgNodeType_Uniform ) ) )
			continue;
		if ( strcmp ( Node->Name, Name ) == 0 )
			return true;
		}
	return false;
	}

void SetPin ( sgInternalPin *Pin, const char *Name, const sgValueType Type )
	{
	memset ( Pin, 0, sizeof ( *Pin ) );
	CopyString ( Pin->PinName, sizeof ( Pin->PinName ), Name );
	Pin->ValueType = Type;
	}

bool CopyValue ( const sgValue *Origin, sgValue *Destination )
	{
	if ( ( Origin == NULL ) || ( Destination == NULL ) )
		return false;
	memset ( Destination, 0, sizeof ( sgValue ) );
	switch ( Origin->Type )
		{
		case sgValueType_Float:
		case sgValueType_Vec2:
		case sgValueType_Vec3:
		case sgValueType_Vec4:
			{
			const unsigned ComponentCount = ( unsigned ) ( Origin->Type - sgValueType_Float ) + 1;
			for ( unsigned Index = 0; Index < ComponentCount; ++Index )
				Destination->Vec4[Index] = Origin->Vec4[Index];
			break;
			}
		case sgValueType_Unsigned:
		case sgValueType_UVec2:
		case sgValueType_UVec3:
		case sgValueType_UVec4:
			{
			const unsigned ComponentCount = ( unsigned ) ( Origin->Type - sgValueType_Unsigned ) + 1;
			for ( unsigned Index = 0; Index < ComponentCount; ++Index )
				Destination->UVec4[Index] = Origin->UVec4[Index];
			break;
			}
		case sgValueType_Mat3:
		case sgValueType_Mat4:
			{
			unsigned Sizes[2] = {9, 16};
			unsigned ComponentCount = Sizes[Origin->Type - sgValueType_Mat3];
			for ( unsigned Index = 0; Index < ComponentCount; ++Index )
				Destination->Matrix4[Index] = Origin->Matrix4[Index];
			break;
			}
		default:
			{
			LOG_ERROR ( "Unhandled type" );
			return false;
			}
		}
	Destination->Type = Origin->Type;
	return true;
	}

void DestroyNodePinArrays ( sgNode *Node )
	{
	if ( Node == NULL )
		return;
	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		PointerArray_Destroy ( &Node->Inputs[PinIndex].OutputConnections );
	for ( unsigned PinIndex = 0; PinIndex < Node->OutputCount; ++PinIndex )
		PointerArray_Destroy ( &Node->Outputs[PinIndex].OutputConnections );
	}

void RemoveConnectionAtIndex ( sgGraph *Graph, const unsigned Index )
	{
	sgConnection *Connection = ( sgConnection * ) PointerArray_Get ( &Graph->Connections, Index );
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

void RemoveAllConnectionsAtNode ( sgGraph *Graph, const sgNode *Target )
	{
	for ( unsigned Index = 0; Index < Graph->Connections.Count; ++Index )
		{
		sgConnection *Connection = ( sgConnection * ) PointerArray_Get ( &Graph->Connections, Index );
		if ( ( Connection->OutputNode == Target ) || ( Connection->InputNode == Target ) )
			{
			RemoveConnectionAtIndex ( Graph, Index );
			--Index;
			}
		}
	}

/* 1 = reachable, 0 = not reachable, -1 = allocation failure. */
int CanReachForward ( const sgNode *From, const sgNode *Target, PointerArray *Visited )
	{
	if ( ( From == NULL ) || ( Target == NULL ) )
		return 0;
	if ( From == Target )
		return 1;
	if ( PointerArray_Find ( Visited, ( void * ) From ) != -1 )
		return 0;
	if ( PointerArray_AddAtEnd ( Visited, ( void * ) From ) == false )
		return -1;

	for ( unsigned PinIndex = 0; PinIndex < From->OutputCount; ++PinIndex )
		{
		const PointerArray *Outputs = &From->Outputs[PinIndex].OutputConnections;
		for ( unsigned ConnIndex = 0; ConnIndex < Outputs->Count; ++ConnIndex )
			{
			const sgConnection *Connection = ( sgConnection * ) PointerArray_Get ( Outputs, ConnIndex );
			const int Reachable = CanReachForward ( Connection->InputNode, Target, Visited );
			if ( Reachable != 0 )
				return Reachable;
			}
		}
	return 0;
	}

int WouldCreateCycle ( const sgNode *Source, const sgNode *Destination )
	{
	if ( Source == Destination )
		return 1;
	PointerArray Visited;
	PointerArray_Initialize ( &Visited );
	const int Reachable = CanReachForward ( Destination, Source, &Visited );
	PointerArray_Destroy ( &Visited );
	return Reachable;
	}

bool NodeInputsConnected ( const sgNode *Node )
	{
	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		{
		if ( Node->Inputs[PinIndex].InputConnection == NULL )
			return false;
		}
	return true;
	}

bool ValidateReachableSubgraph ( const sgNode *Node, PointerArray *Visiting, PointerArray *Visited )
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