//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_filepath.ipp
 * @brief		iris unit test ファイルパスクラス ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_FILEPATH_IPP_D69E7545_BF8A_4EDC_9493_9105C69F9378_
#define INCG_IRIS_IUTEST_FILEPATH_IPP_D69E7545_BF8A_4EDC_9493_9105C69F9378_

//======================================================================
// include
#include "../internal/iutest_filepath.hpp"

namespace iutest {
namespace detail
{

IUTEST_IPP_INLINE bool iuFilePath::IsDirectory(void) const
{
	return !m_path.empty() &&
		IsPathSeparator(m_path.c_str()[length()-1]);
}

IUTEST_IPP_INLINE bool iuFilePath::IsRootDirectory(void) const
{
#ifdef IUTEST_OS_WINDOWS
	if( length() != 3 )
	{
		return false;
	}
#else
	if( length() != 3 )
	{
		return false;
	}
#endif
	return IsAbsolutePath();
}

IUTEST_IPP_INLINE bool iuFilePath::IsAbsolutePath(void) const
{
#ifdef IUTEST_OS_WINDOWS
	if( length() < 3 )
	{
		return false;
	}
	const char* name = m_path.c_str();

	if( IsDBCSLeadByte(name[0]) )
	{
		++name;
	}
	return (name[1] == ':' && IsPathSeparator(name[0]));
#else
	return IsPathSeparator(m_path.c_str()[0]);
#endif
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::RemoveTrailingPathSeparator(void) const
{
	return IsDirectory() ? iuFilePath(std::string(m_path.c_str(), length()-1)) : *this;
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::RemoveExtension(const char* extention) const
{
	const char* const path = m_path.c_str();
	const char* const ext = strrchr(path, '.');
	if( ext == NULL )
	{
		return *this;
	}
	if( extention != NULL && !IsStringCaseEqual(ext + 1, extention) )
	{
		return *this;
	}
	const size_t length = ext - path;
	return iuFilePath(std::string(path, length));
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::RemoveDirectoryName(void) const
{
	const char* const sep = FindLastPathSeparator();
	return sep != NULL ? iuFilePath(sep+1) : *this;
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::RemoveFileName(void) const
{
	const char* sep = FindLastPathSeparator();
	if( sep == NULL )
	{
		return GetRelativeCurrentDir();
	}
	const size_t length = sep - c_str() + 1;
	return iuFilePath(std::string(c_str(), length));
}

IUTEST_IPP_INLINE bool iuFilePath::CreateFolder(void) const
{
#if   defined(IUTEST_OS_WINDOWS_MOBILE)
#elif defined(IUTEST_OS_WINDOWS_MINGW)
	if( mkdir(c_str()) == 0 )
	{
		return true;
	}
#elif defined(IUTEST_OS_WINDOWS)
	if( _mkdir(c_str()) == 0 )
	{
		return true;
	}
#else
	if( mkdir(c_str(), 0777) == 0 )
	{
		return true;
	}
#endif
	return DirectoryExists();
}

IUTEST_IPP_INLINE bool iuFilePath::CreateDirectoriesRecursively(void) const
{
	if( !IsDirectory() )
	{
		return false;
	}

	if( length() == 0 || DirectoryExists() )
	{
		return true;
	}

	const iuFilePath parent = RemoveTrailingPathSeparator().RemoveFileName();
	if( !parent.CreateDirectoriesRecursively() )
	{
		return false;
	}
	return CreateFolder();
}

IUTEST_IPP_INLINE bool iuFilePath::FileOrDirectoryExists(void) const
{
#if IUTEST_HAS_FILE_STAT
	posix::StatStruct file_stat;
	return posix::Stat(c_str(), &file_stat) == 0;
#endif
}

IUTEST_IPP_INLINE bool iuFilePath::DirectoryExists(void) const
{
#if IUTEST_HAS_FILE_STAT

#ifdef IUTEST_OS_WINDOWS
	const iuFilePath& path = IsRootDirectory() ? *this : RemoveTrailingPathSeparator();
#else
	const iuFilePath& path = *this;
#endif
	posix::StatStruct file_stat;
	if( posix::Stat(path.c_str(), &file_stat) == 0 )
	{
		return posix::IsDir(file_stat);
	}
#endif
	return false;
}

IUTEST_IPP_INLINE const char* iuFilePath::FindLastPathSeparator(void) const
{
	const char* ps = c_str();
	if( ps == NULL )
	{
		return NULL;
	}
	const char* pe = ps + length() - 1;
	while( pe >= ps )
	{
		if( IsPathSeparator(*pe) )
		{
			if( (*(pe-1) & 0x80) == 0 )
			{
				return pe;
			}
			--pe;
		}
		--pe;
	}
	return NULL;
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::GetCurrentDir(void)
{
	return iuFilePath(internal::posix::GetCWD());
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::GetRelativeCurrentDir(void)
{
	::std::string dir(".");
	dir += GetPathSeparator();
	return iuFilePath(dir);
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::GetExecFilePath(void)
{
#if   defined(IUTEST_OS_WINDOWS)
	char path[MAX_PATH];
	::GetModuleFileNameA(NULL, path, sizeof(path));
	return iuFilePath(path);
#elif defined(IUTEST_OS_SOLARIS)
	return iuFilePath(getexecname());
#elif defined(__FreeBSD__)
	int exe_path_mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, getpid() };
	char buf[1024];
	size_t length = 0;
	if( sysctl(exe_path_mib, 4, buf, &length, NULL, 0) != 0 )
	{
		return iuFilePath();
	}
	return iuFilePath(buf);
#elif defined(IUTEST_OS_LINUX) || defined(IUTEST_OS_CYGWIN)
	char buf[1024];
	const ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
	if( len == -1 )
	{
		return iuFilePath();
	}
	buf[len] = '\0';
	return iuFilePath(buf);
#else
	return iuFilePath();
#endif
}

IUTEST_IPP_INLINE iuFilePath iuFilePath::ConcatPaths(const iuFilePath& directory, const iuFilePath& relative_path)
{
	::std::string path = directory.RemoveTrailingPathSeparator().c_str();
	path += GetPathSeparator();
	path += relative_path.c_str();
	return iuFilePath(path);
}

IUTEST_IPP_INLINE char iuFilePath::GetPathSeparator(void) IUTEST_CXX_NOEXCEPT_SPEC
{
#ifdef IUTEST_OS_WINDOWS
	return '\\';
#else
	return '/';
#endif
}

IUTEST_IPP_INLINE void iuFilePath::Normalize(void)
{
	const char* src = c_str();
	if( src == NULL )
	{
		m_path = "";
		return;
	}

	char* const dst_top = new char [length()+1];
	char* dst = dst_top;

	while(*src != '\0')
	{
		*dst = *src;
		if( !IsPathSeparator(*src) )
		{
			++src;
		}
		else
		{
			if( IsAltPathSeparator(*src) )
			{
				*dst = GetPathSeparator();
			}
			++src;
			while( IsPathSeparator(*src) )
			{
				++src;
			}
		}
		++dst;
	}
	*dst = '\0';
	m_path = dst_top;
	delete [] dst_top;
}

IUTEST_IPP_INLINE bool iuFilePath::IsPathSeparator(char c) IUTEST_CXX_NOEXCEPT_SPEC
{
#ifdef IUTEST_OS_WINDOWS
	if( c == '\\' )
	{
		return true;
	}
#endif
	return c == '/';
}

IUTEST_IPP_INLINE bool iuFilePath::IsAltPathSeparator(char c) IUTEST_CXX_NOEXCEPT_SPEC
{
#ifdef IUTEST_OS_WINDOWS
	if( c == '/' )
	{
		return true;
	}
#else
	IUTEST_UNUSED_VAR(c);
#endif
	return false;
}

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_FILEPATH_IPP_D69E7545_BF8A_4EDC_9493_9105C69F9378_
