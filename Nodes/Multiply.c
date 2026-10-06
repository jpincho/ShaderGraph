#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

static sgValueType MultiplyOutputType ( const sgValueType A, const sgValueType B )
	{
	if ( ( A == sgValueType_Mat4 ) && ( B == sgValueType_Mat4 ) )
		return sgValueType_Mat4;
	if ( ( A == sgValueType_Mat4 ) && ( B == sgValueType_Vec4 ) )
		return sgValueType_Vec4;
	if ( ( A == sgValueType_Mat3 ) && ( B == sgValueType_Vec3 ) )
		return sgValueType_Vec3;
	if ( ( A == sgValueType_Float ) && ( B != sgValueType_Void ) )
		return B;
	if ( ( B == sgValueType_Float ) && ( A != sgValueType_Void ) )
		return A;
	if ( ( A == B ) && ( A != sgValueType_Void ) )
		return A;
	return sgValueType_Void;
	}

sgNodeHandle sgAddMultiplyTyped ( const sgGraphHandle GraphHandle, const sgValueType AType, const sgValueType BType )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	const sgValueType OutType = MultiplyOutputType ( AType, BType );
	if ( ( Graph == NULL ) || ( OutType == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Multiply", sgNodeType_Multiply );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "A", AType );
	SetPin ( &Node->Inputs[1], "B", BType );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", OutType );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddMultiply ( const sgGraphHandle GraphHandle )
	{
	return sgAddMultiplyTyped ( GraphHandle, sgValueType_Vec4, sgValueType_Vec4 );
	}

bool sgEmitGLSLMultiply ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s * %s;\n",
	                               sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Inputs[1] );
	}
