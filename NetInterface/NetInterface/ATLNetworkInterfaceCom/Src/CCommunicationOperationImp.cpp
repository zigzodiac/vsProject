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
//   File:             CCommunicationOperationImp.cpp
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "CommunicationOperationImp.h"

CommunicationOperationImp::CommunicationOperationImp(tcp::socket* socket)
	:pSocket(socket)
{
}

bool CommunicationOperationImp::PrepareReceive() const
{
	return state_ == reading;
}

void CommunicationOperationImp::Receive(string& message,boost::system::error_code& ec)
{
	if (std::size_t len = pSocket->read_some(boost::asio::buffer(m_buffer), ec))
	{
		std::string str(std::begin(m_buffer), std::begin(m_buffer)+len);
		message = str;

		state_ = writing;
	}
}

bool CommunicationOperationImp::PrepareSend() const
{
	return state_ == writing;
}

void CommunicationOperationImp::Send(string& message,boost::system::error_code& ec)
{
	const char *data = message.c_str();
	if (std::size_t len = pSocket->write_some(
		boost::asio::buffer(data,strlen(data)), ec))
	{
		state_ = strlen(data) == len ? writing : reading;
	}
}