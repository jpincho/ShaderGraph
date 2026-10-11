#include "GLSLNodeSnippets.h"
#include "GeneratorUtils.h"
#include <Platform/Logger.h>
#include <stdio.h>
#include <string.h>

static bool AppendGLSLFloat ( StringBuilder *Body, const float Value )
	{
	if ( ( Value * 0.0f ) != 0.0f )
		return false;

	char Buffer[64];
	const float Absolute = ( Value < 0.0f ) ? -Value : Value;
	const bool UseScientific = ( Absolute != 0.0f ) && ( ( Absolute < 1.0e-4f ) || ( Absolute >= 1.0e7f ) );
	if ( UseScientific )
		{
		if ( snprintf ( Buffer, sizeof ( Buffer ), "%.9g", ( double ) Value ) < 0 )
			return false;
		}
	else if ( snprintf ( Buffer, sizeof ( Buffer ), "%.8f", ( double ) Value ) < 0 )
		return false;

	for ( char *Cursor = Buffer; *Cursor != '\0'; ++Cursor )
		{
		if ( *Cursor == ',' )
			*Cursor = '.';
		}

	char *Dot = strchr ( Buffer, '.' );
	char *Exponent = strchr ( Buffer, 'e' );
	if ( Exponent == NULL )
		Exponent = strchr ( Buffer, 'E' );
	if ( ( Dot != NULL ) && ( Exponent == NULL ) )
		{
		char *End = Buffer + strlen ( Buffer ) - 1;
		while ( ( End > Dot + 1 ) && ( *End == '0' ) )
			{
			*End = '\0';
			--End;
			}
		}
	else if ( ( Dot == NULL ) && ( Exponent == NULL ) )
		{
		const unsigned Length = ( unsigned ) strlen ( Buffer );
		if ( ( Length + 2 ) >= sizeof ( Buffer ) )
			return false;
		Buffer[Length] = '.';
		Buffer[Length + 1] = '0';
		Buffer[Length + 2] = '\0';
		}
	return StringBuilder_Append ( Body, Buffer );
	}

static bool AppendGLSLFloats ( StringBuilder *Body, const float *Values, const unsigned Count, const char *Prefix, const char *Suffix )
	{
	if ( StringBuilder_Append ( Body, Prefix ) == false )
		return false;
	for ( unsigned Index = 0; Index < Count; ++Index )
		{
		if ( ( Index > 0 ) && ( StringBuilder_Append ( Body, ", " ) == false ) )
			return false;
		if ( AppendGLSLFloat ( Body, Values[Index] ) == false )
			return false;
		}
	return StringBuilder_Append ( Body, Suffix );
	}

bool AddGLSLBody_Constant ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	if ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	%s %s = ", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0 ) == false )
		return false;

	bool WroteValue = false;
	switch ( Node->Outputs[0].ValueType )
		{
		case sgValueType_Float:
			WroteValue = AppendGLSLFloat ( Body, Node->ConstantValue.Float );
			break;
		case sgValueType_Unsigned:
			WroteValue = StringBuilder_Appendf ( Body, "%uu", Node->ConstantValue.Unsigned );
			break;
		case sgValueType_UVec2:
			WroteValue = StringBuilder_Appendf ( Body, "uvec2 ( %uu, %uu )", Node->ConstantValue.UVec2[0], Node->ConstantValue.UVec2[1] );
			break;
		case sgValueType_UVec3:
			WroteValue = StringBuilder_Appendf ( Body, "uvec3 ( %uu, %uu, %uu )", Node->ConstantValue.UVec3[0], Node->ConstantValue.UVec3[1], Node->ConstantValue.UVec3[2] );
			break;
		case sgValueType_UVec4:
			WroteValue = StringBuilder_Appendf ( Body, "uvec4 ( %uu, %uu, %uu, %uu )",
			                                    Node->ConstantValue.UVec4[0], Node->ConstantValue.UVec4[1],
			                                    Node->ConstantValue.UVec4[2], Node->ConstantValue.UVec4[3] );
			break;
		case sgValueType_Mat3:
			WroteValue = AppendGLSLFloats ( Body, Node->ConstantValue.Matrix3, 9, "mat3 ( ", " )" );
			break;
		case sgValueType_Vec2:
			WroteValue = AppendGLSLFloats ( Body, Node->ConstantValue.Vec2, 2, "vec2 ( ", " )" );
			break;
		case sgValueType_Vec3:
			WroteValue = AppendGLSLFloats ( Body, Node->ConstantValue.Vec3, 3, "vec3 ( ", " )" );
			break;
		case sgValueType_Vec4:
			WroteValue = AppendGLSLFloats ( Body, Node->ConstantValue.Vec4, 4, "vec4 ( ", " )" );
			break;
		case sgValueType_Mat4:
			WroteValue = AppendGLSLFloats ( Body, Node->ConstantValue.Matrix4, 16, "mat4 ( ", " )" );
			break;
		default:
			return false;
		}
	if ( WroteValue == false )
		return false;
	return StringBuilder_Append ( Body, ";\n" );
	}

bool AddGLSLBody_TextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec4 %s = texture ( %s, %s );\n", Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Multiplication ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s * %s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Addition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s + %s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_Normalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s );\n", Out0, Inputs[0] );
	}

bool AddGLSLBody_ComposeVec ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	const char *GlslType = sgValueTypeToGLSL ( Node->Outputs[0].ValueType );
	switch ( Node->InputCount )
		{
		case 2:
			return StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s );\n", GlslType, Out0, GlslType, Inputs[0], Inputs[1] );
		case 3:
			return StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s, %s );\n", GlslType, Out0, GlslType, Inputs[0], Inputs[1], Inputs[2] );
		case 4:
			return StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s, %s, %s );\n", GlslType, Out0, GlslType, Inputs[0], Inputs[1], Inputs[2], Inputs[3] );
		}
	return false;
	}

bool AddGLSLBody_Swizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	%s %s = %s.%s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Out0, Inputs[0], Node->Swizzle );
	}

bool AddGLSLBody_Mat3Cast ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( %s );\n", Out0, Inputs[0] );
	}

bool AddGLSLBody_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char OutPos[128];
	char OutNrm[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, OutPos, sizeof ( OutPos ) ) == false ) ||
	        ( GetOutputSymbol ( Node, 1, Stage, OutNrm, sizeof ( OutNrm ) ) == false ) )
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
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( normalize ( %s ), normalize ( %s ), normalize ( %s ) );\n",
	                               Out0, Inputs[1], Inputs[2], Inputs[0] );
	}

bool AddGLSLBody_NormalMap ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
		return false;
	return StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s * normalize ( %s * 2.0 - 1.0 ) );\n", Out0, Inputs[0], Inputs[1] );
	}

bool AddGLSLBody_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	char Out0[128];
	char Inputs[SG_MAX_INPUTS][128];
	if ( ( LoadNodeInputs ( Node, Stage, Inputs ) == false ) ||
	        ( GetOutputSymbol ( Node, 0, Stage, Out0, sizeof ( Out0 ) ) == false ) )
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
	if ( LoadNodeInputs ( Node, Stage, Inputs ) == false )
		return false;
	return StringBuilder_Appendf ( Body, "	gl_Position = %s;\n", Inputs[0] );
	}

bool AddGLSLBody_FragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Fragment )
		return true;
	char Inputs[SG_MAX_INPUTS][128];
	if ( LoadNodeInputs ( Node, Stage, Inputs ) == false )
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
	if ( Node->BlinnPhong.LightCount == 0 )
		{
		LOG_ERROR ( "BlinnPhong node '%s' has a light count of 0", Node->Name );
		return false;
		}
	if ( StringBuilder_Appendf ( Body, LightDeclarationsGLSL, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount ) == false )
		return false;
	return StringBuilder_Append ( Body, BlinnPhongLightingGLSL );
	}
