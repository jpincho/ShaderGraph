#include "Nodes/EmitUtils.h"
#include <Platform/Logger.h>

const char *sgValueTypeToGLSL ( const sgValueType Type )
	{
	switch ( Type )
		{
		case sgValueType_Float: return "float";
		case sgValueType_Vec2: return "vec2";
		case sgValueType_Vec3: return "vec3";
		case sgValueType_Vec4: return "vec4";
		case sgValueType_Unsigned: return "uint";
		case sgValueType_UVec2: return "uvec2";
		case sgValueType_UVec3: return "uvec3";
		case sgValueType_UVec4: return "uvec4";
		case sgValueType_Mat3: return "mat3";
		case sgValueType_Mat4: return "mat4";
		case sgValueType_Sampler2D: return "sampler2D";
		case sgValueType_Void:
		default: return "void";
		}
	}

bool sgGetOutputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage,
                         char *Buffer, const unsigned BufferSize )
	{
	if ( ( Node == NULL ) || ( Buffer == NULL ) || ( BufferSize == 0 ) || ( PinIndex >= Node->OutputCount ) )
		{
		if ( ( Buffer != NULL ) && ( BufferSize > 0 ) )
			Buffer[0] = '\0';
		return false;
		}
	if ( Node->Type == sgNodeType_Attribute )
		{
		if ( ( Stage == sgStage_Fragment ) && Node->UsedInShaderStage[sgStage_Fragment] )
			snprintf ( Buffer, BufferSize, "v_%s", Node->Name );
		else
			snprintf ( Buffer, BufferSize, "%s", Node->Name );
		return true;
		}
	if ( Node->Type == sgNodeType_Uniform )
		{
		snprintf ( Buffer, BufferSize, "%s", Node->Name );
		return true;
		}
	if ( Node->OutputCount > 1 )
		snprintf ( Buffer, BufferSize, "n%u_%s", Node->NodeID, Node->Outputs[PinIndex].PinName );
	else
		snprintf ( Buffer, BufferSize, "n%u", Node->NodeID );
	return true;
	}

bool sgGetInputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage,
                        char *Buffer, const unsigned BufferSize )
	{
	if ( ( Node == NULL ) || ( PinIndex >= Node->InputCount ) )
		return false;
	const sgConnection *Connection = Node->Inputs[PinIndex].InputConnection;
	if ( Connection == NULL )
		return false;
	return sgGetOutputSymbol ( Connection->OutputNode, Connection->OutputPinIndex, Stage, Buffer, BufferSize );
	}

bool sgLoadNodeInputs ( const sgNode *Node, const sgShaderStage Stage, char Inputs[SG_MAX_INPUTS][128] )
	{
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( sgGetInputSymbol ( Node, InputIndex, Stage, Inputs[InputIndex], sizeof ( Inputs[InputIndex] ) ) == false )
			return false;
		}
	return true;
	}
