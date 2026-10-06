#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

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

bool sgEmitGLSLNormalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s );\n", Out0, Inputs[0] );
	}
