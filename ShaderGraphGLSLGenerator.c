#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "StringBuilder.h"
#include "Nodes/Nodes.h"
#include <Platform/Logger.h>

static bool NodeUsedInStage ( const sgNode *Node, const sgShaderStage Stage )
	{
	return ( Node != NULL ) && Node->UsedInShaderStage[Stage];
	}

static void ClearStageUsage ( sgGraph *Graph )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		Node->UsedInShaderStage[sgStage_Vertex] = false;
		Node->UsedInShaderStage[sgStage_Fragment] = false;
		}
	}

static void MarkDependencies ( sgNode *Node, const sgShaderStage Stage )
	{
	if ( Node == NULL )
		return;
	if ( Node->UsedInShaderStage[Stage] )
		return;
	Node->UsedInShaderStage[Stage] = true;

	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		{
		const sgConnection *Connection = Node->Inputs[PinIndex].InputConnection;
		if ( Connection != NULL )
			MarkDependencies ( Connection->OutputNode, Stage );
		}
	}

static void ClearTopoColors ( const sgGraph *Graph )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		Node->TopoColor = 0;
		}
	}

static bool TopoVisit ( sgNode *Node, const sgShaderStage Stage, PointerArray *Ordered )
	{
	if ( ( Node == NULL ) || ( NodeUsedInStage ( Node, Stage ) == false ) )
		return true;
	if ( Node->TopoColor == 2 )
		return true;
	if ( Node->TopoColor == 1 )
		{
		LOG_ERROR ( "Cycle detected while ordering node '%s'", Node->Name );
		return false;
		}

	Node->TopoColor = 1;
	for ( unsigned PinIndex = 0; PinIndex < Node->InputCount; ++PinIndex )
		{
		const sgConnection *Connection = Node->Inputs[PinIndex].InputConnection;
		if ( ( Connection != NULL ) && ( TopoVisit ( Connection->OutputNode, Stage, Ordered ) == false ) )
			return false;
		}
	Node->TopoColor = 2;
	return PointerArray_AddAtEnd ( Ordered, Node );
	}

static bool BuildTopoOrder ( const sgGraph *Graph, const sgShaderStage Stage, PointerArray *Ordered )
	{
	ClearTopoColors ( Graph );

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( TopoVisit ( Node, Stage, Ordered ) == false )
			return false;
		}
	return true;
	}

static bool EmitNodeStatement ( StringBuilder *Body, const sgNode *Node, const sgShaderStage Stage )
	{
	switch ( Node->Type )
		{
		case sgNodeType_Attribute:
			return sgEmitGLSLAttribute ( Body, Node, Stage );
		case sgNodeType_Uniform:
			return sgEmitGLSLUniform ( Body, Node, Stage );
		case sgNodeType_Constant:
			return sgEmitGLSLConstant ( Body, Node, Stage );
		case sgNodeType_TextureSample:
			return sgEmitGLSLTextureSample ( Body, Node, Stage );
		case sgNodeType_Multiply:
			return sgEmitGLSLMultiply ( Body, Node, Stage );
		case sgNodeType_Add:
			return sgEmitGLSLAddition ( Body, Node, Stage );
		case sgNodeType_Normalize:
			return sgEmitGLSLNormalize ( Body, Node, Stage );
		case sgNodeType_ComposeVec2:
			return sgEmitGLSLComposeVec2 ( Body, Node, Stage );
		case sgNodeType_ComposeVec3:
			return sgEmitGLSLComposeVec3 ( Body, Node, Stage );
		case sgNodeType_ComposeVec4:
			return sgEmitGLSLComposeVec4 ( Body, Node, Stage );
		case sgNodeType_Swizzle:
			return sgEmitGLSLSwizzle ( Body, Node, Stage );
		case sgNodeType_Mat3Cast:
			return sgEmitGLSLMat3Cast ( Body, Node, Stage );
		case sgNodeType_Skinning:
			return sgEmitGLSLSkinning ( Body, Node, Stage );
		case sgNodeType_TBN:
			return sgEmitGLSLTBN ( Body, Node, Stage );
		case sgNodeType_NormalMap:
			return sgEmitGLSLNormalMap ( Body, Node, Stage );
		case sgNodeType_BlinnPhong:
			return sgEmitGLSLBlinnPhong ( Body, Node, Stage );
		case sgNodeType_VertexPosition:
			return sgEmitGLSLVertexPosition ( Body, Node, Stage );
		case sgNodeType_FragmentColor:
			return sgEmitGLSLFragmentColor ( Body, Node, Stage );
		case sgNodeType_Invalid:
		default:
			LOG_ERROR ( "Unsupported node type '%s'", sgStringifyNodeType ( Node->Type ) );
			return false;
		}
	}

static bool AppendDeclarations ( StringBuilder *Header, const sgGraph *Graph, const sgShaderStage Stage )
	{
	unsigned AttributeLocation = 0;
	bool NeedsBones = false;

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		const sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( NodeUsedInStage ( Node, Stage ) == false )
			continue;

		if ( Node->Type == sgNodeType_Attribute )
			{
			if ( Stage == sgStage_Vertex )
				{
				if ( StringBuilder_Appendf ( Header, "layout ( location = %u ) in %s %s;\n",
				                             AttributeLocation, sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Node->Name ) == false )
					return false;
				++AttributeLocation;
				if ( Node->UsedInShaderStage[sgStage_Fragment] )
					{
					if ( StringBuilder_Appendf ( Header, "out %s v_%s;\n",
					                             sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Node->Name ) == false )
						return false;
					}
				}
			else if ( Node->UsedInShaderStage[sgStage_Fragment] )
				{
				if ( StringBuilder_Appendf ( Header, "in %s v_%s;\n",
				                             sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Node->Name ) == false )
					return false;
				}
			}
		else if ( Node->Type == sgNodeType_Uniform )
			{
			if ( Node->ArrayCount > 0 )
				{
				if ( StringBuilder_Appendf ( Header, "uniform %s %s[%u];\n",
				                             sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Node->Name, Node->ArrayCount ) == false )
					return false;
				}
			else
				{
				if ( StringBuilder_Appendf ( Header, "uniform %s %s;\n",
				                             sgValueTypeToGLSL ( Node->Outputs[0].ValueType ), Node->Name ) == false )
					return false;
				}
			}
		else if ( Node->Type == sgNodeType_Skinning )
			{
			NeedsBones = true;
			}
		}

	if ( NeedsBones && ( Stage == sgStage_Vertex ) )
		{
		if ( StringBuilder_Append ( Header, "uniform mat4 uBoneMatrices[128];\n" ) == false )
			return false;
		}

	if ( Stage == sgStage_Fragment )
		{
		if ( StringBuilder_Append ( Header, "out vec4 FragColor;\n" ) == false )
			return false;
		}

	return true;
	}

static bool AppendVaryingPassThrough ( StringBuilder *Body, const sgGraph *Graph )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		const sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( Node->Type != sgNodeType_Attribute )
			continue;
		if ( Node->UsedInShaderStage[sgStage_Fragment] == false )
			continue;
		if ( StringBuilder_Appendf ( Body, "\tv_%s = %s;\n", Node->Name, Node->Name ) == false )
			return false;
		}
	return true;
	}

static char *BuildShaderSource ( const sgGraph *Graph, const sgShaderStage Stage )
	{
	PointerArray Ordered;
	PointerArray_Initialize ( &Ordered );
	if ( BuildTopoOrder ( Graph, Stage, &Ordered ) == false )
		{
		PointerArray_Destroy ( &Ordered );
		return NULL;
		}

	StringBuilder Source = {0};

	if ( ( StringBuilder_Append ( &Source, "#version 330 core\n\n" ) == false ) ||
	        ( AppendDeclarations ( &Source, Graph, Stage ) == false ) ||
	        ( StringBuilder_Append ( &Source, "\nvoid main ( )\n{\n" ) == false ) )
		goto OnError;

	for ( unsigned Index = 0; Index < Ordered.Count; ++Index )
		{
		const sgNode *Node = PointerArray_Get ( &Ordered, Index );
		if ( NodeUsedInStage ( Node, Stage ) == false )
			continue;
		if ( EmitNodeStatement ( &Source, Node, Stage ) == false )
			goto OnError;
		}

	if ( Stage == sgStage_Vertex )
		{
		if ( AppendVaryingPassThrough ( &Source, Graph ) == false )
			goto OnError;
		}

	if ( StringBuilder_Append ( &Source, "}\n" ) == false )
		goto OnError;

	PointerArray_Destroy ( &Ordered );
	return StringBuilder_Take ( &Source );

OnError:
	PointerArray_Destroy ( &Ordered );
	StringBuilder_Destroy ( &Source );
	return NULL;
	}

bool sgGenerateGLSL ( const sgGraphHandle GraphHandle, char **OutVertexShader, char **OutFragmentShader )
	{
	if ( ( OutVertexShader == NULL ) || ( OutFragmentShader == NULL ) )
		return false;
	*OutVertexShader = NULL;
	*OutFragmentShader = NULL;

	if ( sgValidateGraph ( GraphHandle ) == false )
		return false;

	sgGraph *Graph = sgGraphFromHandle ( GraphHandle );
	ClearStageUsage ( Graph );
	MarkDependencies ( sgNodeFromHandle ( Graph->VertexPositionNode ), sgStage_Vertex );
	MarkDependencies ( sgNodeFromHandle ( Graph->FragmentColorNode ), sgStage_Fragment );

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = PointerArray_Get ( &Graph->Nodes, Index );
		if ( ( Node->Type == sgNodeType_Attribute ) && Node->UsedInShaderStage[sgStage_Fragment] )
			Node->UsedInShaderStage[sgStage_Vertex] = true;
		}

	char *VertexShader = BuildShaderSource ( Graph, sgStage_Vertex );
	char *FragmentShader = BuildShaderSource ( Graph, sgStage_Fragment );

	ClearStageUsage ( Graph );
	ClearTopoColors ( Graph );

	if ( ( VertexShader == NULL ) || ( FragmentShader == NULL ) )
		{
		free ( VertexShader );
		free ( FragmentShader );
		return false;
		}

	*OutVertexShader = VertexShader;
	*OutFragmentShader = FragmentShader;
	return true;
	}
