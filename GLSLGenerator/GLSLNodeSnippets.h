#pragma once
#include "../StringBuilder.h"
#include "../ShaderGraphInternal.h"

bool AddGLSLBody_Constant ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_TextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Multiplication ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Addition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Normalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_ComposeVec ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Swizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Mat3Cast ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLDeclaration_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_Skinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_TBN ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_NormalMap ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLDeclaration_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_BlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_VertexPosition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool AddGLSLBody_FragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
