#include "SecretCin.h"

#include <iostream>

#ifdef WIN32
#	include <windows.h>
#else
#	include <termios.h>
#	include <unistd.h>
#endif

namespace gcpp::input
{

void SetStdinEcho(bool enable = true)
{
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);

	if(!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;

	SetConsoleMode(hStdin, mode);
#else
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if(!enable)
	{
		tty.c_lflag &= ~ECHO;
	}
	else
	{
		tty.c_lflag |= ECHO;
	}

	(void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

std::string SecretCin::readPassword()
{
	std::string password;
	SetStdinEcho(false);

	std::cin >> password;

	SetStdinEcho(true);

	// From ASCII
	// end of text	ctrl-C	3	03	ETX
	// end of xmit	ctrl-D	4	04	EOT
	if(password.empty() == false && (password.back() == 0x03 || password.back() == 0x04))
	{
		// Password should be treated as empty, when user cancel input
		// This seems isn't executed when using std::cin :)
		return "";
	}
	return password;
}

} // namespace gcpp::input
