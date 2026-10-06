#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

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

bool sgEmitGLSLTextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec4 %s = texture ( %s, %s );\n", Out0, Inputs[0], Inputs[1] );
	}
