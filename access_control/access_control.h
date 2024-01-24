#include <iostream>
#include <string>

class AccessTest
{
public:
    const int a;
    const int b;
    const std::string c;
};


void test_access()
{
    AccessTest test;
    muta test.a = 1;
    test.b = 2;
    test.c = "123";
    const AccessTest& test1 = test;
    // test1.a = 2;

    // modify(test1);
    std::cout << test.a << " " << test.b << " " << test.c << std::endl;
}
