#include "ShaderGraphInternal.h"
#include <Platform/Logger.h>

sgNodeHandle sgAddAddition ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "Add", sgNodeType_Add );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "A", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[1], "B", sgValueType_Vec4 );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddAttribute ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( ( Graph == NULL ) || ( Name == NULL ) || ( Name[0] == '\0' ) || ( Type == sgValueType_Void ) )
		return sgNodeHandle_Invalid;
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
	sgNode *Node = CreateNode ( Graph, Name, sgNodeType_Uniform );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
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

sgNodeHandle sgAddComposeVec2 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec2", sgNodeType_ComposeVec2 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	Node->InputCount = 2;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec2 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddComposeVec3 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec3", sgNodeType_ComposeVec3 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	SetPin ( &Node->Inputs[2], "Z", sgValueType_Float );
	Node->InputCount = 3;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec3 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

sgNodeHandle sgAddComposeVec4 ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "ComposeVec4", sgNodeType_ComposeVec4 );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "X", sgValueType_Float );
	SetPin ( &Node->Inputs[1], "Y", sgValueType_Float );
	SetPin ( &Node->Inputs[2], "Z", sgValueType_Float );
	SetPin ( &Node->Inputs[3], "W", sgValueType_Float );
	Node->InputCount = 4;
	SetPin ( &Node->Outputs[0], "Result", sgValueType_Vec4 );
	Node->OutputCount = 1;
	return GetHandleFromNode ( Node );
	}

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

sgNodeHandle sgAddMultiplicationTyped ( const sgGraphHandle GraphHandle, const sgValueType AType, const sgValueType BType )
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
