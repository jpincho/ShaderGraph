#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle A = sgAddMultiply ( Graph );
	sgNodeHandle B = sgAddAddition ( Graph );
	if ( ( A == sgNodeHandle_Invalid ) || ( B == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, A, 0, B, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, B, 0, A, 0 ) == true )
		{
		fprintf ( stderr, "Expected cycle connection to fail\n" );
		sgDestroy ( Graph );
		return 1;
		}
	if ( sgAddConnection ( Graph, A, 0, A, 0 ) == true )
		{
		fprintf ( stderr, "Expected self-connection to fail\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
