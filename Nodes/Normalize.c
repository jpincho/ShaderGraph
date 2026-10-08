#include "ShaderGraphInternal.h"

sgNodeHandle sgAddNormalize ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Normalize", sgNodeType_Normalize );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Vector", sgValueType_Vec3 );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}
