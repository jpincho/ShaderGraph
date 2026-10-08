#include "ShaderGraphInternal.h"

sgNodeHandle sgAddComposeVec2 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec2", sgNodeType_ComposeVec2 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec2 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddComposeVec3 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec3", sgNodeType_ComposeVec3 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	SetPin ( &Node->Inputs[2], "Z", sgValueType_Float );
	Node->InputCount = 3;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddComposeVec4 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec4", sgNodeType_ComposeVec4 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	SetPin ( &Node->Inputs[2], "Z", sgValueType_Float );
	SetPin ( &Node->Inputs[3], "W", sgValueType_Float );
	Node->InputCount = 4;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}
