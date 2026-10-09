#pragma once
#include "ShaderGraphInternal.h"

const char *sgValueTypeToGLSL ( const sgValueType Type );
bool sgGetOutputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );
bool sgGetInputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );
bool sgLoadNodeInputs ( const sgNode *Node, const sgShaderStage Stage, char Inputs[SG_MAX_INPUTS][128] );
bool GenerateVarNameForStage ( const char *Variable, const sgNodeType NodeType, const sgShaderStage Stage, const bool Input, StringBuilder *Output );