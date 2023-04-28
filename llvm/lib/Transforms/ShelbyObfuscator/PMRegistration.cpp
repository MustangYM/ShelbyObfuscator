#include "SplitBasicBlock.h" // 基本块分割
#include "Flattening.h"  // 控制流平坦化
#include "MBAObfuscation.h" // 线性混合布尔算术混淆
#include "StringEncryption.h" // 字符串加密
#include "VMFlatten.h" // 虚拟机控制流平坦化
#include "IndirectBranch.h" // 间接跳转
#include "FunctionWrapper.h" // 函数嵌套
#include "FlatteningEnhanced.h" // 平坦化控制流增强版
#include "BogusControlFlow.h" // 虚假控制流
#include "llvm/Transforms/Utils/SymbolRewriter.h" // 重命名符号
#include "IndirectCall.h" // 间接调用
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

/**
 * @brief Get the Ssage Plugin Info object
 * 
 * @return llvm::PassPluginLibraryInfo 
 */
llvm::PassPluginLibraryInfo getShelbyPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "Shelby", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            outs() << "Version is " << 25 << "\n";
            PB.registerPipelineStartEPCallback(
                [](llvm::ModulePassManager &MPM, // 模块Pass 作用于某个c文件内
                   llvm::OptimizationLevel Level){
                    MPM.addPass(StringEncryptionPass(false)); // 先进行字符串加密 出现字符串加密基本块以后 再进行基本块分割和其他混淆 加大解密难度
                    llvm::FunctionPassManager FPM;
                    FPM.addPass(IndirectCallPass(false)); // 来自goron的间接调用
                    FPM.addPass(SplitBasicBlockPass(false));  // 优先进行基本块分割
                    FPM.addPass(VMFlattenPass(false)); // 虚拟机控制流平坦化
                    FPM.addPass(FlatteningPass(false));       // 对于控制流平坦化 不提前开启LowerSwitch 只在控制流平坦化内调用LegacyLowerSwitch
                    FPM.addPass(MBAObfuscation(false)); // 来自 Pluto 的线性混合布尔算术混淆
                    FPM.addPass(BogusControlFlowPass(false)); // 虚假控制流
                    MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
                    MPM.addPass(FlatteningEnhanced(false)); // 来自 Pluto 的平坦化控制流增强版
                    MPM.addPass(FunctionWrapperPass(false)); // 函数包装 理论上函数包装最好也是放在最后
                    MPM.addPass(IndirectBranchPass(false)); // 间接指令 理论上间接指令应该放在最后
                    MPM.addPass(RewriteSymbolPass()); // 根据yaml信息 重命名特定symbols
            });
            outs() << "Made By SsageParuders\n";
        }};
    
}

/**
 * @brief NEW PM
 * 
 */
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getShelbyPluginInfo();
}

