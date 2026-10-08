#include "ShaderGraphInternal.h"

void CopyString ( char *Dest, const unsigned DestSize, const char *Src )
	{
	if ( ( Dest == NULL ) || ( DestSize == 0 ) )
		return;
	if ( Src == NULL )
		{
		Dest[0] = '\0';
		return;
		}
	strncpy ( Dest, Src, DestSize - 1 );
	Dest[DestSize - 1] = '\0';
	}

bool IsValidNode ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle )
	{
	sgGraph *Graph = GetGraphFromHandle ( GraphHandle );
	sgNode *Node = GetNodeFromHandle ( NodeHandle );
	if ( ( Graph == NULL ) || ( Node == NULL ) )
		return false;
	return PointerArray_Find ( &Graph->Nodes, Node ) != -1;
	}

void SetPin ( sgInternalPin *Pin, const char *Name, const sgValueType Type )
	{
	memset ( Pin, 0, sizeof ( *Pin ) );
	CopyString ( Pin->PinName, sizeof ( Pin->PinName ), Name );
	Pin->ValueType = Type;
	}
