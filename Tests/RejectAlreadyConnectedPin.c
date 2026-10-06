#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle A = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	sgNodeHandle B = sgAddConstantVec4 ( Graph, 0.0f, 1.0f, 0.0f, 1.0f );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( A == sgNodeHandle_Invalid ) || ( B == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, A, 0, FragmentColor, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, B, 0, FragmentColor, 0 ) == true )
		{
		fprintf ( stderr, "Second connection to same input should fail\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
