#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddTBN ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "TBN", sgNodeType_TBN );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[1], "Tangent", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[2], "Bitangent", sgValueType_Vec3 );
	Node->InputCount = 3;
	SetPin ( &Node->Outputs[0], "TBN", sgValueType_Mat3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

bool sgEmitGLSLTBN ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( normalize ( %s ), normalize ( %s ), normalize ( %s ) );\n",
	                               Out0, Inputs[1], Inputs[2], Inputs[0] );
	}
