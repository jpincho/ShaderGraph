#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

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

bool sgEmitGLSLComposeVec2 ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec2 %s = vec2 ( %s, %s );\n", Out0, Inputs[0], Inputs[1] );
	}
