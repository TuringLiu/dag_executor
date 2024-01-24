// [Deprecated] 
// This method meet some problems in [static member template varaibles initialization and not used].
// Please use dynamic_factory2.h instead
#include <functional>
#include <unordered_map>
#include <iostream>

template<typename Base>
class DynamicCreator;

// get the class name by class type
template<typename T>
std::string get_type_name();

// 负责存储
template<typename Base, typename ...Args>
class DynamicFactory
{
public:
    friend class DynamicCreator<Base>;
    static void register_(const std::string& name, std::function<Base*(Args...)> ctor) {
        m_ctors[name] = ctor;
    }
public:
    static std::unordered_map<std::string, std::function<Base*(Args...)>> m_ctors;
};

// // 需要对模板类的静态成员变量进行初始化操作
template<typename Base, typename ...Args>
std::unordered_map<std::string, std::function<Base*(Args...)>> DynamicFactory<Base, Args...>::m_ctors;

// 负责被继承
template<typename Base, typename Derived, typename ...Args>
class DynamicWorker
{
    // // 负责注册
    // template<typename Base, typename Derived, typename ...Args>
    struct DynamicRegister
    { 
        DynamicRegister()
        {
            std::cout << "DynamicRegister " << get_type_name<Derived>() << std::endl;
            DynamicFactory<Base, Args...>::m_ctors[get_type_name<Derived>()] = [](Args... args) -> Base*{ 
                return new Derived(args...); 
            };

            // DynamicFactory<Base, Args...>::register_(get_type_name<Derived>(), [](Args... args) -> Base*{ 
            //     return new Derived(args...); 
            // });
        }
    };

public:
    DynamicWorker()
    {
        std::cout << "DynamicWorker " << get_type_name<Derived>() << std::endl;
        m_register_;
    }
private:
  static DynamicRegister m_register_;
};

// class Test;
// static DynamicFactory<Test, int, int> a;

// template<typename Base, typename Derived, typename ...Args>
// DynamicWorker<Base, Derived, Args...>::DynamicRegister DynamicWorker<Base, Derived, Args...>::m_register_;

template<typename Base, typename Derived, typename ...Args>
typename DynamicWorker<Base, Derived, Args...>::DynamicRegister DynamicWorker<Base, Derived, Args...>::m_register_;

// 负责创造
template<typename Base>
class DynamicCreator
{
public:
    template<typename ...Args>
    static Base* create(const std::string& name, Args... args)
    {
        auto it = DynamicFactory<Base, Args...>::m_ctors.find(name);
        if(it != DynamicFactory<Base, Args...>::m_ctors.end())
        {
            return it->second(args...);
        }
        std::cout << "can't find class " << name << std::endl;
        return nullptr;
    }
};


class Test
{
public:
    Test() {std::cout << " Test created! " << std::endl;}
    Test(int a, int b) {std::cout << a << " Test created! " << b << std::endl;}
    virtual void print() = 0;
};

class Test_66 : public Test, public DynamicWorker<Test, Test_66, int, int>
{
public:
    Test_66(int a) {std::cout << a << " Test_66 created ! " << std::endl;}
    Test_66(int a, int b):a_{a}, b_{b} { std::cout << a << " Test_66 created ! " << b << std::endl;}
    void print() override {std::cout << a_ << " " << b_ << std::endl;}
    int a_;
    int b_;
};

class Test_666 : public Test, public DynamicWorker<Test, Test_666, int, std::string>
{
public:
    Test_666(int a) {std::cout << a << " Test_66 created ! " << std::endl;}
    Test_666(int a, std::string b):a_{a}, b_{b} { std::cout << a << " Test_66 created ! " << b << std::endl;}
    void print() override {std::cout << a_ << " " << b_ << std::endl;}
    int a_;
    std::string b_;
};

void test_dynamic_factory()
{
    // std::cout << DynamicFactory<Test, int, int>::m_ctors.size() << std::endl;

    Test* test_66 = DynamicCreator<Test>::create("Test_66", 1, 2);
    if(test_66) test_66->print();

    Test* test_666 = DynamicCreator<Test>::create("Test_666", 1, "----");
    if(test_666) test_666->print();
}