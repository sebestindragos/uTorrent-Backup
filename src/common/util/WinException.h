#ifndef _WINEXEPTION_H_
#define _WINEXEPTION_H_

/**
 * Class used for throwing custom exceptions.
 */
class Exception
{
public:

  /**
  * Class constructors.
  */
  Exception();
  Exception(HWND aWindow, const wstring & aMessage);

  /**
  * Method for retrieving the message of the exception.
  */
  wstring What();
  HWND Where();

private:

  wstring mMessage;
  HWND    mWindow;
};

#endif // _WINEXEPTION_H_
