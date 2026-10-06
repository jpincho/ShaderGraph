#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddBlinnPhong ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "BlinnPhong", sgNodeType_BlinnPhong );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Albedo", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[1], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[2], "LightDir", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[3], "LightColor", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[4], "ViewDir", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[5], "Shininess", sgValueType_Float );
	Node->InputCount = 6;
	SetPin ( &Node->Outputs[0], "Color", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

bool sgEmitGLSLBlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	if ( StringBuilder_Appendf ( Body,
	                             "	vec3 n%u_N = normalize ( %s );\n"
	                             "	vec3 n%u_L = normalize ( %s );\n"
	                             "	vec3 n%u_V = normalize ( %s );\n"
	                             "	vec3 n%u_H = normalize ( n%u_L + n%u_V );\n"
	                             "	float n%u_Diff = max ( dot ( n%u_N, n%u_L ), 0.0 );\n"
	                             "	float n%u_Spec = pow ( max ( dot ( n%u_N, n%u_H ), 0.0 ), %s );\n"
	                             "	vec3 n%u_Color = %s * %s * n%u_Diff + %s * n%u_Spec;\n"
	                             "	vec4 %s = vec4 ( n%u_Color, 1.0 );\n",
	                             Node->NodeID, Inputs[1],
	                             Node->NodeID, Inputs[2],
	                             Node->NodeID, Inputs[4],
	                             Node->NodeID, Node->NodeID, Node->NodeID,
	                             Node->NodeID, Node->NodeID, Node->NodeID,
	                             Node->NodeID, Node->NodeID, Node->NodeID, Inputs[5],
	                             Node->NodeID, Inputs[0], Inputs[3], Node->NodeID, Inputs[3], Node->NodeID,
	                             Out0, Node->NodeID ) == false )
		return false;
	return true;
	}
