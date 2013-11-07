
#if	!defined(_MFC_VER)
	#if	defined(_DEBUG)
		#pragma	comment( lib, "psdflib_db.lib" )
	#else
		#pragma	comment( lib, "psdflib.lib" )
	#endif
#else
	#if	defined(_DEBUG)
		#pragma	comment( lib, "psdflib_mfcdb.lib" )
	#else
		#pragma	comment( lib, "psdflib_mfc.lib" )
	#endif
#endif
