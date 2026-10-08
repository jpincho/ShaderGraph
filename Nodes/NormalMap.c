#include "ShaderGraphInternal.h"

sgNodeHandle sgAddNormalMap ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "NormalMap", sgNodeType_NormalMap );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "TBN", sgValueType_Mat3 );
	SetPin ( &Node->Inputs[1], "Sample", sgValueType_Vec3 );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Normal", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}