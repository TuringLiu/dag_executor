#include "dynamic_factory.h"

template<> 
std::unordered_map<std::string, std::function<Test*(int, int)>> DynamicFactory<Test, int, int>::m_ctors;

template<>
typename DynamicWorker<Test, Test_66, int, int>::DynamicRegister DynamicWorker<Test, Test_66, int, int>::m_register_;
