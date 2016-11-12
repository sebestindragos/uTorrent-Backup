#ifndef _CONSOLEEXCEPTION_H_
#define _CONSOLEEXCEPTION_H_

/**
 * Class used for working with big integer numbers.
 *
 * @author Dragos Sebestin
 */
class ConsoleException
{
public:
  ConsoleException(const wstring & aMessage);

  wstring & What();

private:

  wstring mMessage;
};

#endif // _CONSOLEEXCEPTION_H_
