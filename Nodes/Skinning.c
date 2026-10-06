#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "Nodes/EmitUtils.h"

sgNodeHandle sgAddSkinning ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Skinning", sgNodeType_Skinning );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Position", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[1], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[2], "Indices", sgValueType_UVec4 );
	SetPin ( &Node->Inputs[3], "Weights", sgValueType_Vec4 );
	Node->InputCount = 4;
	SetPin ( &Node->Outputs[0], "Position", sgValueType_Vec3 );
	SetPin ( &Node->Outputs[1], "Normal", sgValueType_Vec3 );
	Node->OutputCount = 2;
	return GetHandleFromNode ( Node );
	}

bool sgSetSkinningBoneCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const unsigned BoneCount )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_Skinning )
		return false;
	Node->ArrayCount = BoneCount;
	return true;
	}

unsigned sgGetSkinningBoneCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return 0;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_Skinning )
		return 0;
	return Node->ArrayCount;
	}

bool sgEmitGLSLSkinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char OutPos[128];
	char OutNrm[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, OutPos, sizeof ( OutPos ) ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 1, Stage, OutNrm, sizeof ( OutNrm ) ) == false ) )
		return false;
	if ( StringBuilder_Appendf ( Body,
	                             "	mat4 n%u_Skin =\n"
	                             "		uBoneMatrices[int ( %s.x )] * %s.x +\n"
	                             "		uBoneMatrices[int ( %s.y )] * %s.y +\n"
	                             "		uBoneMatrices[int ( %s.z )] * %s.z +\n"
	                             "		uBoneMatrices[int ( %s.w )] * %s.w;\n",
	                             Node->NodeID, Inputs[2], Inputs[3], Inputs[2], Inputs[3], Inputs[2], Inputs[3], Inputs[2], Inputs[3] ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	vec3 %s = ( n%u_Skin * vec4 ( %s, 1.0 ) ).xyz;\n", OutPos, Node->NodeID, Inputs[0] ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = mat3 ( n%u_Skin ) * %s;\n", OutNrm, Node->NodeID, Inputs[1] );
	}
