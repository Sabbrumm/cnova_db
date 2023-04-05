//
// Created by Sabbrumm on 23.03.2023.
//

#ifndef CNOVA_DB_CLASSES_H
#define CNOVA_DB_CLASSES_H

#include <string>
#include <sstream>

using namespace std;





char Male[] = "Мужской";
char Female[] = "Женский";






class Student{
    private:
        char parent_group_id[5] = {0}; //PARENT ID
        char student_id_number[7]={0};
        //SELF ID
    public:
        char first_name[41] = {0};
        char middle_name[41] = {0};
        char last_name[41] = {0};
        bool is_male = false;
        int student_id=0;

        Student(const string& _last_name,
                const string& _first_name,
                const string& _middle_name,
                int _student_id, bool _is_male){
            strcpy(last_name, _last_name.c_str());
            strcpy(first_name, _first_name.c_str());
            strcpy(middle_name, _middle_name.c_str());
            student_id = _student_id;
            is_male = _is_male;
        }
        Student() = default;

        void _set_gid(string p_g_id);
        string stud_id_number(){
            stringstream t;
            char buf[3];
            snprintf(buf, sizeof(buf), "%02d", student_id);
            t<<parent_group_id<<buf;
            return t.str();
        }
        char* stud_id_number_c(){
            stud_id_number().copy(student_id_number, 7);
            return student_id_number;
        }
        char* get_gender(){
            if (is_male){
                return Male;
            }
            else{
                return Female;
            }
        }
        string parent(){
            return string(parent_group_id);
        }
};

void Student::_set_gid(string p_g_id) {
    strcpy(parent_group_id, p_g_id.c_str());
}



//////////////////////////
class Group{
    private:
        int parent_faculty_id = 0;
    int students_count = 0;
    public:
        Student* students = nullptr;
        char screen_name[5] = {0};
        Group(string _group_id){
            strcpy(group_id, _group_id.c_str());
            strcpy(screen_name, _group_id.c_str());
        }
        Group()=default;
        ~Group(){
        }

        void addStudent(Student& new_student);
        void _set_fid(int f_id);
        int size(){
            return students_count;
        }
        int* getScountPointer(){return &students_count;}
        void restore();
        string id(){
            return string(group_id);
        }
        int parent(){
            return parent_faculty_id;
        }
        string* getStudentNames(){
            string* t = new string[students_count];
            stringstream ss;
            for (int i=0;i<students_count;i++){
                ss<<students[i].last_name<<" "<<students[i].first_name<<" (№"<<students[i].stud_id_number()<<")";
                t[i]=ss.str();
                ss.str(string());
                ss.clear();
            }
            return t;
        }

    char group_id[5] = {0};

    void removeStudent(int st_id);
};

void Group::addStudent(Student& new_student) {
    new_student._set_gid(group_id);
    auto* temp = new Student[students_count+1];
    for(int i=0; i<students_count; i++){
        temp[i] = students[i];
    }
    temp[students_count]=new_student;
    students_count++;
    if (students)
        delete [] students;
    students = temp;

}

void Group::_set_fid(int f_id) {
    parent_faculty_id = f_id;
}

void Group::restore() {
    students_count=0;
    students = nullptr;
}
void Group::removeStudent(int st_id) {
    auto* temp = new Student[students_count-1];
    int j=0;
    for(int i=0; i<students_count; i++){
        if (students[i].student_id!=st_id){
            temp[j] = students[i];
            temp[j].student_id=j+1;
            j++;
        }
        else{
            //TODO Проверь, возможно глючит
            delete &students[i];
        }
    }
    students_count--;
    students = temp;
}
//////////////////////////





class Faculty{
    private:
        int faculty_id;
        int groups_count = 0;
    public:
        char faculty_name[21] = {0};
        Group* groups = nullptr;

        Faculty(int id, string name){
            faculty_id = id;
            strcpy(faculty_name, name.c_str());
        }
        ~Faculty(){
        }
        Faculty() = default;
        int* getGcountPointer(){return &groups_count;}
        int* getidPointer(){return &faculty_id;}
        void addGroup(Group& new_group);
        void removeGroup(string group_id, string target);
        int size(){
            return groups_count;
        }

        void restore();
        int id(){
            return faculty_id;
        }
        Group* groupGetById(string g_id){
            for (int j=0; j<size();j++){
                if (string(groups[j].id())==g_id){
                    return &groups[j];
                }
            }
            return nullptr;
        };

        string* getGroupNames(){
            string* t = new string[groups_count];
            stringstream ss;
            for (int i=0;i<groups_count;i++){
                ss<<"Группа "<<groups[i].screen_name;
                t[i]=ss.str();
                ss.str(string());
                ss.clear();
            }
            return t;
        }
};

void Faculty::addGroup(Group& new_group) {
    new_group._set_fid(faculty_id);
    auto* temp = new Group[groups_count+1];
    for(int i=0; i<groups_count; i++){
        temp[i] = groups[i];
    }
    temp[groups_count]=new_group;
    groups_count++;
    if (groups)
        delete [] groups;
    groups = temp;
}

void Faculty::removeGroup(string group_id, string target="") {
    if (!target.empty()){
        Group* Gr = groupGetById(group_id);
        Group* GrT = groupGetById(target);
        for (int i = 0; i<Gr->size(); i++){
            auto* st = &Gr->students[i];
            st->student_id = GrT->size()+1;
            GrT->addStudent(*st);
        }

    }
    if (target.empty()){
        delete groupGetById(group_id);
    }

    auto* temp = new Group[groups_count-1];
    int j=0;
    for(int i=0; i<groups_count; i++){
        if (groups[i].id()!=group_id){
            temp[j] = groups[i];
            j++;
        }
    }
    groups_count--;
    if (target.empty())
        delete [] groups;
    groups = temp;
}

void Faculty::restore() {
    groups_count=0;
    groups = nullptr;
}




//////////////////////////
class University{
    private:
        int faculties_count=0;
    public:
        char univ_name[31]={0};
        Faculty* faculties = nullptr;

        University(string name){
            strcpy(univ_name, name.c_str());
        }
        University()=default;
        ~University(){
        }

        int size(){
            return faculties_count;
        }

        string* getFacultiesNames(){
            string* t = new string[faculties_count];
            stringstream ss;
            for (int i=0;i<faculties_count;i++){
                ss<<faculties[i].faculty_name<<" (№"<<faculties[i].id()<<")";
                t[i]=ss.str();
                ss.str(string());
                ss.clear();
            }
            return t;
        }

        void addFaculty(Faculty& new_faculty);
        void restore();
        void removeFaculty(int faculty_id, int target_id);
        int* getFcountPointer(){return &faculties_count;}
        Group* groupGetById(string g_id){

            for (int i = 0; i<faculties_count; i++){
                for (int j=0; j<faculties[i].size();j++){
                    if (string(faculties[i].groups[j].id())==g_id){
                        return &faculties[i].groups[j];
                    }
                }
            }


            return nullptr;
        };
        Faculty* facultyGetById(int f_id){
            for (int i = 0; i<faculties_count; i++){
                if (faculties[i].id()==f_id){
                    return &faculties[i];
                }
            }
            return nullptr;
        };

    Student** searchStudentByLastName(string lname, int &count);
};


void University::removeFaculty(int faculty_id, int target=-1) {
    if (target!=-1){
        Faculty* Fa = facultyGetById(faculty_id);
        Faculty* Ta = facultyGetById(target);
        for (int i = 0; i<Fa->size(); i++){
            Ta->addGroup(Fa->groups[i]);
        }
    }
    if (target==-1){
        delete facultyGetById(faculty_id);
    }

    auto* temp = new Faculty[faculties_count-1];
    int j=0;
    for(int i=0; i<faculties_count; i++){
        if (faculties[i].id()!=faculty_id){
            temp[j] = faculties[i];
            j++;
        }
    }
    faculties_count--;
    if (target==-1)
        delete [] faculties;
    faculties = temp;
}

void University::addFaculty(Faculty& new_faculty) {
    auto* temp = new Faculty[faculties_count+1];
    for(int i=0; i<faculties_count; i++){
        temp[i] = faculties[i];
    }
    temp[faculties_count]=new_faculty;
    faculties_count++;
    if (faculties)
        delete [] faculties;
    faculties = temp;
}

void University::restore() {
    faculties_count=0;
    faculties = nullptr;
}

Student** University::searchStudentByLastName(string lname, int &count){
    Student** t = nullptr;
    for (int i = 0; i<faculties_count; i++){
        for (int j=0; j<faculties[i].size();j++){

            for (int l=0; l<faculties[i].groups[j].size(); l++){
                if (string(faculties[i].groups[j].students[l].last_name)==lname){
                    Student** t2 = new Student*[count+1];
                    if (t){
                        for (int k=0; k<count; k++){
                            t2[k]=t[k];
                        }
                    }
                    t2[count] = &faculties[i].groups[j].students[l];
                    count++;
                    delete t;
                    t=t2;
                }

            }

        }
    }
    return t;

}







//
//Student* studentSearchByName(const string& _last_name,
//                             const string& _first_name,
//                             const string& _middle_name);
//Student* studentSearchByStudentID(int StudentID); //состоит из номера группы и ID студента записанных подряд



enum DataTypes{
    dt_Student,
    dt_Group,
    dt_Faculty,
    dt_University,
};






#endif //CNOVA_DB_CLASSES_H
