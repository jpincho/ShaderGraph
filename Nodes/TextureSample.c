#include "ShaderGraphInternal.h"

sgNodeHandle sgAddTextureSample ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "TextureSample", sgNodeType_TextureSample );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Texture", sgValueType_Sampler2D );
	SetPin ( &Node->Inputs[1], "UV", sgValueType_Vec2 );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Color", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}