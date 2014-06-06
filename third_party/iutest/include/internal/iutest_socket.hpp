//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_socket.hpp
 * @brief		iris unit test soket
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2013-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SOCKET_HPP_77654A63_0A08_43CA_950E_61232690163B_
#define INCG_IRIS_IUTEST_SOCKET_HPP_77654A63_0A08_43CA_950E_61232690163B_

//======================================================================
// include
#include "iutest_internal_defs.hpp"
#include "iutest_stream.hpp"

#ifdef IUTEST_HAS_SOCKET

#ifdef IUTEST_OS_WINDOWS
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#  include <netdb.h>
#endif

#if defined(_MSC_VER)
#  if defined(IUTEST_OS_WINDOWS_MOBILE)
#    pragma comment(lib, "ws2.lib")
#  else
#    pragma comment(lib, "ws2_32.lib")
#  endif
#endif

namespace iutest {
namespace detail
{

//======================================================================
// class
/*
 * @brief	ソケットベースクラス
*/
class BasicSocket
{
public:
#ifdef IUTEST_OS_WINDOWS
	typedef SOCKET descriptor_t;

#if !defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
	static const descriptor_t INVALID_DESCRIPTOR = INVALID_SOCKET;
#else
	enum { INVALID_DESCRIPTOR = INVALID_SOCKET };
#endif

#else
	typedef int descriptor_t;

#if !defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
	static const descriptor_t INVALID_DESCRIPTOR = -1;
#else
	enum { INVALID_DESCRIPTOR = -1 };
#endif

#endif
public:
	BasicSocket(void) : m_socket(INVALID_DESCRIPTOR)
	{
#ifdef IUTEST_OS_WINDOWS
		WSADATA wsaData;
		(void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	}
	~BasicSocket(void)
	{
		Close();
#ifdef IUTEST_OS_WINDOWS
		WSACleanup();
#endif
	}
public:
	bool Open(const char* host, const char* port)
	{
		if( m_socket != INVALID_DESCRIPTOR )
		{
			return true;
		}
		addrinfo* servinfo = NULL;
		addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		const int err_no = getaddrinfo(host, port, &hints, &servinfo);
		if( err_no != 0 )
		{
			return false;
		}

		for( addrinfo* curr=servinfo; curr != NULL; curr = curr->ai_next )
		{
			const descriptor_t fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
			if( fd != INVALID_DESCRIPTOR )
			{
				if( connect(fd, curr->ai_addr, curr->ai_addrlen) != -1 )
				{
					m_socket = fd;
					break;
				}
				Close(fd);
			}
		}
		freeaddrinfo(servinfo);
		return (m_socket != INVALID_DESCRIPTOR);
	}
	void Close(void)
	{
		Close(m_socket);
		m_socket = INVALID_DESCRIPTOR;
	};
	void CheckLastError(void)
	{
#ifdef IUTEST_OS_WINDOWS
		const int le = WSAGetLastError();
		IUTEST_LOG_(WARNING) << "WSAGetLastError:" << le;
#endif
	}
public:
	static int Close(descriptor_t d)
	{
#ifdef IUTEST_OS_WINDOWS
		return closesocket(d);
#else
		return close(d);
#endif
	}

public:
	bool IsValid(void) const
	{
		return m_socket != INVALID_DESCRIPTOR;
	}
protected:
	descriptor_t m_socket;

	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(BasicSocket);
};

/*
 * @brief	ソケット書き込みクラス
*/
class SocketWriter : virtual public BasicSocket
	, public IOutStream
{
public:
	SocketWriter(void) {}
public:
	bool Send(const ::std::string& message)
	{
		return Write(message.c_str(), message.length(), 1u);
	}
	bool SendLn(const ::std::string& message)
	{
#ifdef IUTEST_OS_WINDOWS
		return Send(message + "\r\n");
#else
		return Send(message + "\r\n");
		//return Send(message + "\n");
#endif
	}
public:
	virtual bool Write(const void* buf, size_t size, size_t cnt) IUTEST_CXX_OVERRIDE
	{
		if( !IsValid() )
		{
			return false;
		}
		const int size_ = static_cast<int>(size);
		for( size_t i=0; i < cnt; ++i )
		{
#ifdef IUTEST_OS_WINDOWS
			if( send(m_socket, static_cast<const char*>(buf), size_, 0) == SOCKET_ERROR )
			{
				CheckLastError();
				return false;
			}
#else
			if( write(m_socket, buf, size_) == -1 )
			{
				return false;
			}
#endif
		}
		return true;
	}
private:
	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(SocketWriter);
};

/*
 * @brief	ソケット読み込みクラス
*/
class SocketReader : virtual public BasicSocket
{
public:
	SocketReader(void) {}
public:
	bool Read(void* buf, size_t size)
	{
		if( !IsValid() )
		{
			return false;
		}
		const int size_ = static_cast<int>(size);
#ifdef IUTEST_OS_WINDOWS
		if( recv(m_socket, static_cast<char*>(buf), size_, 0) == SOCKET_ERROR )
		{
			CheckLastError();
			return false;
		}
#else
		if( read(m_socket, buf, size_) == -1 )
		{
			return false;
		}
#endif
		return true;
	}
private:
	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(SocketReader);
};

/*
 * @brief	ソケット読み込みクラス
*/
class Socket : public SocketWriter, public SocketReader
{
public:
	Socket(void) {}
};

}	// end of namespace detail
}	// end of namespace iutest

#endif

#endif // INCG_IRIS_IUTEST_SOCKET_HPP_77654A63_0A08_43CA_950E_61232690163B_
