#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddVertexPosition ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( Graph->VertexPositionNode != sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "VertexPosition", sgNodeType_VertexPosition );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Position", sgValueType_Vec4 );
	Node->InputCount = 1;
	Graph->VertexPositionNode = GetHandleFromNode ( Node );
	return Graph->VertexPositionNode;
	}

bool sgEmitGLSLVertexPosition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Vertex )
		return true;
	char Inputs[SG_MAX_INPUTS][128];
	if ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	gl_Position = %s;\n", Inputs[0] );
	}
