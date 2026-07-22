// intrptw.hpp

#ifndef INTRPTW_HPP
#define INTRPTW_HPP

#if !defined(CUSTMSYS_HPP)
#include "custmsys.hpp"
#endif

typedef void (ServerFunc)();

Boolean	InstallServer(ServerFunc* proc, int freqMSec);
Boolean	DisposeServer(ServerFunc*);
void	CALLBACK TimeFunc( UINT, UINT, DWORD, DWORD, DWORD );

#endif
