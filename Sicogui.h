//
// Created by Sabbrumm on 27.03.2023.
//

#ifndef CNOVA_DB_SICOGUI_H
#define CNOVA_DB_SICOGUI_H

#define KEY_ESC 27
#define KEY_CONTROL 224
#define KEY_BACKSPACE 8
#define KEY_ENTER 13
#define KEY_CONTROL_DOWN 80
#define KEY_CONTROL_UP 72

#define KEY_CONTROL_RIGHT 77
#define KEY_CONTROL_LEFT 75
#define KEY_0 48
#define KEY_9 57

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100



#include <string>
#include <algorithm>
#include <functional>

using namespace std;

void ConsoleColorWhite(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN);
}
void ConsoleColorBlack(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
}

int diglength(int n) {
    if (n == 0) return 1;
    int l = (n>0)?1:0;
    while(n) {
        l++;
        n /= 10;
    }
    return l;
}

struct ScreenResolution{
    unsigned int width;
    unsigned int height;
    ScreenResolution(){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    ScreenResolution(unsigned int w, unsigned int h){
        width=w,height=h;
    }
};

void SetCur(short x, short y){
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

int unicode_len(std::string s){
    return (s.length() - count_if(s.begin(), s.end(), [](char c)->bool { return (c & 0xC0) == 0x80; }));
}

enum OutputType{
    ot_String, ot_Int
};

class Output{
    public:
        OutputType type = ot_Int;
        string before = "";
        void* variable = nullptr;
        Output(OutputType _type, string bf, char var[] = nullptr){
            before = bf;
            type = _type;
            variable = var;
        }

        Output(OutputType _type, string bf, void* var = nullptr){
            before = bf;
            type = _type;
            variable = var;
        }

};


class Header{
    public:
        string Inside;
        Header(string i){Inside=i;}
};


class Button{
    bool selected = false;
    public:
        string Label = "";
        function<void()> *Function = nullptr;

        Button(string l){
            Label = l;
        };
        Button(string l, function<void()> *F){
            Label = l;
            Function = F;
        };

        void select(){selected= true;}
        void unselect(){selected = false;}
        bool is_selected(){return selected;}
        void doFunction(){(*Function)();}
};

enum ElementType{
    et_Header, et_SKip, et_Button, et_Output
};

class WindowElement{
    public:
        void* Element = nullptr;
        ElementType type = et_SKip;
        WindowElement(ElementType t, void* e = nullptr){
            type = t;
            Element = e;
        }
        WindowElement() = default;
};

class Window{
    ScreenResolution resolution;
    WindowElement* elements = nullptr;
    int elements_cou = 0;

    Button **buttons = nullptr;
    int btns_cou=0;
    int selected = 0;

    void printborders();

public:
        ~Window(){
            delete [] buttons;
            delete [] elements;
        };
        void fullscreen();
        void hideCursor();
        void conmode();
        void setTitle(const wchar_t* myTitle);
        void update();

        void addElement(WindowElement& el);

        void addSkip();
        void addHeader(Header *h);
        void addButton(Button *b);
        void addOutput(Output *o);

        void nextBtn();
        void prevBtn();
        void startup();
        void loop();

        void dialogAttention(string Header, string Content, function<void()> *Function = nullptr);

    void render();

    int dialogChoose(string Header, string* Content, int vcount);

    string dialogInput(string Header, string Subheader, int maxlenght=40);
};

void Window::fullscreen() {
    SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
    resolution = ScreenResolution();
}

void Window::hideCursor() {
    CONSOLE_CURSOR_INFO cinf;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cinf);
    cinf.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cinf);
}

void Window::conmode() {
    stringstream ss;
    ss<<"mode con cols=" << resolution.width << " lines=" << resolution.height;
    system(ss.str().c_str());
}

void Window::printborders() {
    if (resolution.width>10 and resolution.height>10){
        SetCur(0,0);
        cout<<string(resolution.width, '=');
        SetCur(0, resolution.height-1);
        cout<<string(resolution.width, '=');
    }
    SetCur(0,0);
}


void Window::update() {
    if (resolution.width>10 and resolution.height>10){
        for (int i = 1; i<resolution.height-1; i++){
            SetCur(0,i);
            cout<<string(resolution.width, ' ');
        }
    }
    printborders();
    if (btns_cou){
        buttons[selected]->select();
    }
    render();

}

void Window::startup() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    fullscreen();
    hideCursor();
    conmode();
    printborders();
    if (btns_cou){
        buttons[0]->select();
    }
    render();
}

void Window::setTitle(const wchar_t *myTitle) {
    SetConsoleTitleW(myTitle);
}

void Window::addElement(WindowElement &el) {
    auto* temp = new WindowElement[elements_cou+1];
    for(int i=0; i<elements_cou; i++){
        temp[i] = elements[i];
    }
    temp[elements_cou]=el;
    elements_cou++;
    if (elements)
        delete [] elements;
    elements = temp;
}

void Window::dialogAttention(string Header, string Content, function<void()> *Function) {
    auto ul1 = unicode_len(Header);
    auto ul2 = unicode_len(Content);
    auto dialogwidth = max(max(ul2, ul1), 31)+4;
    const int dialogheight = 10;

    int i = 0;
    string goesbrrr;
    for(int i = 0; i<dialogwidth;i++) goesbrrr.append("━");

    for (;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        switch (i) {
            case 0:
                cout<<"┏"<<goesbrrr<<"┓";
                break;
            case (dialogheight-1):
                cout<<"┗"<<goesbrrr<<"┛";
                break;
            default:
                cout<<"┃"<<string(dialogwidth, ' ')<<"┃";
        }

    }
    SetCur((resolution.width-ul1)/2+1,
           (resolution.height-dialogheight)/2+2);
    cout<<Header;
    SetCur((resolution.width-ul2)/2+1,
           (resolution.height-dialogheight)/2+4);
    cout<<Content;

    SetCur((resolution.width-31)/2+1,
           (resolution.height-dialogheight)/2+7);
    cout<<"Для продолжения нажмите [Enter]";

    while (getch()!=KEY_ENTER){}
    if (Function){
        (*Function)();
    }
    i=0;
    for (;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        cout<<" "<<string(dialogwidth, ' ')<<" ";
    }

    render();
}

string Window::dialogInput(string Header, string Subheader, int maxlenght) {
    string output;
    auto ul1 = unicode_len(Header);
    auto ul2 = unicode_len(Subheader);
    auto dialogwidth = max(max(max(ul2, ul1), 31),maxlenght)+4;
    const int dialogheight = 13;

    int i = 0;
    string goesbrrr;
    for(int i = 0; i<dialogwidth;i++) goesbrrr.append("━");

    for (;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        switch (i) {
            case 0:
                cout<<"┏"<<goesbrrr<<"┓";
                break;
            case (dialogheight-1):
                cout<<"┗"<<goesbrrr<<"┛";
                break;
            default:
                cout<<"┃"<<string(dialogwidth, ' ')<<"┃";
        }

    }
    SetCur((resolution.width-ul1)/2+1,
           (resolution.height-dialogheight)/2+2);
    cout<<Header;
    SetCur((resolution.width-ul2)/2+1,
           (resolution.height-dialogheight)/2+4);
    cout<<Subheader;
    SetCur((resolution.width-31)/2+1,
           (resolution.height-dialogheight)/2+10);
    cout<<"  [ESC]                [Enter] ";
    SetCur((resolution.width-31)/2+1,
           (resolution.height-dialogheight)/2+11);
    cout<<"  Назад                 Далее  ";

    unsigned char ch1;
    while (1){
        ConsoleColorWhite();

        SetCur((resolution.width-dialogwidth)/2+1,
               (resolution.height-dialogheight)/2+6);
        cout<<string(dialogwidth, ' ');
        SetCur((resolution.width-unicode_len(output))/2+1,
               (resolution.height-dialogheight)/2+6);
        cout<<output;

        ConsoleColorBlack();
        ch1 = getch();
        if (ch1==208 or ch1==209 or ch1==KEY_BACKSPACE or (ch1>=32 and ch1 <=126)){
            if (ch1==KEY_BACKSPACE){
                if (output.length()!=0)
                    output.pop_back();
                if ((unsigned char)output.back()==208 or (unsigned char)output.back()==209){
                    output.pop_back();
                }
            }
            else if (ch1>=32 and ch1 <=126){
                if (output.length()<maxlenght-1)
                    output+=ch1;
            }
            else if (ch1==208 or ch1==209){
                unsigned char ch2 = getch();
                if (
                        (ch1==208 && ((ch2>=144 && ch2<=191) || ch2==129)||
                         (ch1==209 && ((ch2>=128 && ch2<=143)|| ch2==145)))

                        ){
                    if (output.length()<maxlenght-2){
                        output+=ch1;
                        output+=ch2;
                    }

                }
            }


        }
        else if (ch1==0 or ch1==224){
            getch();
        }
        else if (ch1==KEY_ENTER or ch1==KEY_ESC)
            break;

    }
    i=0;
    for (;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        cout<<" "<<string(dialogwidth, ' ')<<" ";
    }

    render();

    switch(ch1){
        case KEY_ESC:
            return "";
        case KEY_ENTER:
            return output;
        default:
            return "";
    }

}

int Window::dialogChoose(string Header, string* Content, int vcount) {

    //определение длины и ширины рамки
    int dialogwidth = max(unicode_len(Header),28);
    for (int i=0;i<vcount;i++){
        dialogwidth = max(dialogwidth, unicode_len(Content[i]));
    }
    const int dialogheight = 17;



    /////////////// Вывод рамочки

    string goesbrrr;
    for(int i = 0; i<dialogwidth;i++) goesbrrr.append("━");
    for (int i = 0;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        switch (i) {
            case 0:
                cout<<"┏"<<goesbrrr<<"┓";
                break;
            case (dialogheight-1):
                cout<<"┗"<<goesbrrr<<"┛";
                break;
            default:
                cout<<"┃"<<string(dialogwidth, ' ')<<"┃";
        }

    }

    ///////////////




    ////////////// Вывод постоянных элементов рамочки

    SetCur((resolution.width- unicode_len(Header))/2+1,
           (resolution.height-dialogheight)/2+2);
    cout<<Header;

    SetCur((resolution.width-26)/2+1,
           (resolution.height-dialogheight)/2+15);
    cout<<"Для выбора нажмите [Enter]";

    ///////////////

    //печать элементов и перехват

    int startfrom = 0;
    int chosen = 0;
    while (1){

        // Чистка устаревших элементов
        for (int i = 5; i<12; i++){
            SetCur((resolution.width-dialogwidth)/2+1,
                   (resolution.height-dialogheight)/2+i);
            cout<<string(dialogwidth, ' ');
        }



        if (vcount>=6){

            //Вывод показателя что вверху есть элементы;
            SetCur((resolution.width-5)/2+1,
                   (resolution.height-dialogheight)/2+5);
            if (startfrom>0){
                cout<<"[...]";
            }
            else{
                cout<<"     ";
            }
            /////

            for (int i = startfrom; i<startfrom+5;i++){
                if (startfrom<vcount){
                    SetCur((resolution.width-unicode_len(Content[i]))/2+1,
                           (resolution.height-dialogheight)/2+6+i-startfrom);
                    if (i==chosen){
                        ConsoleColorWhite();
                    }
                    cout<< Content[i];
                    ConsoleColorBlack();
                }

            }


            //внизу есть элементы
            SetCur((resolution.width-5)/2+1,
                   (resolution.height-dialogheight)/2+11);
            if (startfrom<vcount-5){
                cout<<"[...]";
            }
            else{
                cout<<"     ";
            }
            /////
        }
        else if (vcount>0){
            for (int i = 0; i<vcount;i++){
                SetCur((resolution.width-unicode_len(Content[i]))/2+1,
                       (resolution.height-dialogheight)/2+6+i);
                if (i==chosen){
                    ConsoleColorWhite();
                }
                cout<< Content[i];
                ConsoleColorBlack();
            }

        }
        else {
            SetCur((resolution.width-9)/2+1,
                   (resolution.height-dialogheight)/2+8);
            cout<<"{ Пусто }";
        }


        int key = getch();
        if (key==KEY_ENTER){
            if (vcount <=0){
                return -1;
            }
            return chosen;
        }
        if (key == KEY_ESC){
            return -1;
        }
        if (key==KEY_CONTROL){
            int key = getch();
            if (key==KEY_CONTROL_UP){
                if (chosen>0){
                    chosen--;
                }


                if (startfrom>0){
                    startfrom--;
                }
            }
            if (key==KEY_CONTROL_DOWN){
                if (chosen<vcount-1){
                    if (chosen>=4){
                        startfrom++;
                    }
                    chosen++;
                }

            }
        }
    }


    for (int i = 0;i<dialogheight;i++){
        SetCur((resolution.width-dialogwidth)/2,
               (resolution.height-dialogheight)/2+i);
        cout<<" "<<string(dialogwidth, ' ')<<" ";
    }

    render();
}


void Window::render() {
    for (int i=0; i<elements_cou; i++){
        if (elements[i].type==et_SKip){}
        else if (elements[i].type==et_Header) {
            auto *h = (Header*)elements[i].Element;
            SetCur((resolution.width- unicode_len(h->Inside))/2,
                   (resolution.height-elements_cou)/2+i);
            cout<<h->Inside;
        }
        else if (elements[i].type==et_Button){
            auto *b = (Button*)elements[i].Element;
            short elw = ((resolution.width - unicode_len(b->Label)-2)/2-3);
            SetCur(elw,
                   (resolution.height-elements_cou)/2+i);
            if (b->is_selected()){
                cout<<">> ["<<b->Label<<"] <<";
            } else{
                cout<<"   ["<<b->Label<<"]   ";
            }

        }
        else if (elements[i].type==et_Output){
            auto *o = (Output*)elements[i].Element;
            if (o->variable!= nullptr){
                switch (o->type) {
                    case ot_Int:
                        SetCur(((short)resolution.width -
                                ((short)(o->before.length()>0)?(unicode_len(o->before)+2):0) -
                                (short)diglength(*(int*)o->variable))/2,
                               (resolution.height-elements_cou)/2+i);

                        if (o->before!=""){
                            cout<<o->before<<": ";
                        }
                        cout<<*(int*)o->variable;
                        break;
                    case ot_String:
                        SetCur((resolution.width -
                                ((o->before.length()>0)?(unicode_len(o->before)+2):0) -
                        unicode_len(string((char*)o->variable)))/2,
                               (resolution.height-elements_cou)/2+i);

                        if (o->before!=""){
                            cout<<o->before<<": ";
                        }
                        cout<<(char*)o->variable;
                        break;
                }
            }

        }
    }
}

void Window::addHeader(Header *hp) {
    auto *we = new WindowElement(et_Header, (void *)hp);
    this->addElement(*we);
}

void Window::addSkip() {
    auto *we = new WindowElement(et_SKip);
    this->addElement(*we);
}

void Window::addButton(Button *b) {
    auto t = new Button*[btns_cou+1];
    for(int i=0; i<btns_cou; i++){
        t[i] = buttons[i];
    }
    t[btns_cou]=b;
    btns_cou++;
    if (buttons)
        delete [] buttons;
    buttons = t;

    auto *we = new WindowElement(et_Button, (void *)b);
    this->addElement(*we);
}

void Window::addOutput(Output *o) {
    auto *we = new WindowElement(et_Output, (void *)o);
    this->addElement(*we);
}

void Window::nextBtn() {
    Button *b = nullptr;
    int i = selected+1;
    for (; i<btns_cou; i++){
        b = buttons[i];
        break;
    }

    if (!b){
        i=0;
        for (; i<elements_cou; i++){
            b=buttons[i];
            break;
        }
    }
    if (b){
        buttons[selected]->unselect();
        b->select();
        selected = i;
    }
}

void Window::prevBtn() {
    Button *b = nullptr;
    int i = selected-1;

    for (; i>=0; i--){
        b = buttons[i];
        break;
    }

    if (!b){
        i=btns_cou-1;
        for (; i>=0; i--){
            b=buttons[i];
            break;
        }
    }
    if (b){
        buttons[selected]->unselect();
        b->select();
        selected = i;
    }
}

void Window::loop() {
    while(1){
        int key = getch();
        if (key==KEY_CONTROL){
            switch (getch()) {
                case KEY_CONTROL_DOWN:
                    nextBtn();
                    render();
                    break;
                case KEY_CONTROL_UP:
                    prevBtn();
                    render();
                default:
                    break;
            }
        }
        else if (key==KEY_ESC){
            return;
        }
        else if (key==KEY_ENTER){
            if (buttons)
            if (buttons[selected]->Function != nullptr)
                buttons[selected]->doFunction();
            update();
        }
    }
}


#endif //CNOVA_DB_SICOGUI_H
