#include "ShaderGraphInternal.h"

sgNodeHandle sgAddAddition ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Add", sgNodeType_Add );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "A", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[1], "B", sgValueType_Vec4 );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}