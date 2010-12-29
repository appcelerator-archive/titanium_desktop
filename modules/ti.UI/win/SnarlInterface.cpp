// About:
//   Snarl C++ interface implementation
//   To understand what the different functions do and what they return, please
//   have a look at the API on http://www.fullphat.net/dev/api.htm.
//   Also have a look at mSnarl_i.bas found in the CVS/SVN repository for Snarl.
//
//   The functions in SnarlInterface both have ANSI(UTF8) and UNICODE versions.
//   If the LPCWSTR (unicode) version of the functions are called, the strings
//   are converted to UTF8 by SnarlInterface before sent to Snarl. So using the
//   ANSI/UTF8/LPCSTR versions of the functions are faster!
//   
//
// Difference to VB implementation:
//   Please note that string functions return NULL when they fail and not an
//   empty string. So check for NULL...
//   Function names doesn't have the pre "sn".
//
// 
// Authors:
//   Written and maintained by Toke Noer N