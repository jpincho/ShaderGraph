#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddUniform ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Uniform );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Outputs[0], Name, Type );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddUniformArray ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type, const unsigned Count )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) || ( Count == 0 ) )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Uniform );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Outputs[0], Name, Type );
	Node->OutputCount = 1;
	Node->ArrayCount = Count;
	return GetHandleFromNode ( Node );
	}

bool sgEmitGLSLUniform ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	( void ) Body;
	( void ) Node;
	( void ) Stage;
	return true;
	}
