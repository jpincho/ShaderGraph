#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle ColorAttr = sgAddAttribute ( Graph, "Color", sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( ColorAttr == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, ColorAttr, 0, FragmentColor, 0 ) == false )
		return 1;

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return 1;
	if ( ( strstr ( VS, "out vec4 v_Color" ) == NULL ) || ( strstr ( VS, "v_Color = Color" ) == NULL ) )
		{
		fprintf ( stderr, "Vertex shader missing color varying pass-through\n%s\n", VS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}
	if ( strstr ( FS, "in vec4 v_Color" ) == NULL || strstr ( FS, "FragColor = v_Color" ) == NULL )
		{
		fprintf ( stderr, "Fragment shader missing color varying usage\n%s\n", FS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
