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
//   File:             INetworkCallback.h
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#pragma once

interface INetworkCallback
{
	virtual HRESULT ReceiveClipInfo(string& in_sClipInfo) = 0;
};