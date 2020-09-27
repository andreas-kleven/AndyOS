#include "GUI.h"
#include <AndyOS.h>
#include <algorithm>
#include <andyos/math.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <netinet/in.h>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

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

    int program_fd = 0;

    MainWindow() : Window("Terminal", 640, 288, Color::Black)
    {
        color = Color::White;
        bcolor = Color::Black;

        gc.Clear(bcolor);
        ResetCommand();

        Open("/bin/dash", "", "", "");
    }

    void OnClose() { exit(0); }

    void KeyPress(KEY_PACKET packet)
    {
        if (packet.code == KEY_BACK) {
            if (command.length() > 0) {
                command.pop_back();
                Putc('\b');
            }
        } else if (packet.code == KEY_RETURN) {
            Putc('\n');

            history.push_back(command);
            history_index = history.size();

            if (program_fd) {
                write(program_fd, command.c_str(), command.length());
                write(program_fd, "\n", 1);
            } else {
                HandleCommand(command);
            }

            ResetCommand();
        } else if (packet.code == KEY_UP) {
            if (history_index > 0) {
                ClearLine();
                ResetCommand();

                history_index -= 1;
                command = history[history_index];
                PrintString(command);
            }
        } else if (packet.code == KEY_DOWN) {
            ClearLine();
            ResetCommand();

            history_index++;

            if (history_index < history.size()) {
                command = history[history_index];
                PrintString(command);
            } else {
                history_index = history.size();
                command = "";
            }
        } else {
            if (isprint(packet.character)) {
                command += packet.character;
                Putc(packet.character);
            }
        }
    }

    void ResetCommand()
    {
        command = "";

        if (!program_fd) {
            x = 0;
            PrintString(terminal_path);
            Print(">");
        }
    }

    void HandleCommand(std::string cmd)
    {
        std::istringstream iss(cmd);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};

        std::string arg1 = tokens.size() > 0 ? tokens[0] : "";
        std::string arg2 = tokens.size() > 1 ? tokens[1] : "";
        std::string arg3 = tokens.size() > 2 ? tokens[2] : "";
        std::string arg4 = tokens.size() > 3 ? tokens[3] : "";
        std::string arg5 = tokens.size() > 4 ? tokens[4] : "";

        if (arg1 == "ls") {
            char buf[1024];
            int fd = open(terminal_path.c_str(), O_RDONLY /* | O_DIRECTORY*/);

            while (true) {
                int read = getdents(fd, (dirent *)buf, sizeof(buf));
                Print("Read %d\n", read);

                if (read == 0)
                    break;

                if (read < 0) {
                    Print("Error\n");
                    break;
                }

                int pos = 0;
                while (pos < read) {
                    dirent *dirp = (dirent *)&buf[pos];
                    Print("%d %d %d %s\n", dirp->d_ino, dirp->d_type, dirp->d_reclen, dirp->d_name);
                    pos += dirp->d_reclen;
                }
            }

            close(fd);
        } else if (arg1 == "cd") {
            if (arg2.length()) {
                if (arg2 == "..") {
                    if (terminal_path_parts.size() > 0)
                        terminal_path_parts.pop_back();
                } else {
                    terminal_path_parts.push_back(arg2);
                }

                terminal_path = FormatPath(terminal_path_parts);
            } else {
                PrintString(terminal_path);
            }
        } else if (arg1 == "cat") {
            std::string path = terminal_path + "/" + arg2;

            FILE *file = fopen(path.c_str(), "r");

            if (!file) {
                Print("Open error\n");
                return;
            }

            char buf[1024];

            while (true) {
                int read = fread(buf, 1, sizeof(buf), file);

                if (read < 0) {
                    Print("Read error\n");
                    break;
                }

                if (read == 0)
                    break;

                for (int i = 0; i < read; i++)
                    Putc(buf[i]);
            }

            fclose(file);
        } else if (arg1 == "open") {
            Open(arg2.c_str(), arg3.c_str(), arg4.c_str(), arg5.c_str());
        } else if (arg1 == "color") {
            color = (0xFF << 24) | strtol(arg2.c_str(), 0, 16);
        } else if (arg1 == "clear") {
            y = 0;
            gc.Clear(bcolor);
        } else if (arg1 == "test") {
            FILE *file = fopen("text.txt", "r");
            Print("File: %X\n", file);

            if (file) {
                int len = 20;
                char data[len];
                memset(data, 0, len);

                if (fread(data, 1, len, file) != -1) {
                    debug_print("%s\n", data);
                    for (int i = 0; i < len; i++)
                        Putc(data[i]);
                } else {
                    Print("Read error\n");
                }

                fclose(file);

                Print("\n");
                if ((file = fopen("/dev/mouse", "")) != 0) {
                    memset(data, 0, 4);

                    while (1) {
                        if (fread(data, 1, 4, file) != -1) {
                            for (int i = 0; i < 4; i++)
                                Print("%X ", (uint8_t)data[i]);
                            Print("\n");
                        }
                    }

                    fclose(file);
                } else {
                    Print("Open error\n");
                }
            }
        } else if (arg1 == "pipe") {
            int pipefd[2];
            pipe(pipefd);

            Print("%i\t%i\n", pipefd[0], pipefd[1]);

            int len = 100;
            char buf[len];
            memset(buf, 0, len);

            const char *str = "Pipe data ...\n";
            int written = write(pipefd[1], str, strlen(str));
            Print("Written: %i\n", written);

            int counter = 0;

            while (1) {
                Print("Wait...\n");
                int l = read(pipefd[0], buf, len);
                Print("Read %d\n", l);

                if (l == 0 || counter == 100) {
                    close(pipefd[0]);
                    close(pipefd[1]);
                    Print("Done\n");
                    break;
                }

                if (l > 0) {
                    for (int i = 0; i < l; i++)
                        Putc(buf[i]);

                    char sb[100];
                    itoa(counter++, sb, 10);
                    sb[strlen(sb) + 1] = 0;
                    sb[strlen(sb)] = '\n';
                    write(pipefd[1], sb, strlen(sb));
                } else {
                    Print("Read error\n");
                    // usleep(1000);
                }
            }

            // close(pipefd[0]);
            // close(pipefd[1]);
        } else if (arg1 == "udpc") {
            int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            if (sockfd < 0) {
                debug_print("Error %d\n", sockfd);
                return;
            }

            sockaddr_in clientaddr;
            clientaddr.sin_family = AF_INET;
            clientaddr.sin_addr.s_addr = htonl(0xC0A8007B); // 192.168.0.123
            clientaddr.sin_port = htons(8080);

            const char *msg = arg2.c_str();
            int r =
                sendto(sockfd, msg, strlen(msg), 0, (sockaddr *)&clientaddr, sizeof(sockaddr_in));
            debug_print("%d\n", r);
        } else if (arg1 == "udpl") {
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            if (sockfd < 0) {
                debug_print("Error %d\n", sockfd);
                return;
            }

            sockaddr_in servaddr;
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = INADDR_ANY;
            servaddr.sin_port = htons(10001);

            int b = bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));

            if (b < 0) {
                debug_print("Bind error %d\n", b);
                return;
            }

            debug_print("Waiting...\n");

            while (true) {
                char buf[256];
                int len = recv(sockfd, buf, sizeof(buf) - 1, 0);
                buf[len] = 0;
                Print("%s\n", buf);
            }
        } else {
            Print("Invalid command");
        }
    }

    void Open(const char *file, const char *arg1, const char *arg2, const char *arg3)
    {
        int fd1[2];
        int fd2[2];

        if (pipe(fd1) == -1) {
            debug_print("Pipe Failed");
            return;
        }

        if (pipe(fd2) == -1) {
            debug_print("Pipe Failed");
            return;
        }

        pid_t p = fork();
        debug_print("pid:%d\n", p);

        if (p < 0) {
            debug_print("fork Failed");
            return;
        } else if (p > 0) {
            close(fd1[1]);
            close(fd2[0]);

            program_fd = fd2[1];

            char buf[BUF_SIZE];

            while (true) {
                int len = read(fd1[0], buf, BUF_SIZE);

                if (len <= 0)
                    break;

                for (int i = 0; i < len; i++)
                    Putc(buf[i]);
            }

            program_fd = 0;

            close(fd1[0]);
            close(fd2[1]);
            Print("Exited\n");
        } else {
            close(fd1[0]);
            close(fd2[1]);

            int fd_stdout = dup2(fd1[1], STDOUT_FILENO);
            int fd_stdin = dup2(fd2[0], STDIN_FILENO);
            dup2(STDOUT_FILENO, STDERR_FILENO);

            if (fd_stdout == STDOUT_FILENO && fd_stdin == STDIN_FILENO) {
                char *const argv[] = {(char *)file, (char *)(strcmp(arg1, "") ? arg1 : NULL),
                                      (char *)(strcmp(arg2, "") ? arg2 : NULL),
                                      (char *)(strcmp(arg3, "") ? arg3 : NULL), NULL};
                char *const envp[] = {/*"PATH=",*/ NULL};
                execve(file, argv, envp);

                // execl(file, file, arg1, arg2, arg3, NULL);
            } else {
                debug_print("Error\n");
            }

            exit(0);
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

        for (int i = 0; i < strlen(buffer); i++) {
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
        switch (c) {
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

        if (x > gc.width / 8) {
            x = 0;
            MoveDown();
        }
    }

    void MoveDown()
    {
        if (y >= gc.height / 16 - 1) {
            gc.CopyTo(0, 16, width, 16 * (height / 16), gc, 0, 0);
            ClearLine();
        }

        y = min(y + 1, gc.height / 16 - 1);
    }

    void ClearLine() { gc.FillRect(0, y * 16, gc.width, 16, bcolor); }
};

int main()
{
    MainWindow *wnd = new MainWindow();

    while (true)
        sleep(1000);
}
