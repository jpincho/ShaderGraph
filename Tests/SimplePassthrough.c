#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle PositionAttributeHandle = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle PositionHandle = sgAddVertexPosition ( Graph );
	if ( ( PositionAttributeHandle == sgNodeHandle_Invalid ) || ( PositionHandle == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, PositionAttributeHandle, 0, PositionHandle, 0 ) == false )
		return 1;

	sgNodeHandle FragmentColorHandle = sgAddFragmentColor ( Graph );
	sgNodeHandle ConstantColorHandle = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( FragmentColorHandle == sgNodeHandle_Invalid ) || ( ConstantColorHandle == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, ConstantColorHandle, 0, FragmentColorHandle, 0 ) == false )
		return 1;

	if ( sgValidateGraph ( Graph ) == false )
		return 1;

	char *VertexShader = NULL;
	char *FragmentShader = NULL;
	if ( sgGenerateGLSL ( Graph, &VertexShader, &FragmentShader ) == false )
		return 1;

	if ( ( strstr ( VertexShader, "gl_Position" ) == NULL ) ||
	        ( strstr ( FragmentShader, "FragColor" ) == NULL ) )
		{
		free ( VertexShader );
		free ( FragmentShader );
		sgDestroy ( Graph );
		return 1;
		}

	printf ( "===== Vertex Shader =====\n%s\n", VertexShader );
	printf ( "===== Fragment Shader =====\n%s\n", FragmentShader );

	free ( VertexShader );
	free ( FragmentShader );
	sgDestroy ( Graph );
	return 0;
	}
