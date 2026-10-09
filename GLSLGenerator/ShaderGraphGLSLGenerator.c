#include "ShaderGraph.h"
#include "ShaderGraphInternal.h"
#include "StringBuilder.h"
#include <Platform/Logger.h>
#include "NodeSnippets.h"
#include "GeneratorUtils.h"

static bool NodeUsedInStage ( const sgNode *Node, const sgShaderStage Stage )
	{
	return ( Node != NULL ) && Node->UsedInShaderStage[Stage];
	}

static void ClearStageUsage ( sgGraph *Graph )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
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
		sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
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
		sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
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
		case sgNodeType_Uniform:
			return true;
		case sgNodeType_Constant:
			return AddGLSLBody_Constant ( Body, Node, Stage );
		case sgNodeType_TextureSample:
			return AddGLSLBody_TextureSample ( Body, Node, Stage );
		case sgNodeType_Multiply:
			return AddGLSLBody_Multiplication ( Body, Node, Stage );
		case sgNodeType_Add:
			return AddGLSLBody_Addition ( Body, Node, Stage );
		case sgNodeType_Normalize:
			return AddGLSLBody_Normalize ( Body, Node, Stage );
		case sgNodeType_ComposeVec2:
		case sgNodeType_ComposeVec3:
		case sgNodeType_ComposeVec4:
			return AddGLSLBody_ComposeVec ( Body, Node, Stage );
		case sgNodeType_Swizzle:
			return AddGLSLBody_Swizzle ( Body, Node, Stage );
		case sgNodeType_Mat3Cast:
			return AddGLSLBody_Mat3Cast ( Body, Node, Stage );
		case sgNodeType_Skinning:
			return AddGLSLBody_Skinning ( Body, Node, Stage );
		case sgNodeType_TBN:
			return AddGLSLBody_TBN ( Body, Node, Stage );
		case sgNodeType_NormalMap:
			return AddGLSLBody_NormalMap ( Body, Node, Stage );
		case sgNodeType_BlinnPhong:
			return AddGLSLBody_BlinnPhong ( Body, Node, Stage );
		case sgNodeType_VertexPosition:
			return AddGLSLBody_VertexPosition ( Body, Node, Stage );
		case sgNodeType_FragmentColor:
			return AddGLSLBody_FragmentColor ( Body, Node, Stage );
		case sgNodeType_Invalid:
			LOG_ERROR ( "Unsupported node type '%s'", sgStringifyNodeType ( Node->Type ) );
			return false;
		}
	return false;
	}

static sgNode *GetFirstNodeOfTypeInStage ( const sgGraph *Graph, const sgNodeType Type, const sgShaderStage Stage )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
		if ( ( Node->Type == Type ) && NodeUsedInStage ( Node, Stage ) )
			return Node;
		}
	return NULL;
	}

static bool AppendDeclarations ( StringBuilder *Header, const sgGraph *Graph, const sgShaderStage Stage )
	{
	unsigned AttributeLocation = 0;
	const sgNode *SkinningNode = NULL;

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		const sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
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
			SkinningNode = Node;
			}
		}

	if ( ( SkinningNode != NULL ) && ( Stage == sgStage_Vertex ) )
		{
		unsigned BoneCount = sgGetSkinningBoneCount ( GetHandleFromGraph ( Graph ), GetHandleFromNode ( SkinningNode ) );
		if ( StringBuilder_Appendf ( Header, "uniform mat4 uBoneMatrices[%u];\n", BoneCount ) == false )
			return false;
		}

	if ( Stage == sgStage_Fragment )
		{
		if ( StringBuilder_Append ( Header, "out vec4 FragColor;\n" ) == false )
			return false;
		sgNode *BlinnPhongNode = GetFirstNodeOfTypeInStage ( Graph, sgNodeType_BlinnPhong, sgStage_Fragment );
		if ( BlinnPhongNode != NULL )
			{
			if ( StringBuilder_Append ( Header, "\n" ) == false )
				return false;
			if ( AddGLSLDeclaration_BlinnPhong ( Header, BlinnPhongNode, sgStage_Fragment ) == false )
				return false;
			}
		}

	return true;
	}

static bool AppendVaryingPassThrough ( StringBuilder *Body, const sgGraph *Graph )
	{
	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		const sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
		if ( Node->Type != sgNodeType_Attribute )
			continue;
		if ( Node->UsedInShaderStage[sgStage_Fragment] == false )
			continue;
		if ( StringBuilder_Appendf ( Body, "\tv_%s = %s;\n", Node->Name, Node->Name ) == false )
			return false;
		}
	return true;
	}

static char *BuildVertexShaderSource ( const sgGraph *Graph )
	{
	PointerArray Ordered;
	PointerArray_Initialize ( &Ordered );
	if ( BuildTopoOrder ( Graph, sgStage_Vertex, &Ordered ) == false )
		{
		PointerArray_Destroy ( &Ordered );
		return NULL;
		}

	StringBuilder Source = {0};

	if ( ( StringBuilder_Append ( &Source, "#version 330 core\n\n" ) == false ) ||
	        ( AppendDeclarations ( &Source, Graph, sgStage_Vertex ) == false ) ||
	        ( StringBuilder_Append ( &Source, "\nvoid main ( )\n{\n" ) == false ) )
		goto OnError;

	for ( unsigned Index = 0; Index < Ordered.Count; ++Index )
		{
		const sgNode *Node = ( sgNode* ) PointerArray_Get ( &Ordered, Index );
		if ( NodeUsedInStage ( Node, sgStage_Vertex ) == false )
			continue;
		if ( EmitNodeStatement ( &Source, Node, sgStage_Vertex ) == false )
			goto OnError;
		}

	if ( AppendVaryingPassThrough ( &Source, Graph ) == false )
		goto OnError;

	if ( StringBuilder_Append ( &Source, "}\n" ) == false )
		goto OnError;

	PointerArray_Destroy ( &Ordered );
	return StringBuilder_Take ( &Source );

OnError:
	PointerArray_Destroy ( &Ordered );
	StringBuilder_Destroy ( &Source );
	return NULL;
	}


static char *BuildFragmentShaderSource ( const sgGraph *Graph )
	{
	PointerArray Ordered;
	PointerArray_Initialize ( &Ordered );
	if ( BuildTopoOrder ( Graph, sgStage_Fragment, &Ordered ) == false )
		{
		PointerArray_Destroy ( &Ordered );
		return NULL;
		}

	StringBuilder Source = {0};

	if ( ( StringBuilder_Append ( &Source, "#version 330 core\n\n" ) == false ) ||
	        ( AppendDeclarations ( &Source, Graph, sgStage_Fragment ) == false ) ||
	        ( StringBuilder_Append ( &Source, "\nvoid main ( )\n{\n" ) == false ) )
		goto OnError;

	for ( unsigned Index = 0; Index < Ordered.Count; ++Index )
		{
		const sgNode *Node = ( sgNode* ) PointerArray_Get ( &Ordered, Index );
		if ( NodeUsedInStage ( Node, sgStage_Fragment ) == false )
			continue;
		if ( EmitNodeStatement ( &Source, Node, sgStage_Fragment ) == false )
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

	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	ClearStageUsage ( Graph );
	MarkDependencies ( GetNodeFromHandle ( Graph->VertexPositionNode ), sgStage_Vertex );
	MarkDependencies ( GetNodeFromHandle ( Graph->FragmentColorNode ), sgStage_Fragment );

	for ( unsigned Index = 0; Index < Graph->Nodes.Count; ++Index )
		{
		sgNode *Node = ( sgNode* ) PointerArray_Get ( &Graph->Nodes, Index );
		if ( ( Node->Type == sgNodeType_Attribute ) && Node->UsedInShaderStage[sgStage_Fragment] )
			Node->UsedInShaderStage[sgStage_Vertex] = true;
		}

	char *VertexShader = BuildVertexShaderSource ( Graph );
	char *FragmentShader = BuildFragmentShaderSource ( Graph );

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
