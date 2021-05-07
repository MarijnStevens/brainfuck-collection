qdb is a quick and dirty brainfuck interpreter, meant to be run
from the command line (by typing "qdb whatever.b" or "qdb.exe
whatever.b", where "whatever.b" is replaced with the filename or
pathname of a brainfuck program). In getting this to run on
Windows, I changed fopen() to fopen_s() to make Visual Studio
happy, and also bumped up the max program size to 640k on
impulse. (Array size is 16 megabytes.) I include the source,
which you're free to tweak as you like. It seems to translate
newlines as ASCII 10 properly, presumably because it opens stdin
and stdout in text mode by default, and it leaves the cell
unchanged on EOF (end-of-file), which (in Windows) you send from
the keyboard by typing CTRL-Z followed by ENTER.

Best of luck!
Daniel Cristofani
http://brainfuck.org
