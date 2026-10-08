#include "ShaderGraphInternal.h"

sgNodeHandle sgAddSwizzle ( const sgGraphHandle GraphHandle, const char *Swizzle, const sgValueType InputType, const sgValueType OutputType )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Swizzle == NULL ) || ( Swizzle[0] == '\0' ) ||
	        ( InputType == sgValueType_Void ) || ( OutputType == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Swizzle", sgNodeType_Swizzle );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	CopyString ( Node->Swizzle, sizeof ( Node->Swizzle ), Swizzle );
	SetPin ( &Node->Inputs[0], "Value", InputType );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], "Result", OutputType );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}
