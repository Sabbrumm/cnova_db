//
// Created by Sabbrumm on 27.03.2023.
//
#include <conio.h>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <sstream>
#include <functional>
#include "Sicogui.h"
#include "classes.h"
#include "datafiles.h"
using namespace std;

//Кнопки студента
void ChangeLastName(Student *St, University *Un){
    string last_name = Window().dialogInput("Введите фамилию студента", "Не длиннее 20 символов кириллицей",41);
    if (last_name.empty()){
        return;
    }
    memset(St->last_name, 0 ,41);
    last_name.copy(St->last_name, 41);
    WriteDB(*Un);
}
void ChangeFirstName(Student *St, University *Un){
    string first_name = Window().dialogInput("Введите имя студента", "Не длиннее 20 символов кириллицей",41);
    if (first_name.empty()){
        return;
    }
    memset(St->first_name, 0 ,41);
    first_name.copy(St->first_name, 41);
    WriteDB(*Un);
}
void ChangeMiddleName(Student *St, University *Un){
    string middle_name = Window().dialogInput("Введите отчество студента", "Не длиннее 20 символов кириллицей",41);
    if (middle_name.empty()){
        return;
    }
    memset(St->middle_name, 0 ,41);
    middle_name.copy(St->middle_name, 41);
    WriteDB(*Un);
}
void Transfer(Student *St, University *Un){
    string name = Window().dialogInput("Введите номер группы", "Только 4 цифры",5);
    if (!name.empty() and name.find_first_not_of("0123456789") == string::npos and name.length()==4 and Un->groupGetById(name)!=nullptr){
        Un->groupGetById(St->parent())->removeStudent(St->student_id);
        St->student_id = Un->groupGetById(name)->size()+1;
        Un->groupGetById(name)->addStudent(*St);
        St->stud_id_number_c(); //обновляем номер студенческого;
        WriteDB(*Un);
    }
    else if (Un->groupGetById(name)==nullptr){
        Window().dialogAttention("Ошибка", "Такой группы не существует");
    }
    else if (name.find_first_not_of("0123456789") != string::npos or name.length()!=4) {
        Window().dialogAttention("Ошибка", "Неправильно набран номер");
    }
}
//
void StudentScreen(Student *St, University* Un) {
    Window sscr;
    function<void()> btn_changeln = bind(&ChangeLastName, St, Un);
    function<void()> btn_changefn = bind(&ChangeFirstName, St, Un);
    function<void()> btn_changemn = bind(&ChangeMiddleName, St, Un);
    function<void()> btn_trans = bind(&Transfer, St, Un);


    sscr.addHeader(new Header("--- Меню группы ---"));
    sscr.addSkip();
    sscr.addSkip();
    sscr.addOutput(new Output(ot_String, "Фамилия", St->last_name));
    sscr.addOutput(new Output(ot_String, "Имя", St->first_name));
    sscr.addOutput(new Output(ot_String, "Отчество", St->middle_name));
    sscr.addOutput(new Output(ot_String, "Пол", St->get_gender()));
    sscr.addOutput(new Output(ot_String, "Номер студенческого билета", St->stud_id_number_c()));
    sscr.addSkip();
    sscr.addSkip();
    sscr.addButton(new Button("Изменить фамилию",&btn_changeln));
    sscr.addSkip();
    sscr.addButton(new Button("Изменить имя",&btn_changefn));
    sscr.addSkip();
    sscr.addButton(new Button("Изменить отчество",&btn_changemn));
    sscr.addSkip();
    sscr.addButton(new Button("Перевести в группу...",&btn_trans));

    sscr.update();
    sscr.loop();
}
//



//Кнопки группы
void ChooseAndGoToStudent(Group* Gr, University* Un){
    int choise = Window().dialogChoose("Выберите студента!", Gr->getStudentNames(), Gr->size());
    if (choise!=-1){
        StudentScreen(&Gr->students[choise], Un);
    }
}
void ChangeGroupNumber(Group* Gr, University *Un){
    string name = Window().dialogInput("Введите новый номер группы", "Только 4 цифры",5);
    if (!name.empty() and name.find_first_not_of("0123456789") == string::npos and name.length()==4 and Un->groupGetById(name)==nullptr){
        memset(Gr->group_id, 0 ,5);
        memset(Gr->screen_name, 0 ,5);

        name.copy(Gr->group_id, 5);
        name.copy(Gr->screen_name, 5);
        for (int i = 0; i<Gr->size(); i++){
            Gr->students[i]._set_gid(name);
        }
        WriteDB(*Un);
    }
    else if (Un->groupGetById(name)!=nullptr){
        Window().dialogAttention("Ошибка", "Такая группа уже существует");
    }
    else if (name.find_first_not_of("0123456789") != string::npos or name.length()!=4) {
        Window().dialogAttention("Ошибка", "Неправильно набран номер");
    }
}
void StudentAdd(Group* Gr, University *Un){
    string last_name = Window().dialogInput("Введите фамилию студента", "Не длиннее 20 символов кириллицей",41);
    if (last_name.empty()){
        return;
    }
    string first_name = Window().dialogInput("Введите имя студента", "Не длиннее 20 символов кириллицей",41);
    if (first_name.empty()){
        return;
    }
    string middle_name = Window().dialogInput("Введите отчество студента", "Не длиннее 20 символов кириллицей",41);
    if (middle_name.empty()){
        return;
    }

    string* t = new string[2];
    t[0] = "Женский";
    t[1] = "Мужской";
    int gender = Window().dialogChoose("Выберите пол студента", t, 2);
    if (gender!=-1){
        Student* s = new Student(last_name, first_name, middle_name, Gr->size() + 1, gender);
        Gr->addStudent(*s);
        WriteDB(*Un);
    }




}
void RemoveStudent(Group* Gr, University *Un){
    int choise = Window().dialogChoose("Выберите студента", Gr->getStudentNames(), Gr->size());
    if (choise == -1)
        return; //Не выбрал - вылет

    Gr->removeStudent(Gr->students[choise].student_id);
    WriteDB(*Un);
}
//
void GroupScreen(Group *Gr, University* Un){
    Window gscr;
    function<void()> btn_choosestudent = bind(&ChooseAndGoToStudent, Gr, Un);
    function<void()> btn_changenumber = bind(&ChangeGroupNumber, Gr, Un);
    function<void()> btn_newstudent = bind(&StudentAdd, Gr, Un);
    function<void()> btn_remstudent = bind(&RemoveStudent, Gr, Un);

    gscr.addHeader(new Header("--- Меню группы ---"));
    gscr.addSkip();
    gscr.addSkip();
    gscr.addOutput(new Output(ot_String, "Номер", Gr->screen_name));
    gscr.addOutput(new Output(ot_Int, "Количество студентов", Gr->getScountPointer()));
    gscr.addSkip();
    gscr.addSkip();
    gscr.addButton(new Button("Изменить номер", &btn_changenumber));
    gscr.addButton(new Button("Зачислить нового студента",&btn_newstudent));
    gscr.addButton(new Button("Отчислить студента",&btn_remstudent));
    gscr.addButton(new Button("Перейти к студенту...", &btn_choosestudent));

    gscr.update();
    gscr.loop();
}
//



//Кнопки факультета
void ChooseAndGoToGroup(Faculty* Fa, University* Un){
    int choise = Window().dialogChoose("Выберите группу!", Fa->getGroupNames(), Fa->size());
    if (choise!=-1){
        GroupScreen(&Fa->groups[choise], Un);
    }
}
void ChangeFacultyName(Faculty *Fa, University *Un){
    string name = Window().dialogInput("Введите новое название факультета", "Не длиннее 10 символов кириллицей",21);
    if (!name.empty()){
        memset(Fa->faculty_name, 0 ,21);
        name.copy(Fa->faculty_name, 21);
        WriteDB(*Un);
    }
}
void GroupAdd(Faculty *Fa, University *Un){
    string name = Window().dialogInput("Введите новый номер группы", "Только 4 цифры",5);
    if (!name.empty() and name.find_first_not_of("0123456789") == string::npos and name.length()==4 and Un->groupGetById(name)==nullptr){
        auto *Gr = new Group("");
        memset(Gr->group_id, 0 ,5);
        memset(Gr->screen_name, 0 ,5);

        name.copy(Gr->group_id, 5);
        name.copy(Gr->screen_name, 5);

        Fa->addGroup(*Gr);
        WriteDB(*Un);
    }
    else if (Un->groupGetById(name)!=nullptr){
        Window().dialogAttention("Ошибка", "Такая группа уже существует");
    }
    else if (name.find_first_not_of("0123456789") != string::npos or name.length()!=4) {
        Window().dialogAttention("Ошибка", "Неправильно набран номер");
    }
}
void GroupRemove(Faculty *Fa, University *Un) {
    int choise = Window().dialogChoose("Выберите Группу", Fa->getGroupNames(), Fa->size());
    if (choise == -1)
        return; //Не выбрал - вылет

    if (Fa->groups[choise].size() == 0) {
        Fa->removeGroup(Fa->groups[choise].id());
        WriteDB(*Un);
        return; //Пустая - удаляем
    } else {
        if (Fa->size() == 1) {
            Fa->removeGroup(Fa->groups[choise].id());
            WriteDB(*Un);
            return;
        }
        string *t = new string[Fa->size()]; //иначе просим перевода
        int j = 0;
        int i = 0;
        for (; i < Fa->size(); i++) {
            if (i != choise) {
                t[j] = Fa->getGroupNames()[i];
                j++;
            }
        }
        int choise2 = Window().dialogChoose("Выберите группу для перевода всех студентов", t, Fa->size() - 1);
        if (choise2 == -1) {
            delete[] t;
            return; // в случае отмены ничего не делаем;
        }

        else {
            if (choise2 >= choise) {
                choise2++;
            }
            Fa->removeGroup(Fa->groups[choise].id(), Fa->groups[choise2].id());
            WriteDB(*Un);
        }
        delete[] t;
    }
}
//
void FacultyScreen(Faculty *Fa, University* Un){
    Window fscr;
    function<void()> btn_choosegroup = bind(&ChooseAndGoToGroup, Fa, Un);
    function<void()> btn_changename = bind(&ChangeFacultyName, Fa, Un);
    function<void()> btn_addgroup = bind(&GroupAdd, Fa, Un);
    function<void()> btn_remgroup = bind(&GroupRemove, Fa, Un);


    fscr.addHeader(new Header("--- Меню факультета ---"));
    fscr.addSkip();
    fscr.addSkip();
    fscr.addOutput(new Output(ot_String, "Название", Fa->faculty_name));
    fscr.addOutput(new Output(ot_Int, "Идентификатор", Fa->getidPointer()));
    fscr.addOutput(new Output(ot_Int, "Количество групп", Fa->getGcountPointer()));
    fscr.addSkip();
    fscr.addSkip();
    fscr.addButton(new Button("Изменить название", &btn_changename));
    fscr.addButton(new Button("Создать новую группу", &btn_addgroup));
    fscr.addButton(new Button("Расформировать группу", &btn_remgroup));
    fscr.addButton(new Button("Перейти к группе...", &btn_choosegroup));


    fscr.update();
    fscr.loop();
}
//



//Кнопки универа
void ChooseAndGoToFaculty(University* Un){
    int choise = Window().dialogChoose("Выберите факультет!", Un->getFacultiesNames(), Un->size());
    if (choise!=-1){
        FacultyScreen(&(Un->faculties[choise]), Un);
    }
}
void ChangeUniversityName(University *Un){
    string name = Window().dialogInput("Введите новое название университета", "Не длиннее 15 символов кириллицей",31);
    if (!name.empty()){
        memset(Un->univ_name, 0 ,31);
        name.copy(Un->univ_name, 31);
        WriteDB(*Un);
    }
}
void FacultyAdd(University *Un){
    string name = Window().dialogInput("Введите новое название факультета", "Не длиннее 10 символов кириллицей",21);
    if (!name.empty()){
        auto *Fa = new Faculty(Un->size()+1, "");
        memset(Fa->faculty_name, 0 ,31);
        name.copy(Fa->faculty_name, 31);
        Un->addFaculty(*Fa);
        stringstream ss;
        ss << "Вы основали " << name <<"! Удачи в научных исследованиях!";
        WriteDB(*Un);
        Window().dialogAttention("Так держать!", ss.str());
        ss.str(string());
        ss.clear();
    }
}
void FacultyRemove(University *Un){
    int choise = Window().dialogChoose("Выберите факультет", Un->getFacultiesNames(), Un->size());
    if (choise==-1)
        return; //Не выбрал - вылет

    if(Un->faculties[choise].size()==0){
        Un->removeFaculty(Un->faculties[choise].id());
        WriteDB(*Un);
        return; //Пустой - удаляем
    }
    else{
        if (Un->size()==1){
            Un->removeFaculty(Un->faculties[choise].id());
            WriteDB(*Un);
            return;
        }
        string* t = new string[Un->size()]; //иначе просим перевода
        int j = 0;
        int i = 0;
        for (; i<Un->size(); i++){
            if (i!=choise){
                t[j] = Un->getFacultiesNames()[i];
                j++;
            }
        }
        int choise2 = Window().dialogChoose("Выберите факультет для перевода всех групп", t, Un->size()-1);
        if (choise2==-1) return; // в случае отмены ничего не делаем;
        else{
            if (choise2>=choise){
                choise2++;
            }
            stringstream ss, ss2;
            ss << Un->faculties[choise].faculty_name << " расформирован!";
            ss2 << "По всем вопросам обращайтесь в деканат "<<Un->faculties[choise2].faculty_name;
            Un->removeFaculty(Un->faculties[choise].id(), Un->faculties[choise2].id());
            Window().dialogAttention(ss.str(), ss2.str());
            ss.str(string());
            ss.clear();
            ss2.str(string());
            ss2.clear();
            WriteDB(*Un);
        }
        delete [] t;
    }






}
//
void UniversityScreen(University *Un){
    Window uscr;

    function<void()> btn_choosefaculty = bind(&ChooseAndGoToFaculty, Un);
    function<void()> btn_changename = bind(&ChangeUniversityName, Un);
    function<void()> btn_addfac = bind(&FacultyAdd, Un);
    function<void()> btn_remfac = bind(&FacultyRemove, Un);


    uscr.addHeader(new Header("--- Меню университета ---"));
    uscr.addSkip();
    uscr.addSkip();
    uscr.addOutput(new Output(ot_String, "Название", Un->univ_name));
    uscr.addOutput(new Output(ot_Int, "Количество факультетов", Un->getFcountPointer()));
    uscr.addSkip();
    uscr.addSkip();
    uscr.addButton(new Button("Изменить название", &btn_changename));
    uscr.addSkip();
    uscr.addButton(new Button("Основать новый факультет", &btn_addfac));
    uscr.addSkip();
    uscr.addButton(new Button("Расформировать факультет", &btn_remfac));
    uscr.addSkip();
    uscr.addButton(new Button(" Перейти к факультету...", &btn_choosefaculty));

    uscr.update();
    uscr.loop();
}
//
void SearchStudent(University *Un){
   string lname = Window().dialogInput("Введите фамилию студента", "Не более 20 символов кириллицей");
   if (lname.empty()){
       return;
   }
   int matches = 0;
   Student** smas = Un->searchStudentByLastName(lname, matches);
   string* bb = new string[matches];

   stringstream ss;
   for (int i=0; i<matches; i++){
       ss<< smas[i]->last_name << " " << smas[i]->first_name << " ("+smas[i]->stud_id_number()<<")";
       bb[i] = ss.str();
       ss.str(string());
       ss.clear();
   }
   int choise = Window().dialogChoose("Выберите студента", bb, matches);
   if (choise==-1){
       return;
   }
   else{
       StudentScreen(smas[choise],Un);
   }
}
void SearchGroup(University *Un){
    string name = Window().dialogInput("Введите номер группы", "Только 4 цифры",5);
    if (!name.empty() and name.find_first_not_of("0123456789") == string::npos and name.length()==4 and Un->groupGetById(name)!=nullptr){
        GroupScreen(Un->groupGetById(name), Un);
    }
    else if (Un->groupGetById(name)==nullptr){
        Window().dialogAttention("Ошибка", "Такая группа не существует");
    }
    else if (name.find_first_not_of("0123456789") != string::npos or name.length()!=4) {
        Window().dialogAttention("Ошибка", "Неправильно набран номер");
    }
}

void SearchScreen(University *Un){
    Window sescr;

    function<void()> btn_searchstudent = bind(&SearchStudent, Un);
    function<void()> btn_searchgroup = bind(&SearchGroup, Un);


    sescr.addHeader(new Header("--- Поиск по базе ---"));
    sescr.addSkip();
    sescr.addSkip();
    sescr.addButton(new Button("Искать студента по фамилии", &btn_searchstudent));
    sescr.addSkip();
    sescr.addButton(new Button("Искать группу по номеру", &btn_searchgroup));

    sescr.update();
    sescr.loop();

}

void MainScreen(University *Un) {
    Window mscr;
    function<void()> btn_exit = bind(&exit, 0);
    function<void()> btn_gotoun = bind(&UniversityScreen, Un);
    function<void()> btn_search = bind(&SearchScreen, Un);

    mscr.addHeader(new Header("--- Главное Меню ---"));
    mscr.addSkip();
    //mscr.addButton(new Button("Поиск по базе"));
    mscr.addSkip();
    mscr.addButton(new Button("Поиск по базе", &btn_search));
    mscr.addSkip();
    mscr.addButton(new Button("Перейти к университету", &btn_gotoun));
    mscr.addSkip();
    mscr.addButton(new Button("Выход из программы", &btn_exit));

    mscr.dialogAttention("Добро пожаловать в CNOVA DB!", "База данных прочитана успешно.");

    mscr.update();
    mscr.loop();
}

University* startup(){
    University* pUNIK = ReadDB();
    Window HelloScreen;
    HelloScreen.update();
    if (pUNIK){
        University UNIK = *pUNIK;
    }
    else{
        function<void()> corrupt_result = bind(&exit, 3);
        HelloScreen.dialogAttention("Ошибка чтения файла базы!",
                                    "Сейчас программа будет завершена. "
                                    "Попробуйте восстановить файл программой repair.exe",
                                    &corrupt_result);
    }
    return pUNIK;
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    Window().setTitle(L"CNOVA DataBase");
    Window().startup();
    University* un = startup();
    MainScreen(un);
    return 0;
}