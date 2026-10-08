#include "ShaderGraphInternal.h"

sgNodeHandle sgAddMat3Cast ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Mat3Cast", sgNodeType_Mat3Cast );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Matrix", sgValueType_Mat4 );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Mat3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}