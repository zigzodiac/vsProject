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
//   File:             CommunicationOperationImp.h
//--------------------------------------------------------------------------
//   
//   Beijing China. 
//--------------------------------------------------------------------------
#pragma once

#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <memory>

#define MAX_BUFFER_SIZE 16*1024
using boost::asio::ip::tcp;

class CommunicationOperationImp
{
public:
	CommunicationOperationImp(tcp::socket* socket);

	bool PrepareReceive() const;

	void Receive(string& message, boost::system::error_code& ec);

	bool PrepareSend() const;

	void Send(string& message, boost::system::error_code& ec);

private:
	tcp::socket* pSocket;
	enum { reading, writing } state_ = reading;
	std::array<char, MAX_BUFFER_SIZE> m_buffer;
};
