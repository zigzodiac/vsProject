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
//   File:             NetworkComImpl.h
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#pragma once

#include "resource.h"
#include <basetyps.h>
#include "ATLNetworkInterfaceCom_i.h"
#include "_INetworkComImplEvents_CP.h"
#include "INetworkCallback.h"

using namespace ATL;
class CLogKeeperNetwork;

class ATL_NO_VTABLE CNetworkComImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNetworkComImpl, &CLSID_NetworkComImpl>,
	public IConnectionPointContainerImpl<CNetworkComImpl>,
	public CProxy_INetworkComImplEvents<CNetworkComImpl>,
	public IDispatchImpl<INetworkComImpl, &IID_INetworkComImpl, &LIBID_ATLNetworkInterfaceComLib, 1, 0>,
	public INetworkCallback
{
public:
	CNetworkComImpl();

DECLARE_REGISTRY_RESOURCEID(IDR_NETWORKCOMIMPL)


BEGIN_COM_MAP(CNetworkComImpl)
	COM_INTERFACE_ENTRY(INetworkComImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CNetworkComImpl)
	CONNECTION_POINT_ENTRY(__uuidof(_INetworkComImplEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(SetUrl)(BSTR in_sUrl);	

	STDMETHOD(SetHostInfo)(BSTR in_sIp, USHORT in_usPort);

	STDMETHOD(Open)();

	STDMETHOD(Close)();

	STDMETHOD(SetCurrentTime)(BSTR in_sTimeString);

	STDMETHOD(ProcessClips)(BSTR in_sFileName, BSTR in_sJumpId);

	HRESULT ReceiveClipInfo(string& in_sClipInfo);
	
private:
	CLogKeeperNetwork*		m_pLogKeeperNetwork;
};

OBJECT_ENTRY_AUTO(__uuidof(NetworkComImpl), CNetworkComImpl)
