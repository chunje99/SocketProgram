#ifndef PERSON_H
#define PERSON_H
#include <iostream>
#include <memory>

class Person{
    public:
        Person();
        Person(int sex, int age, const char* name);
        ~Person();
        Person(const Person& person);
        Person(Person&& person);
        Person& operator=(const Person& person);
        Person& operator=(Person&& person);
        Person& operator+(const Person& person);
        Person& operator+(Person&& person);
        void Print();
        void setSex(int sex);
        void setAge(int age);
        void setName(const char* name);
    private:
        int m_sex;
        int m_age;
        char* m_name;
};
#endif
