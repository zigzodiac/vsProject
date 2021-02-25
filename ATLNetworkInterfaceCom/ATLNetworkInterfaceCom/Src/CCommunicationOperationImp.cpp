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

#define START_CODE 0x02;
#define MESSAGE_SIZE_LEN 4
typedef unsigned char unint8;

CommunicationOperationImp::CommunicationOperationImp(tcp::socket* socket)
	:pSocket(socket)
{
}

bool CommunicationOperationImp::PrepareReceive() const
{
	return state_ == reading;
}

void CommunicationOperationImp::Receive(string& message,boost::system::error_code& ec, int &recvLen)
{
	if (std::size_t len = pSocket->read_some(boost::asio::buffer(m_buffer), ec))
	{
		std::string str(std::begin(m_buffer), std::begin(m_buffer)+len);
		message = str;
		recvLen = len;
		state_ = writing;
	}
}

bool CommunicationOperationImp::PrepareSend() const
{
	return state_ == writing;
}

void CommunicationOperationImp::Send(char* message,boost::system::error_code& ec, int srcLen)
{

	if (std::size_t len = pSocket->write_some(
		boost::asio::buffer(message, srcLen), ec))
	{
		state_ = srcLen == len ? writing : reading;
	}
}

void CommunicationOperationImp::AddMessageHeader(char* dest, int srcLen, char *src) {
	dest[0] = START_CODE;
	UnintToHex(dest + 1, srcLen);
	memcpy(dest + 5, src, srcLen);
}

void CommunicationOperationImp::AbanMessageHeader(char* dest, int srcLen, char* src) {
	int dataLen = HexToUnint(&src[1]);
	std::cerr << "dataLen. " << dataLen << "\n";
	memcpy(dest, &src[5], dataLen);
}

bool CommunicationOperationImp::UnintToHex(char * dest, int srcLen)
{
	int count = 0;
	while (count < MESSAGE_SIZE_LEN) {
		dest[count] = (srcLen & 0xff);
		srcLen >>= 8;
		count++;
		/*printf("size %x %d\n", dest[count], dest[count]);*/
	}
	return true;
}

int CommunicationOperationImp::HexToUnint(char * src)
{
	UINT len = 0;
	UINT cur = 0;
	char* pPos = src;
	int bit = 8;
	for (int i = 0; i < MESSAGE_SIZE_LEN; i++) {
		unint8 byteValue = *pPos;
		cur = (unsigned int)byteValue;
		cur <<= bit * i;
		len += cur;
		pPos++;
	}
	return len;
}