#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddConstantFloat ( const sgGraphHandle GraphHandle, const float Value )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	char Name[SG_MAX_NODE_NAME_LENGTH];
	snprintf ( Name, sizeof ( Name ), "%g", Value );
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValues[0] = Value;
	Node->ConstantComponentCount = 1;
	SetPin ( &Node->Outputs[0], "Value", sgValueType_Float );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddConstantVec2 ( const sgGraphHandle GraphHandle, const float X, const float Y )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	char Name[SG_MAX_NODE_NAME_LENGTH];
	snprintf ( Name, sizeof ( Name ), "vec2 ( %g, %g )", X, Y );
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValues[0] = X;
	Node->ConstantValues[1] = Y;
	Node->ConstantComponentCount = 2;
	snprintf ( Node->Name, sizeof ( Node->Name ), "vec2 ( %g, %g )", Node->ConstantValues[0], Node->ConstantValues[1] );
	SetPin ( &Node->Outputs[0], "Value", sgValueType_Vec2 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddConstantVec3 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	char Name[SG_MAX_NODE_NAME_LENGTH];
	snprintf ( Name, sizeof ( Name ), "vec3 ( %g, %g, %g )", X, Y, Z );
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValues[0] = X;
	Node->ConstantValues[1] = Y;
	Node->ConstantValues[2] = Z;
	Node->ConstantComponentCount = 3;
	SetPin ( &Node->Outputs[0], "Value", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddConstantVec4 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z, const float W )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	char Name[SG_MAX_NODE_NAME_LENGTH];
	snprintf ( Name, sizeof ( Name ), "vec4 ( %g, %g, %g, %g )", X, Y, Z, W );
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValues[0] = X;
	Node->ConstantValues[1] = Y;
	Node->ConstantValues[2] = Z;
	Node->ConstantValues[3] = W;
	Node->ConstantComponentCount = 4;
	SetPin ( &Node->Outputs[0], "Value", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddIdentityMat4 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "mat4 ( 1.0 )", sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValues[0] = 1.0f;
	Node->ConstantComponentCount = 1;
	SetPin ( &Node->Outputs[0], "Value", sgValueType_Mat4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

static bool AppendConstantLiteral ( StringBuilder *Builder, const sgNode *Node )
	{
	switch ( Node->Outputs[0].ValueType )
		{
		case sgValueType_Float:
			return StringBuilder_Appendf ( Builder, "%g", Node->ConstantValues[0] );
		case sgValueType_Vec2:
			return StringBuilder_Appendf ( Builder, "vec2 ( %g, %g )", Node->ConstantValues[0], Node->ConstantValues[1] );
		case sgValueType_Vec3:
			return StringBuilder_Appendf ( Builder, "vec3 ( %g, %g, %g )",
			                               Node->ConstantValues[0], Node->ConstantValues[1], Node->ConstantValues[2] );
		case sgValueType_Vec4:
			return StringBuilder_Appendf ( Builder, "vec4 ( %g, %g, %g, %g )",
			                               Node->ConstantValues[0], Node->ConstantValues[1],
			                               Node->ConstantValues[2], Node->ConstantValues[3] );
		case sgValueType_Mat4:
			return StringBuilder_Append ( Builder, "mat4 ( 1.0 )" );
		default:
			return false;
		}
	}

bool sgEmitGLSLConstant ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	if ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	%s %s = ", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0 ) == false )
		return false;
	if ( AppendConstantLiteral ( Body, Node ) == false )
		return false;
	return StringBuilder_Append ( Body, ";\n" );
	}
