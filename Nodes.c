#include "ShaderGraphInternal.h"
#include <Platform/Logger.h>
#include <string.h>

static bool IsFloatVector ( const sgValueType Type )
	{
	return ( Type == sgValueType_Float ) || ( Type == sgValueType_Vec2 ) || ( Type == sgValueType_Vec3 ) || ( Type == sgValueType_Vec4 );
	}

static bool IsUnsignedVector ( const sgValueType Type )
	{
	return ( Type == sgValueType_Unsigned ) || ( Type == sgValueType_UVec2 ) || ( Type == sgValueType_UVec3 ) || ( Type == sgValueType_UVec4 );
	}

static bool IsSquareMatrix ( const sgValueType Type )
	{
	return ( Type == sgValueType_Mat3 ) || ( Type == sgValueType_Mat4 );
	}

static bool IsNumericValue ( const sgValueType Type )
	{
	return IsFloatVector ( Type ) || IsUnsignedVector ( Type ) || IsSquareMatrix ( Type );
	}

static sgValueType AdditionOutputType ( const sgValueType A, const sgValueType B )
	{
	if ( ( A == B ) && IsNumericValue ( A ) )
		return A;
	if ( ( A == sgValueType_Float ) && ( IsFloatVector ( B ) || IsSquareMatrix ( B ) ) )
		return B;
	if ( ( B == sgValueType_Float ) && ( IsFloatVector ( A ) || IsSquareMatrix ( A ) ) )
		return A;
	if ( ( A == sgValueType_Unsigned ) && IsUnsignedVector ( B ) )
		return B;
	if ( ( B == sgValueType_Unsigned ) && IsUnsignedVector ( A ) )
		return A;
	return sgValueType_Void;
	}

sgNodeHandle sgAddAddition ( const sgGraphHandle GraphHandle )
	{
	return sgAddAdditionTyped ( GraphHandle, sgValueType_Vec4, sgValueType_Vec4 );
	}

sgNodeHandle sgAddAdditionTyped ( const sgGraphHandle GraphHandle, const sgValueType AType, const sgValueType BType )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	const sgValueType OutType = AdditionOutputType ( AType, BType );
	if ( OutType == sgValueType_Void )
		{
		LOG_ERROR ( "Unsupported addition operand types" );
		return sgNodeHandle_Invalid;
		}
	sgNode *Node = CreateNode ( Graph, "Add", sgNodeType_Add );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "A", AType );
	SetPin ( &Node->Inputs[1], "B", BType );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", OutType );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddAttribute ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
	if ( IsTakenShaderName ( Graph, Name, NULL ) )
		{
		LOG_ERROR ( "Shader name '%s' is already used", Name );
		return sgNodeHandle_Invalid;
		}
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Attribute );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Outputs[0], Name, Type );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddUniform ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
	if ( IsTakenShaderName ( Graph, Name, NULL ) )
		{
		LOG_ERROR ( "Shader name '%s' is already used", Name );
		return sgNodeHandle_Invalid;
		}
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Uniform );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Outputs[0], Name, Type );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddUniformArray ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type, const unsigned Count )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) || ( Count == 0 ) )
		return sgNodeHandle_Invalid;
	if ( IsTakenShaderName ( Graph, Name, NULL ) )
		{
		LOG_ERROR ( "Shader name '%s' is already used", Name );
		return sgNodeHandle_Invalid;
		}
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Uniform );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Index", sgValueType_Unsigned );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], Name, Type );
	Node->OutputCount = 1;
	Node->ArrayCount = Count;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddBlinnPhongNode ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( sgGetNodeCountByType ( GraphHandle, sgNodeType_BlinnPhong ) != 0 )
		{
		LOG_ERROR ( "Only one Blinn Phong node is allowed per graph" );
		return sgNodeHandle_Invalid;
		}
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
	if ( ( LightCount == 0 ) || ( IsValidNode ( GraphHandle, NodeHandle ) == false ) )
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

sgNodeHandle sgAddComposeVector ( const sgGraphHandle GraphHandle, const sgValueType Type, const unsigned ComponentCount )
	{
	if ( ( ComponentCount < 2 ) || ( ComponentCount > 4 ) || ( ( Type != sgValueType_Float ) && ( Type != sgValueType_Unsigned ) ) )
		return sgNodeHandle_Invalid;

	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	char Name[SG_MAX_NODE_NAME_LENGTH];
	snprintf ( Name, sizeof ( Name ), "ComposeVector%u", ComponentCount );

	const sgNodeType NodeType = ( sgNodeType ) ( sgNodeType_ComposeVec2 + ComponentCount - 2 );
	const sgValueType OutType = ( Type == sgValueType_Float ) ? ( sgValueType ) ( sgValueType_Vec2 + ComponentCount - 2 ) : ( sgValueType ) ( sgValueType_UVec2 + ComponentCount - 2 );
	sgNode *Node = CreateNode ( Graph, Name, NodeType );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	const char *Names[4] = { "X", "Y", "Z", "W" };
	for ( unsigned Index = 0; Index < ComponentCount; ++Index )
		SetPin ( &Node->Inputs[Index], Names[Index], Type );

	Node->InputCount = ComponentCount;
	SetPin ( &Node->Outputs[0], "Result", OutType );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddConstant ( const sgGraphHandle GraphHandle, const sgValue Value )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;

	sgNode *Node = CreateNode ( Graph, NULL, sgNodeType_Constant );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	Node->ConstantValue.Type = Value.Type;
	if ( sgSetConstantNodeValue ( GraphHandle, GetHandleFromNode ( Node ), Value ) == false )
		{
		sgRemoveNode ( GraphHandle, GetHandleFromNode ( Node ) );
		return sgNodeHandle_Invalid;
		}
	SetPin ( &Node->Outputs[0], "Value", Value.Type );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddConstantFloat ( const sgGraphHandle GraphHandle, const float Value )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Float = Value;
	NewValue.Type = sgValueType_Float;
	return sgAddConstant ( GraphHandle, NewValue );
	}

sgNodeHandle sgAddConstantUnsigned ( const sgGraphHandle GraphHandle, const unsigned Value )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Unsigned = Value;
	NewValue.Type = sgValueType_Unsigned;
	return sgAddConstant ( GraphHandle, NewValue );
	}

sgNodeHandle sgAddConstantVec2 ( const sgGraphHandle GraphHandle, const float X, const float Y )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Vec2[0] = X;
	NewValue.Vec2[1] = Y;
	NewValue.Type = sgValueType_Vec2;
	return sgAddConstant ( GraphHandle, NewValue );
	}

sgNodeHandle sgAddConstantVec3 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Vec3[0] = X;
	NewValue.Vec3[1] = Y;
	NewValue.Vec3[2] = Z;
	NewValue.Type = sgValueType_Vec3;
	return sgAddConstant ( GraphHandle, NewValue );
	}

sgNodeHandle sgAddConstantVec4 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z, const float W )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Vec4[0] = X;
	NewValue.Vec4[1] = Y;
	NewValue.Vec4[2] = Z;
	NewValue.Vec4[3] = W;
	NewValue.Type = sgValueType_Vec4;
	return sgAddConstant ( GraphHandle, NewValue );
	}

sgNodeHandle sgAddIdentityMat4 ( const sgGraphHandle GraphHandle )
	{
	sgValue NewValue;
	memset ( &NewValue, 0, sizeof ( NewValue ) );
	NewValue.Matrix4[0] = NewValue.Matrix4[5] = NewValue.Matrix4[10] = NewValue.Matrix4[15] = 1.0f;
	NewValue.Type = sgValueType_Mat4;
	return sgAddConstant ( GraphHandle, NewValue );
	}

bool sgSetConstantNodeValue ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const sgValue Value )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;

	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_Constant )
		return false;
	switch ( Value.Type )
		{
		case sgValueType_Float:
			snprintf ( Node->Name, sizeof ( Node->Name ), "%g", Value.Float );
			break;
		case sgValueType_Vec2:
			snprintf ( Node->Name, sizeof ( Node->Name ), "vec2 ( %g, %g )", Value.Vec2[0], Value.Vec2[1] );
			break;
		case sgValueType_Vec3:
			snprintf ( Node->Name, sizeof ( Node->Name ), "vec3 ( %g, %g, %g )", Value.Vec3[0], Value.Vec3[1], Value.Vec3[2] );
			break;
		case sgValueType_Vec4:
			snprintf ( Node->Name, sizeof ( Node->Name ), "vec4 ( %g, %g, %g, %g )", Value.Vec4[0], Value.Vec4[1], Value.Vec4[2], Value.Vec4[3] );
			break;
		case sgValueType_Unsigned:
			snprintf ( Node->Name, sizeof ( Node->Name ), "%uu", Value.Unsigned );
			break;
		case sgValueType_UVec2:
			snprintf ( Node->Name, sizeof ( Node->Name ), "uvec2 ( %uu, %uu )", Value.UVec2[0], Value.UVec2[1] );
			break;
		case sgValueType_UVec3:
			snprintf ( Node->Name, sizeof ( Node->Name ), "uvec3 ( %uu, %uu, %uu )", Value.UVec3[0], Value.UVec3[1], Value.UVec3[2] );
			break;
		case sgValueType_UVec4:
			snprintf ( Node->Name, sizeof ( Node->Name ), "uvec4 ( %uu, %uu, %uu, %uu )", Value.UVec4[0], Value.UVec4[1], Value.UVec4[2], Value.UVec4[3] );
			break;
		case sgValueType_Mat3:
			snprintf ( Node->Name, sizeof ( Node->Name ), "mat3" );
			break;
		case sgValueType_Mat4:
			snprintf ( Node->Name, sizeof ( Node->Name ), "mat4" );
			break;
		default:
			LOG_ERROR ( "Unhandled value type" );
			return false;
		}
	if ( CopyValue ( &Value, &Node->ConstantValue ) == false )
		return false;
	return true;
	}

bool sgGetConstantNodeValue ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, sgValue *Value )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;

	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_Constant )
		return false;
	return CopyValue ( &Node->ConstantValue, Value );
	}

sgNodeHandle sgAddFragmentColor ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( Graph->FragmentColorNode != sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "FragmentColor", sgNodeType_FragmentColor );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Color", sgValueType_Vec4 );
	Node->InputCount = 1;
	Graph->FragmentColorNode = GetHandleFromNode ( Node );
	return Graph->FragmentColorNode;
	}

sgNodeHandle sgAddMat3Cast ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Mat3Cast", sgNodeType_Mat3Cast );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Matrix", sgValueType_Mat4 );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Mat3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

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
	Node->ArrayCount = 50;// Default 50 bones
	return GetHandleFromNode ( Node );
	}

bool sgSetSkinningBoneCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const unsigned BoneCount )
	{
	if ( BoneCount == 0 )
		return false;
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

static sgValueType MultiplyOutputType ( const sgValueType A, const sgValueType B )
	{
	if ( ( A == sgValueType_Mat4 ) && ( B == sgValueType_Mat4 ) )
		return sgValueType_Mat4;
	if ( ( A == sgValueType_Mat4 ) && ( B == sgValueType_Vec4 ) )
		return sgValueType_Vec4;
	if ( ( A == sgValueType_Mat3 ) && ( B == sgValueType_Mat3 ) )
		return sgValueType_Mat3;
	if ( ( A == sgValueType_Mat3 ) && ( B == sgValueType_Vec3 ) )
		return sgValueType_Vec3;
	if ( ( A == sgValueType_Float ) && ( IsFloatVector ( B ) || IsSquareMatrix ( B ) ) )
		return B;
	if ( ( B == sgValueType_Float ) && ( IsFloatVector ( A ) || IsSquareMatrix ( A ) ) )
		return A;
	if ( ( A == B ) && ( IsFloatVector ( A ) || IsUnsignedVector ( A ) ) )
		return A;
	return sgValueType_Void;
	}

sgNodeHandle sgAddMultiplicationTyped ( const sgGraphHandle GraphHandle, const sgValueType AType, const sgValueType BType )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	const sgValueType OutType = MultiplyOutputType ( AType, BType );
	if ( OutType == sgValueType_Void )
		{
		LOG_ERROR ( "Unsupported multiply operand types" );
		return sgNodeHandle_Invalid;
		}
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

sgNodeHandle sgAddMultiplication ( const sgGraphHandle GraphHandle )
	{
	return sgAddMultiplicationTyped ( GraphHandle, sgValueType_Vec4, sgValueType_Vec4 );
	}

sgNodeHandle sgAddNormalize ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Normalize", sgNodeType_Normalize );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Vector", sgValueType_Vec3 );
	Node->InputCount = 1;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddTBN ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "TBN", sgNodeType_TBN );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[1], "Tangent", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[2], "Bitangent", sgValueType_Vec3 );
	Node->InputCount = 3;
	SetPin ( &Node->Outputs[0], "TBN", sgValueType_Mat3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddNormalMap ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "NormalMap", sgNodeType_NormalMap );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "TBN", sgValueType_Mat3 );
	SetPin ( &Node->Inputs[1], "Sample", sgValueType_Vec3 );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Normal", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

static unsigned VectorComponentCount ( const sgValueType Type )
	{
	switch ( Type )
		{
		case sgValueType_Float:
		case sgValueType_Unsigned:
			return 1;
		case sgValueType_Vec2:
		case sgValueType_UVec2:
			return 2;
		case sgValueType_Vec3:
		case sgValueType_UVec3:
			return 3;
		case sgValueType_Vec4:
		case sgValueType_UVec4:
			return 4;
		default:
			return 0;
		}
	}

static bool IsValidSwizzle ( const char *Swizzle, const sgValueType InputType, const sgValueType OutputType )
	{
	const unsigned InputComponents = VectorComponentCount ( InputType );
	const unsigned OutputComponents = VectorComponentCount ( OutputType );
	if ( ( InputComponents == 0 ) || ( OutputComponents == 0 ) )
		return false;
	if ( IsFloatVector ( InputType ) != IsFloatVector ( OutputType ) )
		return false;

	const char *Sets[3] = { "xyzw", "rgba", "stpq" };
	int ActiveSet = -1;
	unsigned Length = 0;
	for ( const char *Cursor = Swizzle; *Cursor != '\0'; ++Cursor )
		{
		if ( Length >= 4 )
			return false;
		int Component = -1;
		int SetIndex = -1;
		for ( int CandidateSet = 0; ( Component < 0 ) && ( CandidateSet < 3 ); ++CandidateSet )
			{
			for ( int Index = 0; Index < 4; ++Index )
				{
				if ( Sets[CandidateSet][Index] == *Cursor )
					{
					Component = Index;
					SetIndex = CandidateSet;
					break;
					}
				}
			}
		if ( Component < 0 )
			return false;
		if ( ( unsigned ) Component >= InputComponents )
			return false;
		if ( ActiveSet < 0 )
			ActiveSet = SetIndex;
		else if ( ActiveSet != SetIndex )
			return false;
		++Length;
		}
	return Length == OutputComponents;
	}

sgNodeHandle sgAddSwizzle ( const sgGraphHandle GraphHandle, const char *Swizzle, const sgValueType InputType, const sgValueType OutputType )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Swizzle == NULL ) || ( Swizzle[0] == '\0' ) ||
	        ( IsValidSwizzle ( Swizzle, InputType, OutputType ) == false ) )
		{
		if ( Graph != NULL )
			LOG_ERROR ( "Invalid swizzle" );
		return sgNodeHandle_Invalid;
		}
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

sgNodeHandle sgAddVertexPosition ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( Graph->VertexPositionNode != sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "VertexPosition", sgNodeType_VertexPosition );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Position", sgValueType_Vec4 );
	Node->InputCount = 1;
	Graph->VertexPositionNode = GetHandleFromNode ( Node );
	return Graph->VertexPositionNode;
	}
