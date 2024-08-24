
#ifndef BTF_TESTER_HPP
#define BTF_TESTER_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>

typedef void(*CallableVoid)();

class AllTestCaseManager {
private:
    std::map<std::string, CallableVoid> allcases;

public:
    static AllTestCaseManager& instance() {
        static AllTestCaseManager manager;
        return manager;
    }


    void registeTestCase(std::string casename, CallableVoid function) {
        allcases[casename] = function;
    }

    void runAllTestCase() {
        auto itr = allcases.begin();
        for(; itr != allcases.end(); itr++) {
            std::cout << "Run " << itr->first << std::endl;
            itr->second();
        }
    }
};

class TestCaseResult {
    enum class ResultType {Failure, Success, Skip, Error};
    struct Case {
        std::string case_name;
        ResultType result;
        std::string info;
    };
public:
    std::vector<Case> testResult;
};

class TestCaseReg {
    TestCaseReg(std::string cn, CallableVoid function) {
        auto& tcm = AllTestCaseManager::instance();
        tcm.registeTestCase(cn , function);
    }
};



#define TEST_CASE(case_name, description) \
        void case_name (void); \ 
        TestCaseReg reg_ ## case_name {std::string(#case_name), case_name};\
        void case_name(void)

class TestError: public std::exception {
public:
    TestError()
};

#define ASSERT_TRUE(expr) \
try: \ 
    if ((expr)) { \
    } else { \
        throw TestCaseReg;\
    } \
catch (const std::exception & excep) { \

    }
        





#endif // BTF_TESTER_HPP