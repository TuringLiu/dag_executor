#include <functional>
#include <iostream>
#include <unordered_map>

template<typename Base, typename ...Args>
class DynamicCreator;

// 负责存储
template<typename Base, typename ...Args>
class DynamicFactory
{
public:
    friend class DynamicCreator<Base, Args...>;
    void register_(const std::string& name, std::function<Base*(Args...)> ctor) {
        m_ctors_[name] = ctor;
    }
private:
    std::unordered_map<std::string, std::function<Base*(Args...)>> m_ctors_;
};

// 负责创造
template<typename Base, typename ...Args>
class DynamicCreator
{
public:
    DynamicCreator(DynamicFactory<Base, Args...> dynamic_factory): dynamic_factory_(dynamic_factory) {}
    
    Base* create(const std::string& name, Args... args)
    {
        auto it = dynamic_factory_.m_ctors_.find(name);
        if(it != dynamic_factory_.m_ctors_.end())
        {
            return it->second(args...);
        }
        std::cout << "can't find class " << name << std::endl;
        return nullptr;
    }
private:
    DynamicFactory<Base, Args...>& dynamic_factory_;
};


// ------------------------------ test case ------------------------
class Test
{
public:
    Test() {std::cout << " Test created! " << std::endl;}
    Test(int a, int b) {std::cout << a << " Test created! " << b << std::endl;}
    virtual void print() = 0;
};

class Test_66 : public Test
{
public:
    Test_66(int a) {std::cout << a << " Test_66 created ! " << std::endl;}
    Test_66(int a, int b):a_{a}, b_{b} { std::cout << a << " Test_66 created ! " << b << std::endl;}
    void print() override {std::cout << a_ << " " << b_ << std::endl;}
    int a_;
    int b_;
};

class Test_666 : public Test
{
public:
    Test_666(int a) {std::cout << a << " Test_666 created ! " << std::endl;}
    Test_666(int a, int b):a_{a}, b_{b} { std::cout << a << " Test_666 created ! " << b << std::endl;}
    void print() override {std::cout << a_ << " " << b_ << std::endl;}
    int a_;
    int b_;
};

// template<>
// std::unordered_map<std::string, std::function<Test*(int, int)>> DynamicFactory<Test,int, int>::m_ctors_;

DynamicFactory<Test,int, int>& regist()
{
    static DynamicFactory<Test,int, int> dynamic_factory;
    dynamic_factory.register_("Test_66", [](int a, int b) -> Test*{return new Test_66(a, b);});
    dynamic_factory.register_("Test_666", [](int a, int b) -> Test*{return new Test_666(a, b);});

    return dynamic_factory;
}


void test_dynamic_factory()
{
    DynamicCreator<Test,int,int> dynamic_creator(regist());
    Test* test_66 = dynamic_creator.create("Test_66", 1, 2);
    Test* test_666 = dynamic_creator.create("Test_666", 1, 2);
}