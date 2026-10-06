#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		return 1;
	/* VertexPosition input left unconnected. */
	if ( sgValidateGraph ( Graph ) == true )
		{
		fprintf ( stderr, "Unconnected VertexPosition input should not validate\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
