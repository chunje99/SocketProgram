#include "Person.h"
#include "string.h"

Person::Person()
    : m_name(new char[128])
{
    std::cout << "Person()" << std::endl;
}

Person::Person(int sex, int age, const char* name){
    std::cout << "Person(int,int,const char*)" << std::endl;
    m_sex = sex;
    m_age = age;
    m_name = new char[strlen(name)+1];
    strcpy(m_name, name);
}

Person::Person(const Person& person){
    std::cout << "Person(const Person& person)" << std::endl;
    m_sex = person.m_sex;
    m_age = person.m_age;
    m_name = new char[strlen(person.m_name)+1];
    strcpy(m_name, person.m_name);
}

Person::Person(Person&& person){
    std::cout << "Person(Person&& person)" << std::endl;
    m_sex = std::move(person.m_sex);
    m_age = std::move(person.m_age);
    m_name = std::move(person.m_name);
    person.m_name = nullptr;
}

Person::~Person(){
    std::cout << "~Person()" << std::endl;
    if(m_name){
        std::cout << "~Person() delete" << std::endl;
        delete m_name;
        m_name = nullptr;
    }
}

Person& Person::operator=(const Person& person){
    std::cout << "operator= &" << std::endl;
    return *this;
}

Person& Person::operator=(Person&& person){
    std::cout << "operator= &&" << std::endl;
    return *this;
}

Person& Person::operator+(const Person& person){
//   std::cout << "operator+&" << std::endl;
    return *this;
}

Person& Person::operator+(Person&& person){
//    std::cout << "operator+&&" << std::endl;
    return *this;
}

void Person::Print(){
    std::cout << "sex:" << m_sex << " age:" << m_age << " name: "
        << m_name << std::endl;
}

void Person::setAge(int age){
    m_age = age;
}

void Person::setSex(int sex){
    m_sex = sex;
}

void Person::setName(const char* name){
    if(m_name)
        delete m_name;
    m_name = new char[strlen(name)+1];
    strcpy(m_name, name);

}
