#include "GeneratorUtils.h"
#include <Platform/Logger.h>
#include <stdio.h>
#include <string.h>

const char *sgValueTypeToGLSL ( const sgValueType Type )
	{
	switch ( Type )
		{
		case sgValueType_Float:
			return "float";
		case sgValueType_Vec2:
			return "vec2";
		case sgValueType_Vec3:
			return "vec3";
		case sgValueType_Vec4:
			return "vec4";
		case sgValueType_Unsigned:
			return "uint";
		case sgValueType_UVec2:
			return "uvec2";
		case sgValueType_UVec3:
			return "uvec3";
		case sgValueType_UVec4:
			return "uvec4";
		case sgValueType_Mat3:
			return "mat3";
		case sgValueType_Mat4:
			return "mat4";
		case sgValueType_Sampler2D:
			return "sampler2D";
		case sgValueType_Void:
		default:
			return "void";
		}
	}

static bool SymbolFits ( char *Buffer, const unsigned BufferSize, const int Written )
	{
	if ( ( Written < 0 ) || ( ( unsigned ) Written >= BufferSize ) )
		{
		if ( BufferSize > 0 )
			Buffer[0] = '\0';
		return false;
		}
	return true;
	}

bool GetOutputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize )
	{
	if ( ( Node == NULL ) || ( Buffer == NULL ) || ( BufferSize == 0 ) || ( PinIndex >= Node->OutputCount ) )
		{
		if ( ( Buffer != NULL ) && ( BufferSize > 0 ) )
			Buffer[0] = '\0';
		return false;
		}
	if ( Node->Type == sgNodeType_Attribute )
		{
		if ( Stage == sgStage_Fragment )
			return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "v_%s", Node->Name ) );
		return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "%s", Node->Name ) );
		}
	if ( Node->Type == sgNodeType_Uniform )
		{
		if ( Node->ArrayCount == 0 )
			return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "%s", Node->Name ) );

		const sgConnection *IndexConnection = ( Node->InputCount > 0 ) ? Node->Inputs[0].InputConnection : NULL;
		if ( ( IndexConnection == NULL ) || ( IndexConnection->OutputNode == NULL ) )
			{
			Buffer[0] = '\0';
			return false;
			}

		unsigned ConstantIndex = IndexConnection->OutputNode->ConstantValue.Unsigned;
		if ( ConstantIndex >= Node->ArrayCount )
			{
			LOG_ERROR ( "Uniform array '%s' index %u is outside the array", Node->Name, ConstantIndex );
			Buffer[0] = '\0';
			return false;
			}
		if ( Node->Outputs[0].ValueType == sgValueType_Sampler2D )
			{
			return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "%s[%u]", Node->Name, ConstantIndex ) );
			}

		char Index[128];
		if ( GetInputSymbol ( Node, 0, Stage, Index, sizeof ( Index ) ) == false )
			{
			Buffer[0] = '\0';
			return false;
			}
		return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "%s[int ( %s )]", Node->Name, Index ) );
		}
	if ( Node->OutputCount > 1 )
		return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "n%u_%s", Node->NodeID, Node->Outputs[PinIndex].PinName ) );
	return SymbolFits ( Buffer, BufferSize, snprintf ( Buffer, BufferSize, "n%u", Node->NodeID ) );
	}

bool GetInputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize )
	{
	if ( ( Node == NULL ) || ( PinIndex >= Node->InputCount ) )
		return false;
	const sgConnection *Connection = Node->Inputs[PinIndex].InputConnection;
	if ( Connection == NULL )
		return false;
	return GetOutputSymbol ( Connection->OutputNode, Connection->OutputPinIndex, Stage, Buffer, BufferSize );
	}

bool LoadNodeInputs ( const sgNode *Node, const sgShaderStage Stage, char Inputs[SG_MAX_INPUTS][128] )
	{
	for ( unsigned InputIndex = 0; InputIndex < Node->InputCount; ++InputIndex )
		{
		if ( GetInputSymbol ( Node, InputIndex, Stage, Inputs[InputIndex], sizeof ( Inputs[InputIndex] ) ) == false )
			return false;
		}
	return true;
	}

bool GenerateVarNameForStage ( const char *Variable, const sgNodeType NodeType, const sgShaderStage Stage, const bool Input, StringBuilder *Output )
	{
	switch ( Stage )
		{
		case sgStage_Vertex:
			{
			if ( ( NodeType == sgNodeType_Attribute ) && ( Input == false ) )
				StringBuilder_Append ( Output, "v_" );
			break;
			}
		case sgStage_Fragment:
			{
			if ( NodeType == sgNodeType_Attribute )
				StringBuilder_Append ( Output, "v_" );
			break;
			}
		}
	StringBuilder_Append ( Output, Variable );
	return true;
	}