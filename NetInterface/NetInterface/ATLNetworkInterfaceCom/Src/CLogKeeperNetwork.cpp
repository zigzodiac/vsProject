///==========================================================================
//
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
//   File:             CLogKeeperNetwork.cpp
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------

#include "stdafx.h"
#include <chrono>
#include "LogKeeperNetwork.h"

using namespace std;
using namespace std::chrono;

#define DATA_SEND_TICK 500


CLogKeeperNetwork::CLogKeeperNetwork(INetworkCallback *in_pCallback) :
	m_pCommunication(NULL),
	m_pSocket(NULL),
	m_pCommunicationThread(NULL),
	m_pCallback(in_pCallback)
{

}

CLogKeeperNetwork::~CLogKeeperNetwork()
{
	delete m_pCommunicationThread;	m_pCommunicationThread = NULL;
	delete m_pCommunication;		m_pCommunication = NULL;
	delete m_pSocket;				m_pSocket = NULL;
	m_pCallback = NULL;
}

HRESULT CLogKeeperNetwork::SetHostInfo(string in_sIp, USHORT in_usPort)
{
	m_sIp = in_sIp;
	m_usPort = in_usPort;

	return S_OK;
}

HRESULT CLogKeeperNetwork::GetHost(string& out_sHost)
{
	out_sHost = m_sIp;

	return S_OK;
}

HRESULT CLogKeeperNetwork::GetPort(USHORT& out_usPort)
{
	out_usPort = m_usPort;

	return S_OK;
}

HRESULT CLogKeeperNetwork::GetIOContext(io_context** out_pioContext)
{
	*out_pioContext = &m_io_context;

	return S_OK;
}

tcp::socket* CLogKeeperNetwork::Socket()
{
	return m_pSocket;
}

HRESULT CLogKeeperNetwork::ReceiveMessage(string& message)
{
	m_pCallback->ReceiveClipInfo(message);

	return S_OK;
}

HRESULT CLogKeeperNetwork::SendMessage(string& message)
{
	m_sMessage = message;

	return S_OK;
}

HRESULT CLogKeeperNetwork::GetMessage(string& message)
{
	message = m_sMessage;

	return S_OK;
}

bool CLogKeeperNetwork::IsThreadExit()
{
	return m_bThreadExit;
}

HRESULT CLogKeeperNetwork::Open()
{
	HRESULT hr = S_OK;

	try
	{
		tcp::resolver resolverObj(m_io_context);
		tcp::resolver::results_type endpoints =
			resolverObj.resolve(tcp::v4(), m_sIp.c_str(), std::to_string(m_usPort).c_str());

		m_pSocket = new tcp::socket(m_io_context);

		connect(*m_pSocket, endpoints);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		hr = E_FAIL;
	}

	if (hr == S_OK)
		m_pSocket->non_blocking(true);

	m_pCommunication = new CommunicationOperationImp(m_pSocket);

	m_pCommunicationThread = new std::thread(&CLogKeeperNetwork::ThreadCommunicationFunction, this);

	//std::timed_mutex mutex1;
	//std::condition_variable s;
	////std::lock_guard<std::mutex> lck(_qDatasMtx);
	return hr;
}

HRESULT CLogKeeperNetwork::Close()
{
	m_bThreadExit = true;
	m_pCommunicationThread->join();
	m_pSocket->close();

	return S_OK;
}

HRESULT CLogKeeperNetwork::SetCurrentTime(string& in_sTime)
{
	m_sMessage = in_sTime;

	return S_OK;
}

HRESULT CLogKeeperNetwork::ThreadCommunicationFunction(CLogKeeperNetwork *in_pLogKeeper)
{
	bool read_in_progress_ = false;
	bool write_in_progress_ = false;

	HRESULT hr = S_OK;
	boost::system::error_code error;

	tcp::socket *pSocket = in_pLogKeeper->Socket();
	CommunicationOperationImp operation(pSocket);

	auto start = high_resolution_clock::now();
	auto end = start;
	duration<double, std::nano> fp_nanos = end - start;

	for (;!in_pLogKeeper->IsThreadExit();)
	{
		if (!read_in_progress_)
		{
			read_in_progress_ = true;
			write_in_progress_ = false;

			hr = S_OK;

			try
			{
				pSocket->wait(tcp::socket::wait_read);
			}
			catch (...)
			{
				//std::cerr << e.what() << "\n";
				hr = E_FAIL;
			}

			if (hr == S_OK)
			{
				string msg;
				operation.Receive(msg, error);

				if (error.value() < 0)
				{
					std::cerr << error<< "\n";
					continue;;
				}


				in_pLogKeeper->ReceiveMessage(msg);

				std::cerr << "receive message. " << msg << "\n";
			}

			continue;
		}

		if (!write_in_progress_)
		{
			auto end = high_resolution_clock::now();
			fp_nanos = end - start;

			if (fp_nanos.count()/1000000 < DATA_SEND_TICK)
			{
				continue;
			}

			write_in_progress_ = true;
			read_in_progress_ = false;

			string sMessage;
			in_pLogKeeper->GetMessage(sMessage);

			operation.Send(sMessage, error);
			assert(error.value() >= 0);

			std::cerr << "send message. " << sMessage << "\n";

			start = end;

			continue;
		}
	}

	return S_OK;
}