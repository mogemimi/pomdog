//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_filepath.hpp
 * @brief		iris unit test ファイルパスクラス ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_FILEPATH_HPP_D69E7545_BF8A_4EDC_9493_9105C69F9378_
#define INCG_IRIS_IUTEST_FILEPATH_HPP_D69E7545_BF8A_4EDC_9493_9105C69F9378_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// include
#include "iutest_string.hpp"
#include "iutest_port.hpp"

#if IUTEST_HAS_FILE_STAT
#  include <sys/stat.h>
#endif

#if	defined(_MSC_VER) && !defined(IUTEST_OS_WINDOWS_MOBILE)
#  include <direct.h>
#endif

namespace iutest {

namespace internal {
namespace posix
{

#if IUTEST_HAS_FILE_STAT

#if defined(IUTEST_OS_WINDOWS)

	typedef struct _stat StatStruct;

	inline int Stat(const char* path, StatStruct* buf) { return _stat(path, buf); }
	inline bool IsDir(const StatStruct& st) { return (st.st_mode & _S_IFDIR) != 0; }

#else

	typedef struct stat StatStruct;

	inline int Stat(const char* path, StatStruct* buf) { return stat(path, buf); }
	inline bool IsDir(const StatStruct& st) { return S_ISDIR(st.st_mode); }

#endif

#endif

}	// end of namespace posix
}	// end of namespace internal

namespace detail
{

//======================================================================
// class
/**
 * @brief	ファイルパスクラス
*/
class iuFilePath
{
public:
	iuFilePath(void) : m_path("") {}
	iuFilePath(const iuFilePath& rhs) : m_path(rhs.m_path) {}

	explicit iuFilePath(const char* path) : m_path(path)
	{
		Normalize();
	}
	explicit iuFilePath(const std::string& path) : m_path(path)
	{
		Normalize();
	}

public:
	std::string	ToString(void)	const	{ return m_path; }
	const char* c_str(void)		const	{ return m_path.c_str(); }
	bool		IsEmpty(void)	const	{ return c_str() == NULL || *c_str() == '\0'; }
	size_t		length(void)	const	{ return m_path.length(); }

public:
	bool operator == (const iuFilePath& rhs) const
	{
		return IsStringCaseEqual(c_str(), rhs.c_str());
	}
	bool operator == (const char* rhs) const
	{
		return IsStringCaseEqual(c_str(), rhs);
	}
	//operator const char* (void) const	{ return c_str(); }

public:

	/**
	 * @brief	フォルダパスかどうか
	*/
	bool	IsDirectory(void) const;

	/**
	 * @brief	ルートディレクトリパスかどうか
	*/
	bool	IsRootDirectory(void) const;

	/**
	 * @brief	絶対パスかどうか
	*/
	bool	IsAbsolutePath(void) const;

	/**
	 * @brief	末尾のセパレーターを削除
	*/
	iuFilePath	RemoveTrailingPathSeparator(void) const;

	/**
	 * @brief	拡張子の削除
	*/
	iuFilePath	RemoveExtension(const char* extention=NULL) const;

	/**
	 * @brief	ディレクトリ名の削除
	*/
	iuFilePath	RemoveDirectoryName(void) const;

	/**
	 * @brief	ファイル名の削除
	*/
	iuFilePath	RemoveFileName(void) const;

	/**
	 * @brief	フォルダの作成
	*/
	bool		CreateFolder(void) const;

	/**
	 * @brief	フォルダを再帰的に作成
	*/
	bool		CreateDirectoriesRecursively(void) const;

	/**
	 * @brief	ファイルまたはフォルダが存在するかどうか
	*/
	bool		FileOrDirectoryExists(void) const;

	/**
	 * @brief	フォルダが存在するかどうか
	*/
	bool		DirectoryExists(void) const;

	/**
	 * @brief	一番後ろのパスセパレータのアドレスを取得
	*/
	const char* FindLastPathSeparator(void) const;

public:
	/**
	 * @brief	カレントディレクトリの取得
	*/
	static iuFilePath	GetCurrentDir(void);

	/**
	 * @brief	カレントディレクトリの相対パス取得
	*/
	static iuFilePath	GetRelativeCurrentDir(void);

	/**
	 * @brief	実行ファイルのパスを取得
	*/
	static iuFilePath	GetExecFilePath(void);

	/**
	 * @brief	パスの結合
	*/
	static iuFilePath	ConcatPaths(const iuFilePath& directory, const iuFilePath& relative_path);

public:
	/**
	 * @brief	パス区切り文字の取得
	*/
	static char	GetPathSeparator(void) IUTEST_CXX_NOEXCEPT_SPEC;

private:
	/**
	 * @biref	正規化
	*/
	void Normalize(void);

private:
	static bool IsPathSeparator(char c) IUTEST_CXX_NOEXCEPT_SPEC;
	static bool IsAltPathSeparator(char c) IUTEST_CXX_NOEXCEPT_SPEC;
private:
	std::string	m_path;
};

inline iu_ostream& operator << (iu_ostream& os, const iuFilePath& path)
{
	return os << path.c_str();
}

}	// end of namespace detail

namespace internal
{
	// google test との互換性のため
	typedef detail::iuFilePath FilePath;
}

}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_filepath.ipp"
#endif

#endif

#endif
