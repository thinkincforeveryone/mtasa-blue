/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Win32Utf8FileHooks.hpp
*  PURPOSE:     Hooks for making Windows file functions use UTF8 strings
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "detours/include/detours.h"

/*
The hooks in this file modify the following functions to work correctly with utf8 strings:

    CreateFile
    LoadLibrary
    LoadLibraryEx
    SetDllDirectory
    SetCurrentDirectory
    AddFontResourceEx
    RemoveFontResourceEx
    RemoveDirectory
    GetDiskFreeSpaceEx
    GetFileAttributes
    SetFileAttributes
    ShellExecute
    CreateDirectory
    CopyFile
    MoveFile
    DeleteFile
    GetModuleHandle
    sopen_s
    _chdir
    _mkdir
    _rmdir
    remove
    rename

The following will also work correctly with utf8 strings as they eventually call the above hooked functions:

    fopen
    unlink
    fstream

BUT
    * Many other functions are not hooked and will need utf8 conversions when used, like these:
        FindFirstFile
        GetModuleFileName
        GetModuleFileNameEx
        GetCurrentDirectory
        GetDllDirectory
        getcwd
            and many more...
*/


//
// Determine file name of Microsoft Visual Studio C/C++ Runtime dll
//
#if _MSC_VER == 1500        // MSVC++ 9.0 (Visual Studio 2008)
    #ifdef _DEBUG
        #define MSVCR_DLL "msvcr90d.dll"
    #else
        #define MSVCR_DLL "msvcr90.dll"
    #endif
#elif _MSC_VER == 1800      // MSVC++ 12.0 (Visual Studio 2013)
    #ifdef _DEBUG
        #define MSVCR_DLL "msvcr120d.dll"
    #else
        #define MSVCR_DLL "msvcr120.dll"
    #endif
#elif _MSC_VER == 1900      // MSVC++ 14.0 (Visual Studio 2015)
    #ifdef _DEBUG
        #define MSVCR_DLL "ucrtbased.dll"
    #else
        #define MSVCR_DLL "ucrtbase.dll"
    #endif
#else
    #error "Insert VCR info"
#endif


namespace SharedUtil
{

    /////////////////////////////////////////////////////////////
    //
    // Function defs
    //
    /////////////////////////////////////////////////////////////

    typedef
    HANDLE
    (WINAPI
    *FUNC_CreateFileA)(
        __in     LPCSTR lpFileName,
        __in     DWORD dwDesiredAccess,
        __in     DWORD dwShareMode,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        __in     DWORD dwCreationDisposition,
        __in     DWORD dwFlagsAndAttributes,
        __in_opt HANDLE hTemplateFile
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_LoadLibraryA)(
        __in LPCSTR lpLibFileName
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_LoadLibraryExA)(
        __in       LPCSTR lpLibFileName,
        __reserved HANDLE hFile,
        __in       DWORD dwFlags
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetDllDirectoryA)(
        __in_opt LPCSTR lpPathName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetCurrentDirectoryA)(
        __in LPCSTR lpPathName
        );

    typedef int  (WINAPI *FUNC_AddFontResourceExA)( __in LPCSTR name, __in DWORD fl, __reserved PVOID res);
    typedef BOOL (WINAPI *FUNC_RemoveFontResourceExA)( __in LPCSTR name, __in DWORD fl, __reserved PVOID pdv);

    typedef
    BOOL
    (WINAPI
    *FUNC_RemoveDirectoryA)(
        __in LPCSTR lpPathName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_GetDiskFreeSpaceExA)(
        __in_opt  LPCSTR lpDirectoryName,
        __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
        __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
        __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
        );

    typedef
    DWORD
    (WINAPI
    *FUNC_GetFileAttributesA)(
        __in LPCSTR lpFileName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_SetFileAttributesA)(
        __in LPCSTR lpFileName,
        __in DWORD dwFileAttributes
        );

    typedef HINSTANCE (STDAPICALLTYPE *FUNC_ShellExecuteA)(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters,
        LPCSTR lpDirectory, INT nShowCmd);

    typedef
    BOOL
    (WINAPI
    *FUNC_CreateDirectoryA)(
        __in     LPCSTR lpPathName,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_CopyFileA)(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName,
        __in BOOL bFailIfExists
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_MoveFileA)(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName
        );

    typedef
    BOOL
    (WINAPI
    *FUNC_DeleteFileA)(
        __in LPCSTR lpFileName
        );

    typedef
    HMODULE
    (WINAPI
    *FUNC_GetModuleHandleA)(
        __in_opt LPCSTR lpModuleName
        );

    typedef
    errno_t
    (__cdecl
    *FUNC__sopen_s)(
        _Out_ int * _FileHandle,
        _In_z_ const char * _Filename,
        _In_ int _OpenFlag,
        _In_ int _ShareFlag,
        _In_ int _PermissionMode
        );

    typedef
    int
    (__cdecl
    *FUNC__chdir) (
            const char *_Path
            );

    typedef
    int
    (__cdecl
    *FUNC__mkdir) (
            const char *_Path
            );

    typedef
    int
    (__cdecl
    *FUNC__rmdir) (
            const char *_Path
            );

    typedef
    int
    (__cdecl
    *FUNC_remove) (
            const char *_Filename
            );

    typedef
    int
    (__cdecl
    *FUNC_rename) (
            const char *_OldFilename,
            const char *_NewFilename
            );

    /////////////////////////////////////////////////////////////
    //
    // Hook variables
    //
    /////////////////////////////////////////////////////////////
    #define HOOKVAR(name) \
        FUNC_##name pfn##name;

    HOOKVAR( CreateFileA )
    HOOKVAR( LoadLibraryA )
    HOOKVAR( LoadLibraryExA )
    HOOKVAR( SetDllDirectoryA )
    HOOKVAR( SetCurrentDirectoryA )
    HOOKVAR( AddFontResourceExA )
    HOOKVAR( RemoveFontResourceExA )
    HOOKVAR( RemoveDirectoryA )
    HOOKVAR( GetDiskFreeSpaceExA )
    HOOKVAR( GetFileAttributesA )
    HOOKVAR( SetFileAttributesA )
    HOOKVAR( ShellExecuteA )
    HOOKVAR( CreateDirectoryA )
    HOOKVAR( CopyFileA )
    HOOKVAR( MoveFileA )
    HOOKVAR( DeleteFileA )
    HOOKVAR( GetModuleHandleA )
    HOOKVAR( _sopen_s )
    HOOKVAR( _chdir )
    HOOKVAR( _mkdir )
    HOOKVAR( _rmdir )
    HOOKVAR( remove )
    HOOKVAR( rename )


#ifdef MTA_CLIENT
    /////////////////////////////////////////////////////////////
    //
    // Ensure codepage type characters have been utf8ified
    //
    /////////////////////////////////////////////////////////////
    SString MakeSurePathIsUTF8( const SString& strOriginal )
    {
        if ( strOriginal.Contains( "GTA San Andreas User Files" ) )
        {
            // Fix gta save path
            SString strPathTail = strOriginal.SplitRight( "GTA San Andreas User Files" );
            SString strNewPathName = PathJoin( GetSystemPersonalPath(), "GTA San Andreas User Files", strPathTail );
            return strNewPathName;
        }
        else
        {
            static SString LaunchPathA;
            if ( LaunchPathA.empty() )
            {
                char szBuffer[2048];
                GetModuleFileNameA( NULL, szBuffer, NUMELMS(szBuffer) - 1 );
                LaunchPathA = ExtractPath( szBuffer );
            }
            if ( strOriginal.BeginsWithI( LaunchPathA ) )
            {
                // Fix gta install path
                SString strPathTail = strOriginal.SubStr( LaunchPathA.length() );
                SString strNewPathName = PathJoin( GetLaunchPath(), strPathTail );
                return strNewPathName;
            }
        }
        return strOriginal;
    }
#endif

    /////////////////////////////////////////////////////////////
    //
    // Hook implementations
    //
    /////////////////////////////////////////////////////////////
    HANDLE
    WINAPI
    MyCreateFileA(
        __in     LPCSTR lpFileName,
        __in     DWORD dwDesiredAccess,
        __in     DWORD dwShareMode,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        __in     DWORD dwCreationDisposition,
        __in     DWORD dwFlagsAndAttributes,
        __in_opt HANDLE hTemplateFile
        )
    {
        SString strFileName = lpFileName;
#ifdef MTA_CLIENT
        if ( IsGTAProcess() )
            strFileName = MakeSurePathIsUTF8( strFileName );
#endif
        return CreateFileW( FromUTF8( strFileName ), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
    }

    HMODULE
    WINAPI
    MyLoadLibraryA(
        __in LPCSTR lpLibFileName
        )
    {
        return LoadLibraryW( FromUTF8( lpLibFileName ) );
    }

    HMODULE
    WINAPI
    MyLoadLibraryExA(
        __in       LPCSTR lpLibFileName,
        __reserved HANDLE hFile,
        __in       DWORD dwFlags
        )
    {
        return LoadLibraryExW( FromUTF8( lpLibFileName ), hFile, dwFlags );
    }

    BOOL
    WINAPI
    MySetDllDirectoryA(
        __in_opt LPCSTR lpPathName
        )
    {
        return SetDllDirectoryW( FromUTF8( lpPathName ) );
    }

    BOOL
    WINAPI
    MySetCurrentDirectoryA(
        __in LPCSTR lpPathName
        )
    {
        SString strPathName = lpPathName;
#ifdef MTA_CLIENT
        if ( IsGTAProcess() )
            strPathName = MakeSurePathIsUTF8( strPathName );
#endif
        return SetCurrentDirectoryW( FromUTF8( strPathName ) );
    }

    int  WINAPI MyAddFontResourceExA( __in LPCSTR name, __in DWORD fl, __reserved PVOID res        )
    {
        return AddFontResourceExW( FromUTF8( name ), fl, res );
    }

    BOOL WINAPI MyRemoveFontResourceExA( __in LPCSTR name, __in DWORD fl, __reserved PVOID pdv        )
    {
        return RemoveFontResourceExW( FromUTF8( name ), fl, pdv );
    }

    BOOL
    WINAPI
    MyRemoveDirectoryA(
        __in LPCSTR lpPathName
        )
    {
        return RemoveDirectoryW( FromUTF8( lpPathName ) );
    }

    BOOL
    WINAPI
    MyGetDiskFreeSpaceExA(
        __in_opt  LPCSTR lpDirectoryName,
        __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
        __out_opt PULARGE_INTEGER lpTotalNumberOfBytes,
        __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes
        )
    {
        return GetDiskFreeSpaceExW( FromUTF8( lpDirectoryName ), lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes );
    }

    DWORD
    WINAPI
    MyGetFileAttributesA(
        __in LPCSTR lpFileName
        )
    {
        return GetFileAttributesW( FromUTF8( lpFileName ) );
    }

    BOOL
    WINAPI
    MySetFileAttributesA(
        __in LPCSTR lpFileName,
        __in DWORD dwFileAttributes
        )
    {
        return SetFileAttributesW( FromUTF8( lpFileName ), dwFileAttributes );
    }

    HINSTANCE STDAPICALLTYPE MyShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters,
        LPCSTR lpDirectory, INT nShowCmd        )
    {
        return ShellExecuteW( hwnd, FromUTF8( lpOperation ), FromUTF8( lpFile ), FromUTF8( lpParameters ), FromUTF8( lpDirectory ), nShowCmd );
    }

    BOOL
    WINAPI
    MyCreateDirectoryA(
        __in     LPCSTR lpPathName,
        __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
        )
    {
        SString strPathName = lpPathName;
#ifdef MTA_CLIENT
        if ( IsGTAProcess() )
            strPathName = MakeSurePathIsUTF8( strPathName );
#endif
        return CreateDirectoryW( FromUTF8( strPathName ), lpSecurityAttributes );
    }

    BOOL
    WINAPI
    MyCopyFileA(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName,
        __in BOOL bFailIfExists
        )
    {
        return CopyFileW( FromUTF8( lpExistingFileName ), FromUTF8( lpNewFileName ), bFailIfExists );
    }

    BOOL
    WINAPI
    MyMoveFileA(
        __in LPCSTR lpExistingFileName,
        __in LPCSTR lpNewFileName
        )
    {
        return MoveFileW( FromUTF8( lpExistingFileName ), FromUTF8( lpNewFileName ) );
    }

    BOOL
    WINAPI
    MyDeleteFileA(
        __in LPCSTR lpFileName
        )
    {
        return DeleteFileW( FromUTF8( lpFileName ) );
    }

    HMODULE
    WINAPI
    MyGetModuleHandleA(
        __in_opt LPCSTR lpModuleName
        )
    {
        if ( !lpModuleName )
            return GetModuleHandleW( NULL );
        return GetModuleHandleW( FromUTF8( lpModuleName ) );
    }

    errno_t
    __cdecl
    My_sopen_s(
        _Out_ int * _FileHandle,
        _In_z_ const char * _Filename,
        _In_ int _OpenFlag,
        _In_ int _ShareFlag,
        _In_ int _PermissionMode
    )
    {
        return _wsopen_s( _FileHandle, FromUTF8( _Filename ), _OpenFlag, _ShareFlag, _PermissionMode );
    }


    int __cdecl My_chdir( const char *_Path )
    {
        return _wchdir( FromUTF8( _Path ) );
    }

    int __cdecl My_mkdir( const char *_Path )
    {
        return _wmkdir( FromUTF8( _Path ) );
    }

    int __cdecl My_rmdir( const char *_Path )
    {
        return _wrmdir( FromUTF8( _Path ) );
    }

    int __cdecl Myremove( const char * _Filename )
    {
        return _wremove( FromUTF8( _Filename ) );
    }

    int __cdecl Myrename( const char * _OldFilename, const char * _NewFilename )
    {
        return _wrename( FromUTF8( _OldFilename ), FromUTF8( _NewFilename ) );
    }

    /////////////////////////////////////////////////////////////
    //
    // Hook adding
    //
    /////////////////////////////////////////////////////////////
    void AddUtf8FileHooks( void )
    {
        #define ADDHOOK(module,name) \
                pfn##name = reinterpret_cast < FUNC_##name > ( DetourFunction ( DetourFindFunction ( module, #name ), reinterpret_cast < PBYTE > ( My##name ) ) ); \
                assert( pfn##name );

        ADDHOOK( "Kernel32.dll", CreateFileA )
        ADDHOOK( "Kernel32.dll", LoadLibraryA )
        ADDHOOK( "Kernel32.dll", LoadLibraryExA )
        ADDHOOK( "Kernel32.dll", SetDllDirectoryA )
        ADDHOOK( "Kernel32.dll", SetCurrentDirectoryA )
        ADDHOOK( "Gdi32.dll",    AddFontResourceExA )
        ADDHOOK( "Gdi32.dll",    RemoveFontResourceExA )
        ADDHOOK( "Kernel32.dll", RemoveDirectoryA )
        ADDHOOK( "Kernel32.dll", GetDiskFreeSpaceExA )
        ADDHOOK( "Kernel32.dll", GetFileAttributesA )
        ADDHOOK( "Kernel32.dll", SetFileAttributesA )
        ADDHOOK( "Shell32.dll",  ShellExecuteA )
        ADDHOOK( "Kernel32.dll", CreateDirectoryA )
        ADDHOOK( "Kernel32.dll", CopyFileA )
        ADDHOOK( "Kernel32.dll", MoveFileA )
        ADDHOOK( "Kernel32.dll", DeleteFileA )
        ADDHOOK( "Kernel32.dll", GetModuleHandleA )
        ADDHOOK( MSVCR_DLL, _sopen_s )
        ADDHOOK( MSVCR_DLL, _chdir )
        ADDHOOK( MSVCR_DLL, _mkdir )
        ADDHOOK( MSVCR_DLL, _rmdir )
        ADDHOOK( MSVCR_DLL, remove )
        ADDHOOK( MSVCR_DLL, rename )
    }


    /////////////////////////////////////////////////////////////
    //
    // Hook taking away
    //
    /////////////////////////////////////////////////////////////
    void RemoveUtf8FileHooks( void )
    {
        #define DELHOOK(name) \
            if ( pfn##name ) \
            { \
                DetourRemove ( reinterpret_cast < PBYTE > ( pfn##name ),  \
                               reinterpret_cast < PBYTE > ( My##name  ) ); \
                pfn##name = NULL; \
            }

        DELHOOK( CreateFileA )
        DELHOOK( LoadLibraryA )
        DELHOOK( LoadLibraryExA )
        DELHOOK( SetDllDirectoryA )
        DELHOOK( SetCurrentDirectoryA )
        DELHOOK( AddFontResourceExA )
        DELHOOK( RemoveFontResourceExA )
        DELHOOK( RemoveDirectoryA )
        DELHOOK( GetDiskFreeSpaceExA )
        DELHOOK( GetFileAttributesA )
        DELHOOK( SetFileAttributesA )
        DELHOOK( ShellExecuteA )
        DELHOOK( CreateDirectoryA )
        DELHOOK( CopyFileA )
        DELHOOK( MoveFileA )
        DELHOOK( DeleteFileA )
        DELHOOK( GetModuleHandleA )
        DELHOOK( _sopen_s )
        DELHOOK( _chdir )
        DELHOOK( _mkdir )
        DELHOOK( _rmdir )
        DELHOOK( remove )
        DELHOOK( rename )
    }
}
