#include <iostream>
#include <windows.h>
#include <conio.h>
#include "classes.h"
#include "datafiles.h"
#include "Sicogui.h"
using namespace std;


void testWriteUniversity(){
    University LETI("ЛЭТИ");

    Faculty FEL(1, "ФЭЛ");
    Faculty FKTI(2, "ФКТИ");

    Group g2375("2375");
    Group g2395("2395");
    Group g2394("2394");

    Group g2282("2282");
    Group g2283("2283");
    Group g2284("2284");

    Student st1("Иванченко", "Архип", "Павлович", 1, true);
    Student st2("Шичков", "Георгий", "Витальевич", 2, true);
    Student st3("Шитов", "Богдан", "Антонович", 3, true);
    Student st4("Шичков", "Александр", "Витальевич", 4, true);

    Student st5("Гудинина", "Анна", "Альбертовна",1, false);
    Student st6("Расторженова", "Екатерина", "Ньютоновна",2, false);
    Student st7("Недолгий", "Иван", "Денисович",3, true);
    Student st8("Приятный", "Алексей", "Павлович",4, true);

    Student st9("Ильина", "Елена", "Владимировна",1, false);
    Student st10("Ленин", "Илья", "Макарович",2, true);
    Student st11("Троцкий", "Алексей", "Андреевич",3, true);
    Student st12("Обухова", "Ева", "Алексеевна",4, false);

    Student st13("Чернышев", "Иван", "Владимирович",1, true);
    Student st14("Самсонов", "Олег", "Даниилович",2, true);
    Student st15("Минин", "Георгий", "Константинович",3, true);
    Student st16("Тихомиров", "Даниил", "Тимурович",4, true);

    Student st17("Хомякова", "Кира", "Дмитриевна",1, false);
    Student st18("Жуков", "Артём", "Фёдорович",2, true);
    Student st19("Ларионов", "Никита", "Антонович",3, true);
    Student st20("Голубева", "Александра", "Тимофеевна",4, false);

    Student st21("Чернов", "Максим", "Николаевич",1, true);
    Student st22("Бобров", "Кирилл", "Дмитриевич",2, true);
    Student st23("Петров", "Владислав", "Михайлович",3, true);
    Student st24("Колпаков", "Андрей", "Завалитович",4, true);



    /////////////

    g2375.addStudent(st1);
    g2375.addStudent(st2);
    g2375.addStudent(st3);
    g2375.addStudent(st4);

    g2395.addStudent(st5);
    g2395.addStudent(st6);
    g2395.addStudent(st7);
    g2395.addStudent(st8);

    g2394.addStudent(st9);
    g2394.addStudent(st10);
    g2394.addStudent(st11);
    g2394.addStudent(st12);

    g2282.addStudent(st13);
    g2282.addStudent(st14);
    g2282.addStudent(st15);
    g2282.addStudent(st16);

    g2283.addStudent(st17);
    g2283.addStudent(st18);
    g2283.addStudent(st19);
    g2283.addStudent(st20);

    g2284.addStudent(st21);
    g2284.addStudent(st22);
    g2284.addStudent(st23);
    g2284.addStudent(st24);

    FKTI.addGroup(g2375);
    FKTI.addGroup(g2395);
    FKTI.addGroup(g2394);

    FEL.addGroup(g2282);
    FEL.addGroup(g2283);
    FEL.addGroup(g2284);



    LETI.addFaculty(FKTI);
    LETI.addFaculty(FEL);


    WriteDB(LETI);


}


int main(){
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    testWriteUniversity();
    cout<<"SUCCESSFULLY REPAIRED";
    getch();
    return 0;
}
