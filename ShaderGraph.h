#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef void *sgNodeHandle;
typedef void *sgGraphHandle;

#define sgGraphHandle_Invalid NULL
#define sgNodeHandle_Invalid NULL

typedef enum
	{
	sgNodeType_Invalid = -1,
	sgNodeType_Attribute = 0,
	sgNodeType_Uniform,
	sgNodeType_Constant,
	sgNodeType_TextureSample,
	sgNodeType_Multiply,
	sgNodeType_Add,
	sgNodeType_Normalize,
	sgNodeType_ComposeVec2,
	sgNodeType_ComposeVec3,
	sgNodeType_ComposeVec4,
	sgNodeType_Swizzle,
	sgNodeType_Mat3Cast,
	sgNodeType_Skinning,
	sgNodeType_TBN,
	sgNodeType_NormalMap,
	sgNodeType_BlinnPhong,
	sgNodeType_VertexPosition,
	sgNodeType_FragmentColor
	} sgNodeType;


typedef enum
	{
	sgValueType_Float = 0,
	sgValueType_Vec2,
	sgValueType_Vec3,
	sgValueType_Vec4,
	sgValueType_Unsigned,
	sgValueType_UVec2,
	sgValueType_UVec3,
	sgValueType_UVec4,
	sgValueType_Mat3,
	sgValueType_Mat4,
	sgValueType_Sampler2D,
	sgValueType_Void
	} sgValueType;

sgGraphHandle sgCreate ( void );
void sgClear ( const sgGraphHandle GraphHandle );
void sgDestroy ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddAttribute ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type );
sgNodeHandle sgAddUniform ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type );
sgNodeHandle sgAddUniformArray ( const sgGraphHandle GraphHandle, const char *Name, const sgValueType Type, const unsigned Count );

sgNodeHandle sgAddConstantFloat ( const sgGraphHandle GraphHandle, const float Value );
sgNodeHandle sgAddConstantVec2 ( const sgGraphHandle GraphHandle, const float X, const float Y );
sgNodeHandle sgAddConstantVec3 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z );
sgNodeHandle sgAddConstantVec4 ( const sgGraphHandle GraphHandle, const float X, const float Y, const float Z, const float W );

sgNodeHandle sgAddIdentityMat4 ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddTextureSample ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddMultiply ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddAddition ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddNormalize ( const sgGraphHandle GraphHandle );

sgNodeHandle sgAddComposeVec2 ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddComposeVec3 ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddComposeVec4 ( const sgGraphHandle GraphHandle );

sgNodeHandle sgAddSwizzle ( const sgGraphHandle GraphHandle, const char *Swizzle, const sgValueType InputType, const sgValueType OutputType );
sgNodeHandle sgAddMat3Cast ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddSkinning ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddTBN ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddNormalMap ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddBlinnPhong ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddVertexPosition ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddFragmentColor ( const sgGraphHandle GraphHandle );
bool sgRemoveNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );
unsigned sgGetNodeCount ( const sgGraphHandle GraphHandle );
sgNodeHandle sgFindNodeByIndex ( const sgGraphHandle GraphHandle, const unsigned Index );
sgNodeHandle sgFindNodeById ( const sgGraphHandle GraphHandle, const unsigned ID );
unsigned sgGetNodeId ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );
sgNodeType sgGetNodeType ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );
const char *sgGetNodeName ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );
bool sgAddConnection ( const sgGraphHandle GraphHandle, const sgNodeHandle FromNodeHandle, const unsigned SourcePinIndex, const sgNodeHandle ToNodeHandle, const unsigned TargetPinIndex );
bool sgRemoveConnection ( const sgGraphHandle GraphHandle, const sgNodeHandle FromNodeHandle, const unsigned SourcePinIndex, const sgNodeHandle ToNodeHandle, const unsigned TargetPinIndex );
bool sgValidateGraph ( const sgGraphHandle GraphHandle );

/* Allocates null-terminated GLSL 330 sources. Caller frees both strings with free(). */
bool sgGenerateGLSL ( const sgGraphHandle GraphHandle, char **OutVertexShader, char **OutFragmentShader );

const char *sgStringifyNodeType ( const sgNodeType Value );
const char *sgStringifyValueType ( const sgValueType Value );
