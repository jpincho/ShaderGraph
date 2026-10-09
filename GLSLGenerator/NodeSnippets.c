#include "NodeSnippets.h"
#include "GeneratorUtils.h"

bool AddGLSLBody_Constant ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	if ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	%s %s = ", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0 ) == false )
		return false;

	switch ( Node->Outputs[0].ValueType )
		{
		case sgValueType_Float:
			return StringBuilder_Appendf ( Body, "%g;\n", Node->ConstantValues[0] );
		case sgValueType_Vec2:
			return StringBuilder_Appendf ( Body, "vec2 ( %g, %g );\n", Node->ConstantValues[0], Node->ConstantValues[1] );
		case sgValueType_Vec3:
			return StringBuilder_Appendf ( Body, "vec3 ( %g, %g, %g );\n",
			                               Node->ConstantValues[0], Node->ConstantValues[1], Node->ConstantValues[2] );
		case sgValueType_Vec4:
			return StringBuilder_Appendf ( Body, "vec4 ( %g, %g, %g, %g );\n",
			                               Node->ConstantValues[0], Node->ConstantValues[1],
			                               Node->ConstantValues[2], Node->ConstantValues[3] );
		case sgValueType_Mat4:
			return StringBuilder_Append ( Body, "mat4 ( 1.0 );\n" );
		default:
			return false;
		}
	return false;
	}

bool AddGLSLBody_TextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec4 %s = texture ( %s, %s );\n", Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Multiplication ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s * %s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Addition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec4 %s = %s + %s;\n", Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Normalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s );\n", Out0, Inputs[0] );
	}

bool AddGLSLBody_ComposeVec ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	switch ( Node->InputCount )
		{
		case 2:
			return StringBuilder_Appendf ( Body, "	vec2 %s = vec2 ( %s, %s );\n", Out0, Inputs[0], Inputs[1] );
		case 3:
			return StringBuilder_Appendf ( Body, "	vec3 %s = vec3 ( %s, %s, %s );\n", Out0, Inputs[0], Inputs[1], Inputs[2] );
		case 4:
			return StringBuilder_Appendf ( Body, "	vec4 %s = vec4 ( %s, %s, %s, %s );\n", Out0, Inputs[0], Inputs[1], Inputs[2], Inputs[3] );
		}
	return false;
	}

bool AddGLSLBody_Swizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s.%s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Node->Swizzle );
	}

bool AddGLSLBody_Mat3Cast ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( %s );\n", Out0, Inputs[0] );
	}

bool AddGLSLBody_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
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

bool AddGLSLBody_TBN ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( normalize ( %s ), normalize ( %s ), normalize ( %s ) );\n",
	                               Out0, Inputs[1], Inputs[2], Inputs[0] );
	}

bool AddGLSLBody_NormalMap ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( sgGetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s * normalize ( %s * 2.0 - 1.0 ) );\n", Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
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

bool AddGLSLBody_VertexPosition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Vertex )
		return true;
	char Inputs[SG_MAX_INPUTS][128];
	if ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	gl_Position = %s;\n", Inputs[0] );
	}

bool AddGLSLBody_FragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Fragment )
		return true;
	char Inputs[SG_MAX_INPUTS][128];
	if ( sgLoadNodeInputs ( Node, Stage, Inputs ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	FragColor = %s;\n", Inputs[0] );
	}

bool AddGLSLDeclaration_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Body == NULL )
		return false;
	UNUSED ( Stage );
	const char *LightDeclarationsGLSL =
#include "LightDeclarations.glsl"
	    ;
	const char *BlinnPhongLightingGLSL =
#include "BlinnPhongLighting.glsl"
	    ;
	if ( StringBuilder_Append ( Body, LightDeclarationsGLSL ) == false )
		return false;
	return StringBuilder_Appendf ( Body, BlinnPhongLightingGLSL, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount );
	}
