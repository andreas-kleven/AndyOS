#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <AndyOS.h>
#include <andyos/msg.h>
#include <andyos/math.h>
#include "GUI.h"

#define BUF_SIZE 256

using namespace gui;

class MainWindow : public Window
{
public:
	Color color;
	Color bcolor;

	std::string command;
	std::vector<std::string> terminal_path_parts;
	std::string terminal_path;
	std::vector<std::string> history;

	int history_index = 0;
	int x = 0;
	int y = 0;

	MainWindow()
		: Window("Terminal", 424, 288, Color::Black)
	{
		color = Color::White;
		bcolor = Color::Black;

		gc.Clear(bcolor);
		ResetCommand();
	}

	void OnClose()
	{
		exit(0);
	}

	void KeyPress(KEY_PACKET packet)
	{
		if (packet.code == KEY_BACK)
		{
			if (command.length() > 0)
			{
				command.pop_back();
				Putc('\b');
			}
		}
		else if (packet.code == KEY_RETURN)
		{
			Putc('\n');
			HandleCommand(command);
			Putc('\n');
			ResetCommand();
		}
		else if (packet.code == KEY_UP)
		{
			if (history_index > 0)
			{
				ClearLine();
				ResetCommand();

				history_index -= 1;
				command = history[history_index];
				PrintString(command);
			}
		}
		else if (packet.code == KEY_DOWN)
		{
			ClearLine();
			ResetCommand();

			history_index++;

			if (history_index < history.size())
			{
				command = history[history_index];
				PrintString(command);
			}
			else
			{
				history_index = history.size();
				command = "";
			}
		}
		else
		{
			if (packet.character)
			{
				command += packet.character;
				Putc(packet.character);
			}
		}
	}

	void ResetCommand()
	{
		command = "";
		x = 0;
		PrintString(terminal_path);
		Print(">");
	}

	void HandleCommand(std::string cmd)
	{
		history.push_back(cmd);
		history_index = history.size();

		std::istringstream iss(cmd);
		std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

		std::string arg1 = tokens.size() > 0 ? tokens[0] : "";
		std::string arg2 = tokens.size() > 1 ? tokens[1] : "";
		std::string arg3 = tokens.size() > 2 ? tokens[2] : "";

		if (arg1 == "ls")
		{
			/*FILE_INFO* files;
			DIRECTORY_INFO* dirs;
			int file_count;
			int dir_count;

			VFS::List(terminal_path, files, dirs, file_count, dir_count);

			for (int i = 0; i < file_count; i++)
			{
				Debug::Print("%s\n", files[i].name);
			}

			for (int i = 0; i < dir_count; i++)
			{
				Debug::Print("%s\n", dirs[i].name);
			}*/
		}
		else if (arg1 == "cd")
		{
			if (arg2.length())
			{
				if (arg2 == "..")
				{
					if (terminal_path_parts.size() > 0)
						terminal_path_parts.pop_back();
				}
				else
				{
					terminal_path_parts.push_back(arg2);
				}

				terminal_path = FormatPath(terminal_path_parts);
			}
			else
			{
				PrintString(terminal_path);
			}
		}
		else if (arg1 == "read")
		{
			std::string path = terminal_path + "/" + arg2;

			char *buf;
			uint32_t size = read_file(buf, path.c_str());;

			if (size == 0)
			{
				Print("File not found\n");
			}
			else
			{
				for (int i = 0; i < size; i++)
				{
					Putc(buf[i]);
				}
			}
		}
		else if (arg1 == "open")
		{
			Open(arg2.c_str());
		}
		else if (arg1 == "color")
		{
			color = (0xFF << 24) | strtol(arg2.c_str(), 0, 16);
		}
		else if (arg1 == "clear")
		{
			y = 0;
			gc.Clear(bcolor);
		}
		else if (arg1 == "test")
		{
			FILE *file = fopen("text.txt", "r");
			Print("File: %X\n", file);

			if (file)
			{
				int len = 20;
				char data[len];
				memset(data, 0, len);

				if (fread(data, 1, len, file) != -1)
				{
					debug_print("%s\n", data);
					for (int i = 0; i < len; i++)
						Putc(data[i]);
				}
				else
				{
					Print("Read error\n");
				}

				fclose(file);

				Print("\n");
				if ((file = fopen("/dev/mouse", "")) != 0)
				{
					memset(data, 0, 4);

					while (1)
					{
						if (fread(data, 1, 4, file) != -1)
						{
							for (int i = 0; i < 4; i++)
								Print("%X ", (uint8_t)data[i]);
							Print("\n");
						}
					}

					fclose(file);
				}
				else
				{
					Print("Open error\n");
				}
			}
		}
		else if (arg1 == "pipe")
		{
			int pipefd[2];
			pipe(pipefd);

			Print("%i\t%i\n", pipefd[0], pipefd[1]);

			int len = 6;
			char buf[len];
			memset(buf, 0, len);

			const char *str = "Pipe data ...\n";
			int written = write(pipefd[1], str, strlen(str));
			Print("Written: %i\n", written);

			int counter = 0;

			while (1)
			{
				int l = read(pipefd[0], buf, len);

				if (l > 0)
				{
					for (int i = 0; i < l; i++)
						Putc(buf[i]);
				}
				else if (l == 0)
				{
					char sb[100];
					itoa(counter++, sb, 10);
					write(pipefd[1], sb, strlen(sb));
				}
				else
				{
					Print("Read error");
					//usleep(1000);
				}
			}

			//close(pipefd[0]);
			//close(pipefd[1]);
		}
		else
		{
			Print("Invalid command");
		}
	}

	void Open(const char *file)
	{
		int pipefd[2];
		pid_t pid;

		if (pipe(pipefd) == -1)
		{
			Print("Error\n");
			return;
		}

		if ((pid = fork()) == -1)
		{
			Print("Error\n");
			return;
		}

		debug_print("pid: %i\n", pid);

		if (pid == 0)
		{
			close(pipefd[1]);

			char buf[BUF_SIZE];

			while (true)
			{
				int len = read(pipefd[0], buf, BUF_SIZE);

				if (len == -1)
					break;

				for (int i = 0; i < len; i++)
					Putc(buf[i]);

				usleep(10);
			}
		}
		else
		{
			int fd = dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);

			if (fd == STDOUT_FILENO)
			{
				char *argv[] = {(char *)file, NULL};
				char *envp[] = {NULL};
				execve(file, argv, envp);
				//execl(file, file, 0);
			}
			else
			{
				debug_print("Error\n");
			}

			while (1)
			{
			}
		}
	}

	std::string FormatPath(const std::vector<std::string> &path)
	{
		std::ostringstream imploded;
		std::copy(path.begin(), path.end(), std::ostream_iterator<std::string>(imploded, "/"));
		std::string formatted = imploded.str();

		if (formatted.length() > 0)
			formatted.pop_back(); // remove trailing delimiter

		return imploded.str();
	}

	void Print(const char *format, ...)
	{
		char buffer[BUF_SIZE];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);

		for (int i = 0; i < strlen(buffer); i++)
		{
			Putc(buffer[i]);
		}
	}

	void PrintString(const std::string &s)
	{
		for (char const &c : s)
			Putc(c);
	}

	void Putc(char c)
	{
		switch (c)
		{
		case '\n':
			x = 0;
			MoveDown();
			break;

		case '\r':
			x = 0;
			break;

		case '\t':
			Putc(' ');
			while (x % 4)
				Putc(' ');
			break;

		case '\b':
			x = clamp(x - 1, 0, gc.width / 8);
			gc.DrawText(x * 8, y * 16, " ", color, bcolor);
			break;

		default:
			char str[] = {c, '\0'};
			gc.DrawText(x * 8, y * 16, str, color, bcolor);
			x++;
			break;
		}

		if (x > gc.width / 8)
		{
			x = 0;
			MoveDown();
		}
	}

	void MoveDown()
	{
		if (y >= gc.height / 16 - 1)
		{
			gc.CopyTo(0, 16, width, 16 * (height / 16), gc, 0, 0);
			ClearLine();
		}

		y = min(y + 1, gc.height / 16 - 1);
	}

	void ClearLine()
	{
		gc.FillRect(0, y * 16, gc.width, 16, bcolor);
	}
};

int main()
{
	Drawing::Init();
	MainWindow *wnd = new MainWindow();

	while (true)
		usleep(100);
}