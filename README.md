# Simple Shell
This is a simple OS shell written inn C for Unix-type systems.

## Description
The shell supports the following:
- Execution of external system programs including their parameters.
- The following built-in commands:
  - <b>cd</b> – change working directory.
  - <b>getpath</b> – print system path.
  - <b>setpath</b> – set system path.
  - <b>history</b> – print history contents (numbered list of commands in history including their parameters in ascending order from least to most recent).
  - <b>!!</b> – invoke the last command from history.
  - <b>!&lt;no&gt;</b> - invoke command with number &lt;no&gt; from history.
  - <b>!-&lt;no&gt;</b> - invoke the command with number the number of the current command minus &lt;no&gt;.
  - <b>alias</b> – print all set aliases (alias plus aliased command).
  - <b>alias &lt;name&gt; &lt;command&gt;</b> - alias name to be the command.
  - <b>unalias &lt;command&gt;</b> - remove any associated alias.
- Persistent history of user commands (up to 20 commands).
- Persistent aliases (up to 10 aliases, loop detection included as well).

## How to use it
The program replaces the standard integrated shell of the Unix-type systems.
Just type the commands that you want to execute.

## Requirements:
- GCC

## Compile:
````
gcc main.c shell.c aliases.c
````
