#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

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

bool sgEmitGLSLSwizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s.%s;\n",
	                               sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Node->Swizzle );
	}
