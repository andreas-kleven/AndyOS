#include "GUI.h"
#include "math.h"
#include <AndyOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace gui;

void btnClick();

class MainWindow : public Window
{
  public:
    Label *label;
    Button *button;
    TextBox *textBox;

    MainWindow(char *title) : Window(title)
    {
        label = new Label("Label");
        button = new Button("A button");
        textBox = new TextBox();

        button->bounds = Rect(100, 200, 80, 20);
        button->OnClick = btnClick;

        textBox->bounds.x = 10;
        textBox->bounds.y = 30;

        AddChild(label);
        AddChild(button);
        AddChild(textBox);
    }

    void OnClose() { exit(0); }
};

MainWindow *wnd;

int clicks = 0;
void btnClick()
{
    char buf[100];
    sprintf(buf, "Clicked %i", ++clicks);
    wnd->label->text = buf;
}

int main()
{
    char title[256];
    sprintf(title, "Test window: %u", get_ticks());
    wnd = new MainWindow(title);

    while (true)
        sleep(1000);
    return 0;
}
