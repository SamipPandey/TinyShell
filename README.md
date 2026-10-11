# 🐚 TinyShell

A small command-line shell written in **C for Linux**.

TinyShell is a personal project I built to understand how a shell works behind the scenes. While working on it, I explored Linux system calls, process creation, command execution, and handling keyboard input directly through the terminal.

It supports basic command execution, a few built-in commands, and command history with arrow-key navigation.

---

## ✨ Features

- 💻 **Command execution** — Run external commands such as `ls`, `pwd`, and `date`.
- 📜 **Command history** — Keep track of up to 100 commands during a session.
- ⬆️ **History navigation** — Use the Up and Down arrow keys to recall previous commands.
- ⌫ **Backspace support** — Edit commands while typing.
- 📂 **Directory navigation** — Change directories using the built-in `cd` command.
- 🕘 **History viewer** — Display saved commands with `history`.
- 🚪 **Exit command** — Leave the shell using `exit`.
- ⚙️ **Process management** — Create and manage child processes using `fork()`, `execvp()`, and `waitpid()`.
- 🖥️ **Terminal input handling** — Read input character by character using `termios` and `read()`.

---

## 🛠️ Built With

| Technology | Purpose |
|---|---|
| C | Main programming language |
| GCC | Compiling the source code |

---

## 🚀 Getting Started

Follow these steps to compile and run TinyShell on Linux.

### 📋 Prerequisites

Make sure you have:

- A Linux or Unix-like operating system
- GCC or another compatible C compiler
- A terminal emulator

Check whether GCC is installed:

```bash
gcc --version
```

### 1️⃣ Clone the repository

```bash
git clone <https://github.com/SamipPandey/TinyShell>
cd <TinyShell>
```

Alternatively, save the source code as `tinyshell.c` in your preferred directory.

### 2️⃣ Compile the program

Run the following command from the directory containing `tinyshell.c`:

```bash
gcc tinyshell.c -o tinyshell
```

This compiles the source code and creates an executable named `tinyshell`.

### 3️⃣ Start TinyShell

```bash
./tinyshell
```

If everything works, you should see the prompt:

```text
<tinyshell>
```

You're now running your own simple shell! 🎉

---

## 💻 Commands

TinyShell can execute external programs available through your system's `PATH` and also supports three built-in commands.

| Command | Description | Example |
|---|---|---|
| `cd` | Change the current working directory | `cd /home` |
| `history` | Display commands entered during the current session | `history` |
| `exit` | Exit TinyShell | `exit` |
| `ls` | List files and directories | `ls -l` |
| `pwd` | Display the current working directory | `pwd` |
| `whoami` | Display the current username | `whoami` |
| `date` | Display the current date and time | `date` |

The commands in the last four rows are external programs, not built-in commands implemented by TinyShell.

### 📂 Changing directories

```text
<tinyshell> cd /home
<tinyshell> pwd
```

The `cd` command uses `chdir()` to change the working directory of the shell process. This is important because changing directories inside a child process would not change the working directory of the parent shell.

### 📜 Viewing command history

```text
<tinyshell> pwd
<tinyshell> ls
<tinyshell> whoami
<tinyshell> history
```

Example output:

```text
1: pwd
2: ls
3: whoami
4: history
```

The history is stored in memory and is cleared when TinyShell exits. Empty commands are not added to the history.

### ⬆️ Navigating command history

You can use the arrow keys to move through previously entered commands.

| Key | Action |
|---|---|
| ⬆️ Up Arrow | Recall an earlier command |
| ⬇️ Down Arrow | Move toward newer commands |
| ⌫ Backspace | Delete the last character |
| ↵ Enter | Submit the command |

You can edit a recalled command before executing it.

### 🚪 Exiting TinyShell

```text
<tinyshell> exit
Have a nice day!
```

---

## ⚙️ How It Works

TinyShell follows a simple loop: read user input, check for built-in commands, execute external commands when needed, and display the prompt again.

### 🔹 1. Reading terminal input

The `read_input()` function reads one character at a time using `read()`.

The program uses `termios` to disable canonical input mode (`ICANON`) and automatic terminal echo (`ECHO`). This makes it possible to handle keyboard input directly, including Backspace and arrow keys.

The original terminal settings are restored when input handling finishes through the registered `atexit()` cleanup function.

### 🔹 2. Storing command history

The program stores commands in a fixed-size array:

```c
static char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
```

The constants define the maximum number of saved commands and the maximum length of each history entry.

The history functions are responsible for:

- Adding non-empty commands.
- Moving backward through saved commands.
- Moving forward through saved commands.
- Printing the complete history.

History is session-based, so it does not persist after the program exits.

### 🔹 3. Parsing commands

The program uses `strtok()` to split the input into space-separated arguments.

For example, the command:

```bash
ls -l
```

is separated into arguments similar to:

```c
args[0] = "ls";
args[1] = "-l";
args[2] = NULL;
```

The resulting argument array is passed to `execvp()`.

This is a basic parser: it does not interpret quoted strings, pipes, redirection, or other shell syntax.

### 🔹 4. Handling built-in commands

TinyShell checks whether the input is `exit`, `history`, or `cd` before attempting to launch an external program.

The built-in commands are handled directly by the shell:

- `exit` ends the main loop.
- `history` prints the saved commands.
- `cd` changes the current working directory using `chdir()`.

Built-in commands need to be handled separately because some operations, particularly changing directories, must affect the shell process itself.

### 🔹 5. Creating and executing processes

For external commands, TinyShell uses `fork()` to create a child process.

The child calls `execvp()` to replace its process image with the requested program. Meanwhile, the parent calls `waitpid()` to wait for the child to finish before displaying the next prompt.

The basic execution flow looks like this:

```text
          User enters a command
                    |
                    v
             Read the input
                    |
                    v
          Check built-in commands
                    |
           +--------+--------+
           |                 |
           v                 v
      Built-in           External
      command             command
           |                 |
           v                 v
      Handle it          fork()
                             |
                     +-------+-------+
                     |               |
                     v               v
                 Child process   Parent process
                     |               |
                     v               v
                  execvp()        waitpid()
                     |               |
                     v               v
                Run program     Show prompt again
```

### 🔹 6. Handling errors

If `chdir()`, `fork()`, or `execvp()` fails, the program reports an error using functions such as `perror()`.

This helps identify problems such as invalid directories, unavailable commands, or process creation failures.

---

## 📁 Project Structure

The project can be kept simple:

```text
tinyshell/
├── tinyshell.c
├── README.md
└── LICENSE
```

- `tinyshell.c` — The C source code.
- `README.md` — Project documentation.
- `LICENSE` — The MIT License.

---

## ⚠️ Current Limitations

TinyShell is intentionally simple and does not yet support some features found in full-featured shells such as Bash or Zsh.

| Feature | Status |
|---|---|
| Basic external commands | ✅ Supported |
| Built-in `cd`, `history`, and `exit` | ✅ Supported |
| Command history navigation | ✅ Supported |
| Backspace handling | ✅ Supported |
| Pipes (`\|`) | ❌ Not supported |
| Input/output redirection (`<`, `>`, `>>`) | ❌ Not supported |
| Background execution (`&`) | ❌ Not supported |
| Quoted arguments | ❌ Not supported |
| Persistent command history | ❌ Not supported |
| Full job control | ❌ Not supported |

Commands are split using spaces, so arguments containing spaces and more advanced shell expressions are not handled correctly.

---

## 📚 What I Learned

Building TinyShell gave me a practical introduction to several concepts that are useful when programming on Linux:

- How a shell launches external programs.
- How parent and child processes work.
- How `fork()`, `execvp()`, and `waitpid()` work together.
- How to read keyboard input directly from a terminal.
- How terminal settings can be changed using `termios`.
- How to parse commands and manage strings in C.
- How built-in shell commands differ from external programs.

It was a good way to get more comfortable with C and understand some of the mechanisms behind a Linux terminal.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

⭐ If you find this project interesting, feel free to explore the code and learn from it.

**Built with C on Linux 🐧**