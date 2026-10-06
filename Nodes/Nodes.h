#pragma once
#include "Nodes/EmitUtils.h"

bool sgEmitGLSLAttribute ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLUniform ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLConstant ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLTextureSample ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLMultiply ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLAddition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLNormalize ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLComposeVec2 ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLComposeVec3 ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLComposeVec4 ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLSwizzle ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLMat3Cast ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLSkinning ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLTBN ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLNormalMap ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLBlinnPhong ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLVertexPosition ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
bool sgEmitGLSLFragmentColor ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage );
