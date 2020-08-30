#include <Drivers/tty.h>
#include <Drivers/vt100.h>
#include <Drivers/vtty.h>
#include <Kernel/timer.h>
#include <Process/scheduler.h>
#include <circbuf.h>
#include <ctype.h>
#include <debug.h>
#include <errno.h>
#include <hal.h>
#include <keycodes.h>
#include <limits.h>

#define NUM_TERMINALS 8

namespace VTTY {
Vt100Driver *terminals[NUM_TERMINALS];
int termid = 0;

THREAD *listen_thread;
CircularDataBuffer *input_buffer;
KEYCODE extended_scancodes[256];

bool ctrl = false;
bool shift = false;
bool alt = false;
bool caps = false;
bool num = false;
bool scroll = false;
bool extended = false;
KEYCODE code = KEY_INVALID;
KEYCODE current_key;

char DecodeCharacter()
{
    int key = current_key;

    if (!isprint(key)) {
        switch (key) {
        case KEY_SPACE:
            return ' ';

        case KEY_TAB:
            return '\t';

        case KEY_BACK:
            return '\b';

        case KEY_RETURN:
            return '\n';

        default:
            return 0;
        }
    }

    if (isupper(key)) {
        if (shift == caps)
            key += 32;
    } else if (isdigit(key)) {
        if (shift) {
            switch (key) {
            case KEY_D0:
                return '=';
            case KEY_D1:
                return '!';
            case KEY_D2:
                return '"';
            case KEY_D3:
                return '#';
            case KEY_D4:
                return '¤';
            case KEY_D5:
                return '%';
            case KEY_D6:
                return '&';
            case KEY_D7:
                return '/';
            case KEY_D8:
                return '(';
            case KEY_D9:
                return ')';
            }
        }
    } else {
        if (shift) {
            switch (key) {
            case KEY_BAR:
                return '§';
            case KEY_PLUS:
                return '?';
            case KEY_BACKSLASH:
                return '`';
            case KEY_CARET:
                return '^';
            case KEY_QUOTE:
                return '*';
            case KEY_LESS:
                return '>';
            case KEY_COMMA:
                return ';';
            case KEY_DOT:
                return ':';
            case KEY_MINUS:
                return '_';
            }
        } else {
            switch (key) {
            case KEY_BAR:
                return '|';
            case KEY_PLUS:
                return '+';
            case KEY_BACKSLASH:
                return '\\';
            case KEY_CARET:
                return '^';
            case KEY_QUOTE:
                return '\'';
            case KEY_LESS:
                return '<';
            case KEY_COMMA:
                return ',';
            case KEY_DOT:
                return '.';
            case KEY_MINUS:
                return '-';

            case KEY_SPACE:
                return ' ';
            case KEY_TAB:
                return '\t';
            case KEY_RETURN:
                return '\n';
            case KEY_BACK:
                return '\b';
            }
        }
    }

    return key;
}

void EmitKey()
{
    if (current_key == KEY_INVALID)
        return;

    char c = DecodeCharacter();

    if (c) {
        Vt100Driver *terminal = terminals[termid];

        /*if (alt)
        {
            char esc = 0x1B;
            terminal->HandleInput(&esc, 1);
        }*/

        if (ctrl) {
            if (c >= 'a')
                c -= 'a' - 'A';

            if (c == ' ')
                c = 0;
            else if (c >= '@' && c <= '~')
                c = c - '@';
        }

        terminal->HandleInput(&c, 1);
    }
}

void HandleInput(uint8 scan)
{
    if (scan == 0xE0 || scan == 0xE1) {
        extended = true;
    } else {
        if (extended) {
            extended = false;
            code = extended_scancodes[scan & ~0x80];
        } else {
            code = scancodes[scan & ~0x80];
        }

        bool pressed = !(scan & 0x80);

        if (pressed) {
            if (code == KEY_CAPS)
                caps = !caps;
            else if (code == KEY_NUMLOCK)
                num = !num;
            else if (code == KEY_SCROLLLOCK)
                scroll = !scroll;

            else if (code == KEY_LCTRL || code == KEY_RCTRL)
                ctrl = true;
            else if (code == KEY_LSHIFT || code == KEY_RSHIFT)
                shift = true;
            else if (code == KEY_LALT || code == KEY_RALT)
                alt = true;

            else {
                if (alt) {
                    int id = (int)code - (int)KEY_F1;

                    if (id >= 0 && id < NUM_TERMINALS && (int)current_key <= KEY_F12) {
                        SwitchTerminal(id);
                    } else if (code == KEY_LEFT) {
                        SwitchTerminal(termid - 1);
                    } else if (code == KEY_RIGHT) {
                        SwitchTerminal(termid + 1);
                    }
                } else {
                    current_key = code;
                }
            }
        } else {
            scan &= ~0x80;

            if (code == KEY_LCTRL || code == KEY_RCTRL)
                ctrl = false;
            else if (code == KEY_LSHIFT || code == KEY_RSHIFT)
                shift = false;
            else if (code == KEY_LALT || code == KEY_RALT)
                alt = false;

            else if (code == current_key) {
                current_key = KEY_INVALID;
            }
        }
    }
}

Vt100Driver *CurrentTerminal()
{
    return terminals[termid];
}

int SwitchTerminal(int id, bool force)
{
    if (id < 0 || id >= NUM_TERMINALS)
        return -ENOENT;

    if (id == termid && !force)
        return 0;

    terminals[termid]->Deactivate();
    termid = id;
    terminals[termid]->Activate();

    debug_print("Switch terminal %d\n", id);
    return 0;
}

void SetupScancodes()
{
    extended_scancodes[0x48] = KEY_UP;
    extended_scancodes[0x49] = KEY_PAGEUP;
    extended_scancodes[0x4B] = KEY_LEFT;
    extended_scancodes[0x4D] = KEY_RIGHT;
    extended_scancodes[0x50] = KEY_DOWN;
    extended_scancodes[0x51] = KEY_PAGEDOWN;
}

void SetupTerminals()
{
    for (int i = 0; i < NUM_TERMINALS; i++) {
        terminals[i] = new Vt100Driver();
        TtyDriver *tty = new TtyDriver(terminals[i], i);
        DriverManager::AddDriver(tty);
    }

    SwitchTerminal(0, true);
}

void Start()
{
    listen_thread = Scheduler::CurrentThread();
    input_buffer = new CircularDataBuffer(256);

    uint64 press_time = 0;
    uint64 sleep_time = 0;

    SetupTerminals();
    SetupScancodes();

    while (true) {
        uint64 time = Timer::Ticks();

        if (input_buffer->IsEmpty()) {
            if (sleep_time)
                Scheduler::SleepThread(sleep_time, listen_thread);
            else
                Scheduler::SleepThread(ULLONG_MAX, listen_thread);
        }

        uint8 scan = 0;

        disable();

        if (!input_buffer->IsEmpty())
            input_buffer->Read(1, &scan);

        enable();

        KEYCODE last_key = current_key;

        if (scan)
            HandleInput(scan);

        if (current_key) {
            if (current_key != last_key) {
                press_time = Timer::Ticks();
                sleep_time = press_time + 500000;
                EmitKey();
            }

            if (sleep_time <= time) {
                EmitKey();

                while (sleep_time < time)
                    sleep_time += 100000;
            }
        } else {
            press_time = 0;
            sleep_time = 0;
        }
    }
}

void QueueInput(uint8 scan)
{
    if (listen_thread) {
        input_buffer->Write(&scan, 1);
        Scheduler::SleepThread(0, listen_thread);
    }
}
}; // namespace VTTY
