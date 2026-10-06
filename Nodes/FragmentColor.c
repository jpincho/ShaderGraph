#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddFragmentColor ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( Graph->FragmentColorNode != sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "FragmentColor", sgNodeType_FragmentColor );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Color", sgValueType_Vec4 );
	Node->InputCount = 1;
	Graph->FragmentColorNode = GetHandleFromNode ( Node );
	return Graph->FragmentColorNode;
	}

bool sgEmitGLSLFragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Fragment )
		return true;
	char Inputs[SG_MAX_INPUTS][128];
	if ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	FragColor = %s;\n", Inputs[0] );
	}
