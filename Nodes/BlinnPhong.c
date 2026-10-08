#include "BlinnPhong.h"
#include "ShaderGraphInternal.h"
#include <Platform/Logger.h>

sgNodeHandle sgAddBlinnPhongNode ( const sgGraphHandle GraphHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	if ( Graph == NULL )
		return sgNodeHandle_Invalid;
	if ( sgGetNodeCountByType ( GraphHandle, sgNodeType_BlinnPhong ) != 0 )
		{
		LOG_ERROR ( "Only one Blinn Phong node is allowed per graph" );
		return sgNodeHandle_Invalid;
		}
	sgNode *Node = CreateNode ( Graph, "BlinnPhong", sgNodeType_BlinnPhong );
	if ( Node == NULL )
		return sgNodeHandle_Invalid;
	SetPin ( &Node->Inputs[0], "Diffuse", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[1], "Specular", sgValueType_Vec4 );
	SetPin ( &Node->Inputs[2], "Shininess", sgValueType_Float );
	SetPin ( &Node->Inputs[3], "Normal", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[4], "WorldPosition", sgValueType_Vec3 );
	SetPin ( &Node->Inputs[5], "CameraPosition", sgValueType_Vec3 );
	Node->InputCount = 6;
	SetPin ( &Node->Outputs[0], "Color", sgValueType_Vec4 );
	Node->OutputCount = 1;
	Node->BlinnPhong.LightCount = 8;
	return GetHandleFromNode ( Node );
	}

bool sgSetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const unsigned LightCount )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return false;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_BlinnPhong )
		return false;
	Node->BlinnPhong.LightCount = LightCount;
	return true;
	}

unsigned sgGetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	if ( IsValidNode ( GraphHandle, NodeHandle ) == false )
		return 0;
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( Node->Type != sgNodeType_BlinnPhong )
		return 0;
	return Node->BlinnPhong.LightCount;
	}
