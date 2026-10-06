#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

bool sgAppendBlinnPhongLightingSupport ( const sgGraph *Graph, const sgNode *BlinnPhongNode, StringBuilder *Header )
	{
	if ( Header == NULL )
		return false;
	const char *BlinnPhongLightingGLSL =
#include "BlinnPhongLighting.glsl"
	    ;

	return StringBuilder_Appendf ( Header, BlinnPhongLightingGLSL, BlinnPhongNode->BlinnPhong.LightCount, BlinnPhongNode->BlinnPhong.LightCount, BlinnPhongNode->BlinnPhong.LightCount );
	}

sgNodeHandle sgAddBlinnPhong ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "BlinnPhong", sgNodeType_BlinnPhong );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Diffuse", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[1], "Specular", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[2], "Shininess", sgValueType_Float );
	SetPin ( &Node->Inputs[3], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[4], "WorldPosition", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[5], "CameraPosition", sgValueType_Vec3 );
	Node->InputCount = 6;
	SetPin ( &Node->Outputs[0], "Color", sgValueType_Vec4 );
	Node->OutputCount = 1;
	Node->BlinnPhong.LightCount = 8;
	return GetHandleFromNode ( Node );
	}


bool sgSetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const unsigned LightCount )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_BlinnPhong )
		return false;
	Node->BlinnPhong.LightCount = LightCount;
	return true;
	}

unsigned sgGetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return 0;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_BlinnPhong )
		return 0;
	return Node->BlinnPhong.LightCount;
	}

bool sgEmitGLSLBlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body,
	                               "	vec4 %s = EvaluateLights ( %s, %s, %s, %s, %s, %s );\n",
	                               Out0, Inputs[0], Inputs[1], Inputs[2], Inputs[3], Inputs[4], Inputs[5] );
	}
