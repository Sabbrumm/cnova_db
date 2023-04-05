//
// Created by Sabbrumm on 23.03.2023.
//

#ifndef CNOVA_DB_DATAFILES_H
#define CNOVA_DB_DATAFILES_H
#include <fstream>
#include "classes.h"
#include <exception>
#define filename "base.bin"
#define byte char

struct Serialisation{
    char dataType{};
    unsigned short size{};
    char* bytes{};
    Serialisation(char _dataType, short _size, char* _bytes){
        dataType = _dataType;
        size = _size;
        bytes = _bytes;
    }
    Serialisation() = default;
};


Student* CastToStudent(Serialisation *s){
    if (s->dataType == dt_Student){
        return (Student*)(s->bytes);
    }
    else
        return nullptr;
}
Group* CastToGroup(Serialisation *s){
    if (s->dataType == dt_Group){
        auto * temp = (Group*)(s->bytes);
        temp->restore();
        return temp;
    }
    else
        return nullptr;
}
Faculty* CastToFaculty(Serialisation *s){
    if (s->dataType == dt_Faculty){
        auto * temp = (Faculty*)(s->bytes);
        temp->restore();
        return temp;
    }
    else
        return nullptr;
}
University* CastToUniversity(Serialisation *s){
    if (s->dataType == dt_University){
        auto * temp = (University*)(s->bytes);
        temp->restore();
        return temp;
    }
    else
        return nullptr;
}

// [ ser1 ser2 ser3   ]
struct Database{
    Serialisation* sermass = nullptr;
    int obj_cou = 0;
    void AddObj(Serialisation* ser){
        auto *temp = new Serialisation[obj_cou+1];
        for(int i=0; i<obj_cou; i++){
            temp[i] = sermass[i];
        }
        temp[obj_cou]=*ser;
        obj_cou++;
        if (sermass)
            delete [] sermass;
        sermass=temp;
    };
    void clear(){
        delete [] sermass;

    }

    void _sm_pop(int x){
        if (obj_cou==1){
            sermass= nullptr;
            obj_cou--;
            return;
        }
        auto temp = new Serialisation[obj_cou-1];
        int j;

        for (int i = 0; i < obj_cou; i++){
            if (i == x)
                continue;
            temp[j]=sermass[i];
            j++;
        }
        delete [] sermass;
        sermass=temp;
        obj_cou--;
    }

    int ReadObjects(){
        ifstream file(filename, ios::binary | ios::in);
        if (file.bad() or !file.is_open())
            return 0;
        file.seekg (0, ios::beg);
        char b;
        char t_dataType = 0;
        unsigned short t_size=0;
        unsigned char* buf;

        while (file.get(b)){
            //был получен первый байт
            t_dataType = b;
            for (int i=0; i<2;i++){
                file.get(b);
                unsigned char d = b;
                t_size = t_size | (unsigned short)d<<8*i;
            }
            if (t_dataType<0 or t_dataType>3){
                clear();
                return 0;
            }
            buf = new unsigned char[t_size];
            *buf = {0};
            for (int i=0; i<t_size; i++){
                file.get(b);
                if (file.eof()){
                    clear();
                    return 0;
                }
                buf[i] = b;
            }
            auto *t = new Serialisation(t_dataType, t_size, (char*)buf);
            t_size = 0;
            t_dataType=0;
            AddObj(t);
        }

        file.close();
        return 1;
    }
    University* GetUniversity(){
        University* temp = nullptr;

            for (int i = obj_cou-1; i>=0; i--){
                if (sermass[i].dataType==dt_University){
                    if (!temp){
                        temp = CastToUniversity(&sermass[i]);
                    } else{
                        return nullptr;
                    }
                    //если уник не задан, но считан, ставим его.
                    //если уник идет по второму кругу - ошибка чтения



                } else if (sermass[i].dataType==dt_Faculty){
                    if (!temp){
                        return nullptr;
                    } //если не считали уник - ошибка чтения
                    temp->addFaculty(*CastToFaculty(&sermass[i]));


                } else if (sermass[i].dataType==dt_Group){
                    if (!temp){
                        return nullptr;
                    } //если не считали уник - ошибка чтения

                    auto* gr = CastToGroup(&sermass[i]);
                    Faculty* fct = temp->facultyGetById(gr->parent());
                    if (fct){ //Убеждаемся что нашли факультет;
                        fct->addGroup(*gr);
                    }
                    else{
                        return nullptr;
                    }

                } else if (sermass[i].dataType==dt_Student){
                    if (!temp){
                        return nullptr;
                    } //если не считали уник - ошибка чтения

                    auto* st = CastToStudent(&sermass[i]);
                    Group *tg = temp->groupGetById(st->parent());
                    if (tg){ //убеждаемся что точно нашли группу
                        tg->addStudent(*st);
                    }
                    else{
                        return nullptr;
                    }
                }
            }
        return temp;
    };



    void SetUniversity(University& univ){
        for (int u = 0; u<univ.size(); u++){

            Faculty &fac = univ.faculties[u];
            for (int f = 0; f<fac.size(); f++){

                Group &grp = fac.groups[f];
                for (int g=0; g<grp.size();g++){
                    Student &stu = grp.students[g];
                    Serialisation st_ser(dt_Student, sizeof(*&stu), (byte*)&stu);
                    AddObj(&st_ser);
                }

                Serialisation grp_ser(dt_Group, sizeof(*&grp), (byte*)&grp);
                AddObj(&grp_ser);



            }
            Serialisation fac_ser(dt_Faculty, sizeof(*&fac), (byte*)&fac);
            AddObj(&fac_ser);

        }
        Serialisation uni_ser(dt_University, sizeof(*&univ), (byte*)&univ);
        AddObj(&uni_ser);
    }
    void WriteData(){
        ofstream file(filename, ios::binary | ios::out);
        if (!file.is_open()){
            return;
        }
        file.clear();
        for (int i = 0; i < obj_cou; i++) {
            Serialisation ser = sermass[i];


            file.write((const char*)&(ser.dataType), sizeof ser.dataType);
            file.write((const char*)&(ser.size), sizeof ser.size);
            file.write(ser.bytes, ser.size);

        }
        file.close();
    }
    ~Database(){
        clear();
    }
};

void WriteDB(University& un){
    Database DB;
    DB.clear();
    DB.SetUniversity(un);
    DB.WriteData();
}

University* ReadDB(){
    Database DB;
    if (!DB.ReadObjects()){
        return nullptr;
    }
    return DB.GetUniversity();
}



#endif //CNOVA_DB_DATAFILES_H
