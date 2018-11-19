#include "iostream"
#include "Elevator.h"
#include "Person.h"
#include <vector>
#include <chrono>

using namespace std;

using microfortnights = std::chrono::duration<float, std::ratio<14*24*60*60, 1000000>>;
int main(){
    cout << "Hello" << endl;
    cout << "=============" << endl;
    Person(1,2,"a");
    cout << "=============" << endl;
    Person a = Person(1,2,"a");
    a.Print();
    cout << "=============" << endl;
    Person b(std::move(a));
    b.Print();
    a.setName("b");
    a.Print();
    b.Print();
    cout << "=============" << endl;
    //for( int i = 0 ; i < 100000 ; i++)
    vector<Person> vecA;
    //for(int i = 0 ; i < 10000000 ; i++)
    for(int i = 0 ; i < 2 ; i++)
        vecA.push_back(a);
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    cout << "=============" << endl;
    vector<Person> vecB = vecA;

    std::chrono::system_clock::time_point start2 = std::chrono::system_clock::now();
    vector<Person> vecC = std::move(vecA);
    cout << "=============" << endl;
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> sec = start2 - start;
    std::chrono::duration<double> sec2 = end - start2;
    std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << microfortnights(sec).count() << " seconds" << std::endl;
    std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << microfortnights(sec2).count() << " seconds" << std::endl;
    cout << "=============" << endl;
    std::shared_ptr<Person> personPtr( new Person());
    cout << "=============" << endl;

    return 0;
}
