#pragma once
#include "ShaderGraphInternal.h"

const char *sgValueTypeToGLSL ( const sgValueType Type );
bool GetOutputSymbolName ( StringBuilder *SymbolName, const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage );
bool GetInputSymbolName ( StringBuilder *SymbolName, const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage );
bool GetOutputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );
bool GetInputSymbol ( const sgNode *Node, const unsigned PinIndex, const sgShaderStage Stage, char *Buffer, const unsigned BufferSize );