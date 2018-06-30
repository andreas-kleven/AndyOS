#include <AndyOS.h>
#include <sys/msg.h>
#include "GUI.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

using namespace gui;

class MainWindow : public Window
{
public:
	Color color;
	Color bcolor;

	char terminal_path[256] = "/";
	int x = 0;
	int y = 0;

	String command;
	List<String> history;
	int history_index = 0;

	MainWindow()
		: Window("Terminal", 424, 288)
	{
		color = Color::White;
		bcolor = Color::Black;

		Drawing::Clear(bcolor, gc);
		ResetCommand();
	}

	void KeyPress(KEY_PACKET packet)
	{
		if (packet.code == KEY_BACK)
        {
            if (command.Length() > 0)
            {
                command = command.Remove(command.Length() - 2);
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
				Print("%s", command.ToChar());
			}
		}
		else if (packet.code == KEY_DOWN)
		{
			ClearLine();
			ResetCommand();

			history_index++;

			if (history_index < history.Count())
			{
				command = history[history_index];
				Print("%s", command.ToChar());
			}
			else
			{
				history_index = history.Count();
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
		x = 0;
		command = "";
		Print("%s>", terminal_path);
	}

	void HandleCommand(String cmd)
	{
		history.Add(cmd);
		history_index = history.Count();

		char* saveptr;

		char* arg1 = 0;
		char* arg2 = 0;
		char* arg3 = 0;

		arg1 = strtok_r(cmd.ToChar(), " \t", &saveptr);
		if (arg1) arg2 = strtok_r(0, " \t", &saveptr);
		if (arg2) arg3 = strtok_r(0, " \t", &saveptr);

		if (strcmp(arg1, "ls") == 0)
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
		else if (strcmp(arg1, "cd") == 0)
		{
			if (arg2)
			{
				if (strcmp(arg2, "..") == 0)
				{
					int length = strlen(terminal_path) - 1;

					if (length > 0)
					{
						while (terminal_path[--length] != '/')
							terminal_path[length] = 0;
					}
				}
				else
				{
					int arg2_len = strlen(arg2);

					if (arg2_len > 0 && arg2[0] != '/')
					{
						strcpy(terminal_path + strlen(terminal_path), arg2);

						int length = strlen(terminal_path);
						if (terminal_path[length - 1] != '/')
						{
							terminal_path[length] = '/';
							terminal_path[length + 1] = 0;
						}
					}
				}
			}
			else
			{
				Print("%s\n", terminal_path);
			}
		}
		else if (strcmp(arg1, "read") == 0)
		{
			int path_len = strlen(terminal_path) + strlen(arg2);
			char* path = new char[path_len];
			memset(path, 0, path_len);

			strcpy(path, terminal_path);
			strcpy(path + strlen(terminal_path), arg2);

			char* buf;
			uint32 size = read_file(buf, path);

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
		else if (strcmp(arg1, "open") == 0)
		{
			if (!create_process(arg2)) 
			{
				Print("Could not create process");
			}
		}
		else if (strcmp(arg1, "color") == 0)
		{
			color = (0xFF << 24) | strtol(arg2, 0, 16);
		}
		else if (strcmp(arg1, "clear") == 0)
		{
			y = 0;
			Drawing::Clear(bcolor, gc);
		}
		else
		{
			Print("Invalid command");
		}
	}

	void Print(char* format, ...)
	{
		va_list	args;
		va_start(args, format);

		char buf[256];
		vsprintf(buf, format, args);

		for (int i = 0; i < strlen(buf) + 1; i++)
		{
			Putc(buf[i]);
		}
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
			Drawing::DrawText(x * 8, y * 16, " ", color, bcolor, gc);
			break;

		default:
			char str[] = { c, '\0' };
			Drawing::DrawText(x * 8, y * 16, str, color, bcolor, gc);
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
			Drawing::BitBlt(gc, 0, 16, width, 16 * 15, gc, 0, 0);
			ClearLine();
		}

		y = min(y + 1, gc.height / 16 - 1);
	}

	void ClearLine()
	{
		Drawing::FillRect(0, y * 16, gc.width, 16, bcolor, gc);
	}
};

int main()
{
	Drawing::Init();
	MainWindow* wnd = new MainWindow();

	while (1)
		sleep(1000);

	exit(0);
}