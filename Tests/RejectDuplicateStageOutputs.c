#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle FirstVertex = sgAddVertexPosition ( Graph );
	sgNodeHandle SecondVertex = sgAddVertexPosition ( Graph );
	sgNodeHandle FirstFragment = sgAddFragmentColor ( Graph );
	sgNodeHandle SecondFragment = sgAddFragmentColor ( Graph );
	if ( FirstVertex == sgNodeHandle_Invalid || FirstFragment == sgNodeHandle_Invalid )
		return 1;
	if ( SecondVertex != sgNodeHandle_Invalid )
		{
		fprintf ( stderr, "Second VertexPosition should be rejected\n" );
		sgDestroy ( Graph );
		return 1;
		}
	if ( SecondFragment != sgNodeHandle_Invalid )
		{
		fprintf ( stderr, "Second FragmentColor should be rejected\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
