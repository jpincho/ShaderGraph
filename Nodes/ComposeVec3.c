#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

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

bool sgEmitGLSLComposeVec3 ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = vec3 ( %s, %s, %s );\n", Out0, Inputs[0], Inputs[1], Inputs[2] );
	}
