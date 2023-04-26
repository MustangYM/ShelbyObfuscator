/**
 * @file Utils.cpp
 * @author SsageParuders
 * @brief 本代码参考原OLLVM项目:https://github.com/obfuscator-llvm/obfuscator
 *        感谢地球人前辈的指点
 * @version 0.1
 * @date 2022-07-14
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Utils.h"

using namespace llvm;
using std::vector;

LLVMContext *CONTEXT = nullptr;

/**
 * @brief 参考资料:https://www.jianshu.com/p/0567346fd5e8
 *        作用是读取llvm.global.annotations中的annotation值 从而实现过滤函数 只对单独某功能开启PASS
 * @param f
 * @return std::string
 */
std::string llvm::readAnnotate(Function *f){ //取自原版ollvm项目
    std::string annotation = "";
    /* Get annotation variable */
    GlobalVariable *glob=f->getParent()->getGlobalVariable( "llvm.global.annotations" );
    if ( glob != NULL ){
        /* Get the array */
        if ( ConstantArray * ca = dyn_cast<ConstantArray>( glob->getInitializer() ) ){
            for ( unsigned i = 0; i < ca->getNumOperands(); ++i ){
                /* Get the struct */
                if ( ConstantStruct * structAn = dyn_cast<ConstantStruct>( ca->getOperand( i ) ) ){
                    if ( ConstantExpr * expr = dyn_cast<ConstantExpr>( structAn->getOperand( 0 ) ) ){
                        /*
                         * If it's a bitcast we can check if the annotation is concerning
                         * the current function
                         */
                        if ( expr->getOpcode() == Instruction::BitCast && expr->getOperand( 0 ) == f ){
                            ConstantExpr *note = cast<ConstantExpr>( structAn->getOperand( 1 ) );
                            /*
                             * If it's a GetElementPtr, that means we found the variable
                             * containing the annotations
                             */
                            if ( note->getOpcode() == Instruction::GetElementPtr ){
                                if ( GlobalVariable * annoteStr = dyn_cast<GlobalVariable>( note->getOperand( 0 ) ) ){
                                    if ( ConstantDataSequential * data = dyn_cast<ConstantDataSequential>( annoteStr->getInitializer() ) ){
                                        if ( data->isString() ){
                                            annotation += data->getAsString().lower() + " ";
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return(annotation);
}

/**
 * @brief 用于判断是否开启混淆
 * 
 * @param flag 
 * @param f 
 * @param attribute 
 * @return true 
 * @return false 
 */
bool llvm::toObfuscate(bool flag, Function *f, std::string const &attribute) { //取自原版ollvm项目
//    std::string attr = attribute;
//    std::string attrNo = "no" + attr;
//    // Check if declaration
//    if (f->isDeclaration()) {
//        return false;
//    }
//    // Check external linkage
//    if (f->hasAvailableExternallyLinkage() != 0) {
//        return false;
//    }
//    // We have to check the nofla flag first
//    // Because .find("fla") is true for a string like "fla" or
//    // "nofla"
//    if (readAnnotate(f).find(attrNo) != std::string::npos) { // 是否禁止开启XXX
//        return false;
//    }
//    // If fla annotations
//    if (readAnnotate(f).find(attr) != std::string::npos) { // 是否开启XXX
//        return true;
//    }
//    // If fla flag is set
//    if (flag == true) { // 开启PASS
//        return true;
//    }
    return true;
}

/**
 * @brief 修复PHI指令和逃逸变量
 * 
 * @param F 
 */
void llvm::fixStack(Function &F) {
    vector<PHINode*> origPHI;
    vector<Instruction*> origReg;
    BasicBlock &entryBB = F.getEntryBlock();
    for(BasicBlock &BB : F){
        for(Instruction &I : BB){
            if(PHINode *PN = dyn_cast<PHINode>(&I)){
                origPHI.push_back(PN);
            }else if(!(isa<AllocaInst>(&I) && I.getParent() == &entryBB) 
                && I.isUsedOutsideOfBlock(&BB)){
                origReg.push_back(&I);
            }
        }
    }
    for(PHINode *PN : origPHI){
        DemotePHIToStack(PN, entryBB.getTerminator());
    }
    for(Instruction *I : origReg){
        DemoteRegToStack(*I, entryBB.getTerminator());
    }
}

/**
 * @brief 
 * 
 * @param Func 
 */
void llvm::FixFunctionConstantExpr(Function *Func) {
  // Replace ConstantExpr with equal instructions
  // Otherwise replacing on Constant will crash the compiler
  for (BasicBlock &BB : *Func) {
    FixBasicBlockConstantExpr(&BB);
  }
}
/**
 * @brief 
 * 
 * @param BB 
 */
void llvm::FixBasicBlockConstantExpr(BasicBlock *BB) {
  // Replace ConstantExpr with equal instructions
  // Otherwise replacing on Constant will crash the compiler
  // Things to note:
  // - Phis must be placed at BB start so CEs must be placed prior to current BB
  assert(!BB->empty() && "BasicBlock is empty!");
  assert((BB->getParent() != NULL) && "BasicBlock must be in a Function!");
  Instruction *FunctionInsertPt = &*(BB->getParent()->getEntryBlock().getFirstInsertionPt());
  // Instruction* LocalBBInsertPt=&*(BB.getFirstInsertionPt());
  for (Instruction &I : *BB) {
    if (isa<LandingPadInst>(I) || isa<FuncletPadInst>(I)) {
        continue;
    }
    for (unsigned i = 0; i < I.getNumOperands(); i++) {
      if (ConstantExpr *C = dyn_cast<ConstantExpr>(I.getOperand(i))) {
        Instruction *InsertPt = &I;
        IRBuilder<NoFolder> IRB(InsertPt);
        if (isa<PHINode>(I)) {
          IRB.SetInsertPoint(FunctionInsertPt);
        }
        Instruction *Inst = IRB.Insert(C->getAsInstruction());
        I.setOperand(i, Inst);
      }
    }
  }
}

/**
 * @brief 随机字符串
 * 
 * @param len 
 * @return string 
 */
string llvm::rand_str(int len){
    string str;
    char c = 'O';
    int idx;
    for (idx = 0; idx < len; idx++){
        
        switch ((rand() % 3)){
            case 1:
                c = 'O';
                break;
            case 2:
                c = '0';
                break;
            default:
                c = 'o';
                break;
		}
        str.push_back(c);
    }
    return str;
}
