#include "stdafx.h"
#include "verinfo.h"

/////////////////////////////////////////////////////////////////////////////


HRESULT STDAPICALLTYPE DllGetVersion( IN  HMODULE hModule, 
									  OUT DLLVERSIONINFO* lpDVI )
{
	if( hModule == NULL ||  ::IsBadReadPtr( lpDVI, sizeof( DLLVERSIONINFO* ) ) )
	{
		ASSERT_RETURN( S_FALSE );
	}

	CONST DWORD cbSize = lpDVI->cbSize;

	if((cbSize != sizeof( DLLVERSIONINFO  ) && cbSize != sizeof( DLLVERSIONINFO2 ) ) || ::IsBadWritePtr( lpDVI, cbSize ) )
	{
		ASSERT_RETURN( S_FALSE );
	}

	::ZeroMemory( lpDVI, cbSize );
	lpDVI->cbSize = cbSize;
	
	CFileVersionInfo fvi;
	if( fvi.Open( hModule ) )
	{
		VS_FIXEDFILEINFO vsffi = fvi.GetVSFFI();

		if( vsffi.dwFileType == VFT_DLL ||
			vsffi.dwFileType == VFT_STATIC_LIB )
		{
			switch( vsffi.dwFileOS )
			{			
			case VOS__WINDOWS32:
			case VOS_NT_WINDOWS32:
				lpDVI->dwPlatformID = DLLVER_PLATFORM_WINDOWS;
				break;
			case VOS_NT:
				lpDVI->dwPlatformID = DLLVER_PLATFORM_NT;
				break;			
			default:
				return ( S_FALSE );
			}
			
			lpDVI->dwMajorVersion = HIWORD( vsffi.dwFileVersionMS );
			lpDVI->dwMinorVersion = LOWORD( vsffi.dwFileVersionMS );
			lpDVI->dwBuildNumber  = HIWORD( vsffi.dwFileVersionLS );
			
			if( cbSize == sizeof( DLLVERSIONINFO2 ) )
			{
				DLLVERSIONINFO2* lpDVI2 = (DLLVERSIONINFO2*)lpDVI;
				lpDVI2->ullVersion = MAKEDLLVERULL( 
					lpDVI->dwMajorVersion,
					lpDVI->dwMinorVersion,
					lpDVI->dwBuildNumber ,
					LOWORD( vsffi.dwFileVersionLS )
				);
			}
			return ( S_OK );
		}
	#ifdef _DEBUG
		else
			ASSERT( 0 );
	#endif

		fvi.Close();
	}

	return ( S_FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// HIWORD( ffi.dwFileVersionMS ) - major
// LOWORD( ffi.dwFileVersionMS ) - minor
// HIWORD( ffi.dwFileVersionLS ) - build
// LOWORD( ffi.dwFileVersionLS ) - QFE
/////////////////////////////////////////////////////////////////////////////

CFileVersionInfo::CFileVersionInfo( void ): m_lpbyVIB( NULL )
{		
	Close();
}

CFileVersionInfo::~CFileVersionInfo( void )
{
	Close();
}

LPCTSTR CFileVersionInfo::s_ppszStr[] = { 
	_T( "Comments" ),			_T( "CompanyName" ),      
	_T( "FileDescription" ),	_T( "FileVersion" ),
	_T( "InternalName" ),		_T( "LegalCopyright" ),
	_T( "LegalTrademarks" ),	_T( "OriginalFilename" ),
	_T( "PrivateBuild" ),		_T( "ProductName" ),
	_T( "ProductVersion" ),		_T( "SpecialBuild" ),
	_T( "OLESelfRegister" )
};

////////////////////////////////////////////////////////////////////////////////
// Implementation

BOOL CFileVersionInfo::Open( IN HINSTANCE hInstance )
{
	if( hInstance == NULL )
		ASSERT_RETURN( FALSE );

	TCHAR szFileName[ MAX_PATH ] = { 0 };
	if( ::GetModuleFileName( hInstance, szFileName, MAX_PATH ) )
		return Open( szFileName );

	return FALSE;
};

BOOL CFileVersionInfo::Open( IN LPCTSTR lpszFileName )
{
	if( lpszFileName == NULL )
		ASSERT_RETURN( FALSE );

	Close();
	if( !GetVersionInfo( lpszFileName ) || !QueryVersionTrans() )
		Close();

	return m_bValid;
};

BOOL CFileVersionInfo::GetVersionInfo( IN LPCTSTR lpszFileName )
{
	DWORD dwDummy = 0;
	DWORD dwSize  = ::GetFileVersionInfoSize( lpszFileName , &dwDummy  );

	if ( dwSize > 0 )
	{		
		m_lpbyVIB = (LPBYTE)new BYTE[dwSize ];

		if ( m_lpbyVIB != NULL && ::GetFileVersionInfo( lpszFileName, 0, dwSize, m_lpbyVIB ) )
		{
			UINT   uLen    = 0;
			LPVOID lpVSFFI = NULL;
			
			if ( ::VerQueryValue( m_lpbyVIB, _T( "\\" ), (LPVOID*)&lpVSFFI, &uLen ) )
			{
				::CopyMemory( &m_vsffi, lpVSFFI, sizeof( VS_FIXEDFILEINFO ) );
				m_bValid = ( m_vsffi.dwSignature == VS_FFI_SIGNATURE );
			}
		}
	}

	return m_bValid;
}

BOOL CFileVersionInfo::QueryVersionTrans( void )
{
	if( m_bValid == FALSE )
		ASSERT_RETURN( FALSE );

	UINT   uLen  = 0;
	LPVOID lpBuf = NULL;
  
	if( ::VerQueryValue( m_lpbyVIB, _T( "\\VarFileInfo\\Translation" ), (LPVOID*)&lpBuf, &uLen ) )
	{
		m_lpdwTrans = (LPDWORD)lpBuf;
		m_nTransCnt = ( uLen / sizeof( DWORD ) );		
	}	
	return (BOOL)( m_lpdwTrans != NULL );
}

void CFileVersionInfo::Close( void )
{
	m_nTransCnt  = 0;
	m_nTransCur  = 0;
	m_bValid	 = FALSE;	
	m_lpdwTrans  = NULL;
		
	::ZeroMemory( &m_vsffi, sizeof( VS_FIXEDFILEINFO ) );
	if ( m_lpbyVIB != NULL)
	{
		delete [] m_lpbyVIB;
		m_lpbyVIB = NULL;
	}
}

BOOL CFileVersionInfo::QueryStringValue( IN  LPCTSTR lpszItem,
										 OUT LPTSTR  lpszValue, 
										 IN  INT     nBuf ) const
{
	if( m_bValid  == FALSE || lpszItem == NULL )
		ASSERT_RETURN( FALSE );
	
	if( lpszValue != NULL && nBuf <= 0 )
		ASSERT_RETURN( FALSE );

	::ZeroMemory( lpszValue, nBuf * sizeof( TCHAR ) );

	TCHAR szSFI[ MAX_PATH ] = { 0 };
	::wsprintf( szSFI, _T( "\\StringFileInfo\\%04X%04X\\%s" ), 
		GetCurLID(), GetCurCP(), lpszItem );

	BOOL   bRes    = FALSE;
	UINT   uLen    = 0;
	LPTSTR lpszBuf = NULL;

	if( ::VerQueryValue( m_lpbyVIB, (LPTSTR)szSFI, (LPVOID*)&lpszBuf, &uLen ) )
	{
		if( lpszValue != NULL && nBuf > 0 )
			bRes = (BOOL)( ::lstrcpyn( lpszValue, lpszBuf, nBuf ) != NULL );
		else
			bRes = TRUE;
	}
	
	return ( bRes );
}

BOOL CFileVersionInfo::QueryStringValue( IN  INT    nIndex, 
										 OUT LPTSTR lpszValue,
										 IN  INT    nBuf ) const
{
	if( nIndex < VI_STR_COMMENTS || 
		nIndex > VI_STR_OLESELFREGISTER ) 
	{ 
		ASSERT_RETURN( FALSE );
	}	
	return QueryStringValue( s_ppszStr[ nIndex ], lpszValue, nBuf );
}

LPCTSTR CFileVersionInfo::GetVerStringName( IN INT nIndex )
{ 
	if( nIndex < VI_STR_COMMENTS || 
		nIndex > VI_STR_OLESELFREGISTER ) 
	{ 
		ASSERT_RETURN( FALSE );
	}
	return (LPCTSTR)s_ppszStr[ nIndex ];
}

INT CFileVersionInfo::FindTrans( IN LANGID wLID,
								 IN WORD   wCP ) const
{	
	if( m_bValid == FALSE )
		ASSERT_RETURN( -1 );

	for( UINT n = 0; n < m_nTransCnt; n++ )
	{
		if( LOWORD( m_lpdwTrans[ n ] ) == wLID && 
			HIWORD( m_lpdwTrans[ n ] ) == wCP  )
		{		
			return n;
		}
	}
	return -1;
}

BOOL CFileVersionInfo::SetTrans( IN LANGID wLID /*LANG_NEUTRAL*/, 
								 IN WORD   wCP  /*WSLVI_CP_UNICODE*/ )
{	
	if( m_bValid == FALSE )
		ASSERT_RETURN( FALSE );

	if( GetCurLID() == wLID && GetCurCP() == wCP )
		return TRUE;

	INT nPos = FindTrans( wLID, wCP );
	if( nPos != -1 ) m_nTransCur = nPos;

	return ( m_nTransCur == (UINT)nPos );
}

DWORD CFileVersionInfo::GetTransByIndex( IN UINT nIndex ) const
{
	if( m_bValid == FALSE || nIndex < 0 || nIndex > m_nTransCnt )
		ASSERT_RETURN( 0 );

	return m_lpdwTrans[ nIndex ];
}

BOOL CFileVersionInfo::SetTransIndex( IN UINT nIndex /*0*/ )
{
	if( m_bValid == FALSE )
		ASSERT_RETURN( FALSE );

	if( m_nTransCur == nIndex )
		return TRUE;

	if( nIndex >= 0 && nIndex <= m_nTransCnt )
		m_nTransCur = nIndex;
	
	return ( m_nTransCur == nIndex );
}

/////////////////////////////////////////////////////////////////////////////
// Static members

// If the LID identifier is unknown, it returns a 
// default string ("Language Neutral"):

BOOL CFileVersionInfo::GetLIDName( IN  WORD   wLID, 
								   OUT LPTSTR lpszName, 
								   IN  INT    nBuf )
{
	if( lpszName == NULL || nBuf <= 0 )
		ASSERT_RETURN( FALSE );

	return (BOOL)::VerLanguageName( wLID, lpszName, nBuf );
}

// If the CP identifier is unknown, it returns a 
// default string ("Unknown"):

BOOL CFileVersionInfo::GetCPName( IN  WORD	   wCP,
								  OUT LPCTSTR* ppszName )
{
	if( ppszName == NULL )
		ASSERT_RETURN( FALSE );

	BOOL bRes = TRUE;	
	*ppszName  = NULL;

	switch ( wCP )
	{
		case VI_CP_ASCII:	 *ppszName = _T( "7-bit ASCII" );				break;
		case VI_CP_JAPAN:	 *ppszName = _T( "Japan (Shift – JIS X-0208)" );break;
		case VI_CP_KOREA:	 *ppszName = _T( "Korea (Shift – KSC 5601)" );	break;
		case VI_CP_TAIWAN:	 *ppszName = _T( "Taiwan (Big5)" );				break;
		case VI_CP_UNICODE:	 *ppszName = _T( "Unicode" );					break;
		case VI_CP_LATIN2:	 *ppszName = _T( "Latin-2 (Eastern European)" );break;
		case VI_CP_CYRILLIC: *ppszName = _T( "Cyrillic" );					break;
		case VI_CP_MULTILNG: *ppszName = _T( "Multilingual" );				break;
		case VI_CP_GREEK:	 *ppszName = _T( "Greek" );						break;
		case VI_CP_TURKISH:	 *ppszName = _T( "Turkish" );					break;
		case VI_CP_HEBREW:	 *ppszName = _T( "Hebrew" );					break;
		case VI_CP_ARABIC:	 *ppszName = _T( "Arabic" );					break;		
		default:			 *ppszName = _T( "Unknown" ); bRes = FALSE;		break;
	}
	return bRes;
}
/////////////////////////////////////////////////////////////////////////////