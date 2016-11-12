#ifndef _AUTOHANDLE_H_
#define _AUTOHANDLE_H_

/*
 * Class implementing an autohandle object which uses RAII idiom. 
 *
 * @author Sebestin Dragos
 */
class AutoHandle
{
public:

  /**
    * Class constructors.
    */
  AutoHandle();
  AutoHandle(HANDLE aHandle);
  ~AutoHandle();

  /*
   * Asignment operator.
   */
  HANDLE operator = (HANDLE aHandle);

  operator HANDLE();

private:
  HANDLE mHandle;
}; // AutoHandle

#endif // _AUTOHANDLE_H_
