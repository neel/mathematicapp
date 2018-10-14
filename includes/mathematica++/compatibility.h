#ifndef MATHEMATICAPP_COMPATIBILITY_H
#define MATHEMATICAPP_COMPATIBILITY_H

#ifdef USING_ML

#define wmk_int64_t mlint64

#define WMK_LINK MLINK
#define WMK_ENV  MLENV
#define WMK_MARK MLMARK

#define	WMK_TKERR       MLTKERR
#define	WMK_TKINT       MLTKINT
#define	WMK_TKFUNC      MLTKFUNC
#define	WMK_TKREAL      MLTKREAL
#define	WMK_TKSTR       MLTKSTR
#define	WMK_TKSYM       MLTKSYM
#define	WMK_TKOLDINT    MLTKOLDINT
#define	WMK_TKOLDREAL   MLTKOLDREAL
#define	WMK_TKOLDSTR    MLTKOLDSTR
#define	WMK_TKOLDSYM    MLTKOLDSYM
#define	WMK_TKOPTSTR    MLTKOPTSTR
#define	WMK_TKOPTSYM    MLTKOPTSYM

#define WMK_TK_SHORT    MLTK_MLSHORT
#define WMK_TK_INT      MLTK_MLINT
#define WMK_TK_LONG     MLTK_MLLONG
#define WMK_TK_INT64    MLTK_MLINT64
#define WMK_TK_SIZE_T   MLTK_MLSIZE_T

#define WMK_EOK MLEOK

#define WMK_Close               MLClose
#define WMK_Flush               MLFlush
#define WMK_Error               MLError
#define WMK_ErrorMessage        MLErrorMessage
#define WMK_ParametersPointer   MLParametersPointer
#define WMK_Initialize          MLInitialize
#define WMK_OpenArgcArgv        MLOpenArgcArgv
#define WMK_OpenString          MLOpenString
#define WMK_Activate            MLActivate
#define WMK_LinkName            MLLinkName
#define WMK_ReleaseLinkName     MLReleaseLinkName
#define WMK_WaitForLinkActivity MLWaitForLinkActivity

#define WMK_EndPacket         MLEndPacket
#define WMK_Deinitialize      MLDeinitialize
#define WMK_NewPacket         MLNewPacket
#define WMK_NextPacket        MLNextPacket
#define WMK_GetNext           MLGetNext
#define WMK_TestHead          MLTestHead
#define WMK_GetRawType        MLGetRawType

#define WMK_CreateMark        MLCreateMark
#define WMK_SeekToMark        MLSeekToMark
#define WMK_DestroyMark       MLDestroyMark

#define WMK_PutFunction       MLPutFunction
#define WMK_PutInteger        MLPutInteger
#define WMK_PutLongInteger    MLPutLongInteger
#define WMK_PutReal           MLPutReal
#define WMK_PutString         MLPutString
#define WMK_PutSymbol         MLPutSymbol
#define WMK_GetFunction       MLGetFunction
#define WMK_GetInteger16      MLGetInteger16
#define WMK_GetInteger32      MLGetInteger32
#define WMK_GetInteger64      MLGetInteger64
#define WMK_GetLongInteger    MLGetLongInteger
#define WMK_GetNumberAsString MLGetNumberAsString
#define WMK_GetReal           MLGetReal
#define WMK_GetString         MLGetString
#define WMK_GetSymbol         MLGetSymbol
#define WMK_ReleaseSymbol     MLReleaseSymbol
#define WMK_ReleaseString     MLReleaseString
#define WMK_PutRealNumberAsString MLPutRealNumberAsString

#define WMK_PutInteger8List   MLPutInteger8List
#define WMK_PutInteger16List  MLPutInteger16List
#define WMK_PutInteger32List  MLPutInteger32List
#define WMK_PutInteger64List  MLPutInteger64List
#define WMK_PutReal32List     MLPutReal32List
#define WMK_PutReal64List     MLPutReal64List
#define WMK_PutInteger8Array  MLPutInteger8Array
#define WMK_PutInteger16Array MLPutInteger16Array
#define WMK_PutInteger32Array MLPutInteger32Array
#define WMK_PutInteger64Array MLPutInteger64Array
#define WMK_PutReal32Array    MLPutReal32Array
#define WMK_PutReal64Array    MLPutReal64Array
#else

#define wmk_int64_t wsint64

#define WMK_LINK WSLINK
#define WMK_ENV  WSENV
#define WMK_MARK WSMARK

#define	WMK_TKERR       WSTKERR
#define	WMK_TKINT       WSTKINT
#define	WMK_TKFUNC      WSTKFUNC
#define	WMK_TKREAL      WSTKREAL
#define	WMK_TKSTR       WSTKSTR
#define	WMK_TKSYM       WSTKSYM
#define	WMK_TKOLDINT    WSTKOLDINT
#define	WMK_TKOLDREAL   WSTKOLDREAL
#define	WMK_TKOLDSTR    WSTKOLDSTR
#define	WMK_TKOLDSYM    WSTKOLDSYM
#define	WMK_TKOPTSTR    WSTKOPTSTR
#define	WMK_TKOPTSYM    WSTKOPTSYM

#define WMK_TK_SHORT    WSTK_WSSHORT
#define WMK_TK_INT      WSTK_WSINT
#define WMK_TK_LONG     WSTK_WSLONG
#define WMK_TK_INT64    WSTK_WSINT64
#define WMK_TK_SIZE_T   WSTK_WSSIZE_T

#define WMK_EOK WSEOK

#define WMK_Close               WSClose
#define WMK_Flush               WSFlush
#define WMK_Error               WSError
#define WMK_ErrorMessage        WSErrorMessage
#define WMK_ParametersPointer   WSParametersPointer
#define WMK_Initialize          WSInitialize
#define WMK_OpenArgcArgv        WSOpenArgcArgv
#define WMK_OpenString          WSOpenString
#define WMK_Activate            WSActivate
#define WMK_LinkName            WSLinkName
#define WMK_ReleaseLinkName     WSReleaseLinkName
#define WMK_WaitForLinkActivity WSWaitForLinkActivity

#define WMK_EndPacket         WSEndPacket
#define WMK_Deinitialize      WSDeinitialize
#define WMK_NewPacket         WSNewPacket
#define WMK_NextPacket        WSNextPacket
#define WMK_GetNext           WSGetNext
#define WMK_TestHead          WSTestHead
#define WMK_GetRawType        WSGetRawType

#define WMK_CreateMark        WSCreateMark
#define WMK_SeekToMark        WSSeekToMark
#define WMK_DestroyMark       WSDestroyMark

#define WMK_PutFunction       WSPutFunction
#define WMK_PutInteger        WSPutInteger
#define WMK_PutLongInteger    WSPutLongInteger
#define WMK_PutReal           WSPutReal
#define WMK_PutString         WSPutString
#define WMK_PutSymbol         WSPutSymbol
#define WMK_GetFunction       WSGetFunction
#define WMK_GetInteger16      WSGetInteger16
#define WMK_GetInteger32      WSGetInteger32
#define WMK_GetInteger64      WSGetInteger64
#define WMK_GetLongInteger    WSGetLongInteger
#define WMK_GetNumberAsString WSGetNumberAsString
#define WMK_GetReal           WSGetReal
#define WMK_GetString         WSGetString
#define WMK_GetSymbol         WSGetSymbol
#define WMK_ReleaseSymbol     WSReleaseSymbol
#define WMK_ReleaseString     WSReleaseString
#define WMK_PutRealNumberAsString WSPutRealNumberAsString

#define WMK_PutInteger8List   WSPutInteger8List
#define WMK_PutInteger16List  WSPutInteger16List
#define WMK_PutInteger32List  WSPutInteger32List
#define WMK_PutInteger64List  WSPutInteger64List
#define WMK_PutReal32List     WSPutReal32List
#define WMK_PutReal64List     WSPutReal64List
#define WMK_PutInteger8Array  WSPutInteger8Array
#define WMK_PutInteger16Array WSPutInteger16Array
#define WMK_PutInteger32Array WSPutInteger32Array
#define WMK_PutInteger64Array WSPutInteger64Array
#define WMK_PutReal32Array    WSPutReal32Array
#define WMK_PutReal64Array    WSPutReal64Array
#endif

#endif // MATHEMATICAPP_COMPATIBILITY_H
