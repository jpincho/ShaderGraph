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
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder_Initialize ( &OutputSymbol );
	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	if ( StringBuilder_Appendf ( Body, "	%s %s = ", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), OutputSymbol.Data ) == false )
		goto OnError;

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
			break;
		}
	if ( WroteValue == false )
		goto OnError;
	if ( StringBuilder_Append ( Body, ";\n" ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	return Result;
	}

bool AddGLSLBody_TextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[2];
	StringBuilder_Initialize ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	vec4 %s = texture ( %s, %s );\n", OutputSymbol.Data, InputSymbols[0].Data, InputSymbols[1].Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_Multiplication ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[2];
	StringBuilder_Initialize ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	%s %s = %s * %s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), OutputSymbol.Data, InputSymbols[0].Data, InputSymbols[1].Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_Addition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[2];
	StringBuilder_Initialize ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	%s %s = %s + %s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), OutputSymbol.Data, InputSymbols[0].Data, InputSymbols[1].Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < 2; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_Normalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder Input1;
	StringBuilder_Initialize ( &OutputSymbol );
	StringBuilder_Initialize ( &Input1 );
	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	if ( GetInputSymbolName ( &Input1, Node, 0, Stage ) == false )
		goto OnError;
	if ( StringBuilder_Appendf ( Body, "	%s %s = normalize ( %s );\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), OutputSymbol.Data, Input1.Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	StringBuilder_Destroy ( &Input1 );
	return Result;
	}

bool AddGLSLBody_ComposeVec ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[4];
	StringBuilder_Initialize ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	const char *GLSLType = sgValueTypeToGLSL ( Node->Outputs[0].ValueType );
	bool WroteValue = false;
	switch ( Node->InputCount )
		{
		case 2:
			WroteValue = StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s );\n", GLSLType, OutputSymbol.Data, GLSLType, InputSymbols[0].Data, InputSymbols[1].Data );
		case 3:
			WroteValue = StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s, %s );\n", GLSLType, OutputSymbol.Data, GLSLType, InputSymbols[0].Data, InputSymbols[1].Data, InputSymbols[2].Data );
		case 4:
			WroteValue = StringBuilder_Appendf ( Body, "	%s %s = %s ( %s, %s, %s, %s );\n", GLSLType, OutputSymbol.Data, GLSLType, InputSymbols[0].Data, InputSymbols[1].Data, InputSymbols[2].Data, InputSymbols[3].Data );
		}
	if ( WroteValue == false )
		goto OnError;

	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_Swizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbol;
	StringBuilder_Initialize ( &OutputSymbol );
	StringBuilder_Initialize ( &InputSymbol );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	if ( GetInputSymbolName ( &InputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	if ( StringBuilder_Appendf ( Body, "	%s %s = %s.%s;\n", sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), OutputSymbol.Data, InputSymbol.Data, Node->Swizzle ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	StringBuilder_Destroy ( &InputSymbol );
	return Result;
	}

bool AddGLSLBody_Mat3Cast ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbol;
	StringBuilder_Initialize ( &OutputSymbol );
	StringBuilder_Initialize ( &InputSymbol );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	if ( GetInputSymbolName ( &InputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	if ( StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( %s );\n", OutputSymbol.Data, InputSymbol.Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	StringBuilder_Destroy ( &InputSymbol );
	return Result;
	}

bool AddGLSLDeclaration_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( ( Body == NULL ) || ( Node == NULL ) || ( Stage != sgStage_Vertex ) || ( Node->Type != sgNodeType_Skinning ) )
		return false;
	if ( Node->Skinning.BoneCount == 0 )
		{
		LOG_ERROR ( "Skinning node '%s' has a bone count of 0", Node->Name );
		return false;
		}
	return StringBuilder_Appendf ( Body,
	                               "layout ( std140 ) uniform %s\n"
	                               "{\n"
	                               "	mat4 u_BoneMatrices[%u];\n"
	                               "};\n",
	                               Node->Name,
	                               Node->Skinning.BoneCount );
	}

bool AddGLSLBody_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbolPosition, OutputSymbolNormal;
	StringBuilder InputSymbols[SG_MAX_INPUTS];
	StringBuilder_Initialize ( &OutputSymbolPosition );
	StringBuilder_Initialize ( &OutputSymbolNormal );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbolPosition, Node, 0, Stage ) == false )
		goto OnError;
	if ( GetOutputSymbolName ( &OutputSymbolNormal, Node, 1, Stage ) == false )
		goto OnError;

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body,
	                             "	mat4 n%u_Skin =\n"
	                             "		u_BoneMatrices[int ( %s.x )] * %s.x +\n"
	                             "		u_BoneMatrices[int ( %s.y )] * %s.y +\n"
	                             "		u_BoneMatrices[int ( %s.z )] * %s.z +\n"
	                             "		u_BoneMatrices[int ( %s.w )] * %s.w;\n",
	                             Node->NodeID, InputSymbols[2].Data, InputSymbols[3].Data,
	                             InputSymbols[2].Data, InputSymbols[3].Data,
	                             InputSymbols[2].Data, InputSymbols[3].Data,
	                             InputSymbols[2].Data, InputSymbols[3].Data ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	vec3 %s = ( n%u_Skin * vec4 ( %s, 1.0 ) ).xyz;\n", OutputSymbolPosition.Data, Node->NodeID, InputSymbols[0].Data ) == false )
		return false;
	if ( StringBuilder_Appendf ( Body, "	vec3 %s = mat3 ( n%u_Skin ) * %s;\n", OutputSymbolNormal.Data, Node->NodeID, InputSymbols[1].Data ) == false )
		goto OnError;
	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbolPosition );
	StringBuilder_Destroy ( &OutputSymbolNormal );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_TBN ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[SG_MAX_INPUTS];
	StringBuilder_Initialize ( &OutputSymbol );

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	mat3 %s = mat3 ( normalize ( %s ), normalize ( %s ), normalize ( %s ) );\n",
	                             OutputSymbol.Data, InputSymbols[1].Data, InputSymbols[2].Data, InputSymbols[0].Data ) == false )
		goto OnError;

	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_NormalMap ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[SG_MAX_INPUTS];
	StringBuilder_Initialize ( &OutputSymbol );

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	vec3 %s = normalize ( %s * normalize ( %s * 2.0 - 1.0 ) );\n", OutputSymbol.Data, InputSymbols[0].Data, InputSymbols[1].Data ) == false )
		goto OnError;

	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLDeclaration_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	const char *LightDeclarationsGLSL =
#include "LightDeclarations.glsl"
	    ;
	const char *BlinnPhongLightingGLSL =
#include "BlinnPhongLighting.glsl"
	    ;
	UNUSED ( Stage );
	if ( Node->BlinnPhong.LightCount == 0 )
		{
		LOG_ERROR ( "BlinnPhong node '%s' has a light count of 0", Node->Name );
		return false;
		}
	if ( StringBuilder_Appendf ( Body, LightDeclarationsGLSL, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount, Node->BlinnPhong.LightCount ) == false )
		return false;
	return StringBuilder_Append ( Body, BlinnPhongLightingGLSL );
	}

bool AddGLSLBody_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	bool Result = false;
	StringBuilder OutputSymbol;
	StringBuilder InputSymbols[SG_MAX_INPUTS];
	StringBuilder_Initialize ( &OutputSymbol );

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	if ( GetOutputSymbolName ( &OutputSymbol, Node, 0, Stage ) == false )
		goto OnError;

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	vec4 %s = EvaluateLights ( %s, %s, %s, %s, %s, %s );\n",
	                             OutputSymbol.Data,
	                             InputSymbols[0].Data,
	                             InputSymbols[1].Data,
	                             InputSymbols[2].Data,
	                             InputSymbols[3].Data,
	                             InputSymbols[4].Data,
	                             InputSymbols[5].Data ) == false )
		goto OnError;

	Result = true;

OnError:
	StringBuilder_Destroy ( &OutputSymbol );
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_VertexPosition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Vertex )
		return true;

	bool Result = false;
	StringBuilder InputSymbols[SG_MAX_INPUTS];

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	gl_Position = %s;\n", InputSymbols[0].Data ) == false )
		goto OnError;

	Result = true;

OnError:
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}

bool AddGLSLBody_FragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	if ( Stage != sgStage_Fragment )
		return true;

	bool Result = false;
	StringBuilder InputSymbols[SG_MAX_INPUTS];

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		StringBuilder_Initialize ( &InputSymbols[InputIndex] );

	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbolName ( &InputSymbols[InputIndex], Node, InputIndex, Stage ) == false )
			goto OnError;
		}

	if ( StringBuilder_Appendf ( Body, "	FragColor = %s;\n", InputSymbols[0].Data ) == false )
		goto OnError;

	Result = true;

OnError:
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		StringBuilder_Destroy ( &InputSymbols[InputIndex] );
		}
	return Result;
	}
