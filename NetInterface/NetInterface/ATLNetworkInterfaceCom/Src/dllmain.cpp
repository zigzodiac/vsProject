///==========================================================================
//
// (c) Copyright NewAuto Video Technology CO.,Ltd., 2020. All rights reserved. 
//
// This code and information is provided "as is" without warranty of any kind, 
// either expressed or implied, including but not limited to the implied 
// warranties of merchantability and/or fitness for a particular purpose.
//
//--------------------------------------------------------------------------
//   Birth Date:       NOVEMBER 2020
//   Author:           NewAuto video team
//   Project:          ATLNetworkInterfaceCom
//   File:             dllmain.cpp
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------

#include "stdafx.h"
#include <comsvcs.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include "resource.h"
#include "ATLNetworkInterfaceCom_i.h"
#include "dllmain.h"
#include "compreg.h"

CATLNetworkInterfaceComModule _AtlModule;

// DLL Èë¿Úµã
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
