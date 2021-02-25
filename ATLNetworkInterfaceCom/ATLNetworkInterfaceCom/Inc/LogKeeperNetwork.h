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
//   File:             LogKeeperNetwork.h
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#pragma once

#include "NetworkApi.h"
#include <basetyps.h>
#include "INetworkCallback.h"
#include "CommunicationOperationImp.h"

using boost::asio::ip::tcp;

class CLogKeeperNetwork
{

public:
	CLogKeeperNetwork(INetworkCallback *in_pCallback);
	~CLogKeeperNetwork();
public:
	HRESULT SetHostInfo(string in_sIp, USHORT in_usPort);

	HRESULT Open();

	HRESULT Close();

	HRESULT SetCurrentTime(string& in_sTime);

	HRESULT GetHost(string& out_sHost);

	HRESULT GetPort(USHORT& out_usPort);

	HRESULT GetIOContext(io_context** out_pioContext);

	tcp::socket* Socket();

	HRESULT ReceiveMessage(string& message);

	HRESULT SendMessage(string& message);

	HRESULT GetMessage(string& message);

	bool IsThreadExit();
private:

	static HRESULT ThreadCommunicationFunction(CLogKeeperNetwork *in_pLogKeeper);

private:
	INetworkCallback*				m_pCallback;
	io_context						m_io_context;
	string							m_sIp;
	USHORT							m_usPort;
	tcp::socket*					m_pSocket;
	CommunicationOperationImp*		m_pCommunication;
	std::thread*					m_pCommunicationThread;
	string							m_sMessage;
	bool							m_bThreadExit = false;
};