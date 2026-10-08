#include "ShaderGraphInternal.h"

sgNodeHandle sgAddFragmentColor ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( Graph->FragmentColorNode != sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	sgNode *Node = CreateNode ( Graph, "FragmentColor", sgNodeType_FragmentColor );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Color", sgValueType_Vec4 );
	Node->InputCount = 1;
	Graph->FragmentColorNode = GetHandleFromNode ( Node );
	return Graph->FragmentColorNode;
	}