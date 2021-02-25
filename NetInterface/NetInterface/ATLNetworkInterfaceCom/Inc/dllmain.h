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
//   File:             dllmain.h
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#pragma once

class CATLNetworkInterfaceComModule : public ATL::CAtlDllModuleT< CATLNetworkInterfaceComModule >
{
public :
	DECLARE_LIBID(LIBID_ATLNetworkInterfaceComLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLNETWORKINTERFACECOM, "{7CE142DA-5ECC-49F2-97F4-7BAC3264701F}")
};

extern class CATLNetworkInterfaceComModule _AtlModule;
