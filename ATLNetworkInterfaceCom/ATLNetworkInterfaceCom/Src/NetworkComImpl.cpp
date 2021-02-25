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
//   File:             NetworkComImpl.cpp
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "LogKeeperNetwork.h"
#include <comsvcs.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include "NetworkComImpl.h"
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")

CNetworkComImpl::CNetworkComImpl():
	m_pLogKeeperNetwork(NULL)
{
	m_pLogKeeperNetwork = new CLogKeeperNetwork(this);
}

STDMETHODIMP CNetworkComImpl::SetUrl(BSTR in_sUrl)
{
	_bstr_t bstr_t(in_sUrl);
	string str(bstr_t);

	return S_OK;
}


STDMETHODIMP CNetworkComImpl::SetHostInfo(BSTR in_sIp, USHORT in_usPort)
{
	_bstr_t bstr_t(in_sIp);
	string str(bstr_t);
	m_pLogKeeperNetwork->SetHostInfo(str, in_usPort);

	char temp[100];
	sprintf_s(temp, "asio:%s", str.c_str());

	OutputDebugStringA(temp);

	return S_OK;
}


STDMETHODIMP CNetworkComImpl::Open()
{
	m_pLogKeeperNetwork->Open();

	return S_OK;
}


STDMETHODIMP CNetworkComImpl::Close()
{
	m_pLogKeeperNetwork->Close();

	return S_OK;
}


STDMETHODIMP CNetworkComImpl::SetCurrentTime(BSTR in_sTimeString)
{
	_bstr_t bstr_t(in_sTimeString);
	string str(bstr_t);

	m_pLogKeeperNetwork->SetCurrentTime(str);

	return S_OK;
}


STDMETHODIMP CNetworkComImpl::ProcessClips(BSTR in_sFileName, BSTR in_sJumpId)
{
	// TODO: 在此添加实现代码

	return S_OK;
}

HRESULT CNetworkComImpl::ReceiveClipInfo(string& in_sClipInfo)
{
	BSTR sClipInfo = _com_util::ConvertStringToBSTR(in_sClipInfo.c_str());
	Fire_ClipInfoNotify(sClipInfo);
	return S_OK;
}