#pragma once
#include "ShaderGraphInternal.h"
#include "StringBuilder.h"

const char *sgValueTypeToGLSL ( const sgValueType Type );
bool sgGetOutputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );
bool sgGetInputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );
bool sgLoadNodeInputs ( const sgNode *Node, const sgShaderStage Stage, char Inputs[SG_MAX_INPUTS][128] );
