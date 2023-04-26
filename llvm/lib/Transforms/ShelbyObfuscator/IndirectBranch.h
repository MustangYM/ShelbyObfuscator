#ifndef LLVM_INDIRECTBRANCH_H
#define LLVM_INDIRECTBRANCH_H
// LLVM libs
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
// User libs
#include "Utils.h"
using namespace llvm;
using namespace std;
namespace llvm{ // 间接跳转
    class IndirectBranchPass : public PassInfoMixin<IndirectBranchPass>{
        public:
            bool flag;
            bool initialized;
            map<BasicBlock *, unsigned long long> indexmap;
            IndirectBranchPass(bool flag){
                this->flag = flag;
            } // 携带flag的构造函数
            PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM); // Pass实现函数
            bool initialize(Module &M); // 开始初始化
            bool HandleFunction(Function &Func); // 处理间接跳转
            bool doFinalization(Module &M); // 结束初始化
            static bool isRequired() { return true; } // 直接返回true即可
    };
    IndirectBranchPass *createIndirectBranch(bool flag); // 创建间接跳转
}
#endif // LLVM_INDIRECTBRANCH_H