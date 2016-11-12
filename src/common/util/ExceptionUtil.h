#ifndef _EXCEPTION_UTIL_H_
#define _EXCEPTION_UTIL_H_

#ifdef _WINEXEPTION_H_
  #define CATCH_WIN_EXCEPTION \
  catch (Exception e)\
  {\
    ::MessageBox(e.Where(), e.What().c_str(), L"Error", MB_ICONERROR | MB_OK); \
  }
#else
  #define CATCH_WIN_EXCEPTION 
#endif

#ifdef _CONSOLEEXCEPTION_H_
  #define CATCH_CONSOLE_EXCEPTION \
  catch (ConsoleException e)\
  {\
  wcout << e.What(); \
  }
#else
  #define CATCH_CONSOLE_EXCEPTION 
#endif

#define CATCH_EVERYTHING \
  CATCH_WIN_EXCEPTION \
  CATCH_CONSOLE_EXCEPTION \
  catch (std::exception e)\
  {\
    ::MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR | MB_OK);\
  }\
  catch (...)\
  {\
    ::MessageBox(NULL, L"The application ran into a problem that in couldn't handle.",\
    L"Error", MB_ICONERROR | MB_OK);\
  }

#endif // _EXCEPTION_UTIL_H_
