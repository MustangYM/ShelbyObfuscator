#include "Utils.h"
#include "CryptoUtils.h"
#include "MBAUtils.h"
#include "MBAObfuscation.h"

#include <iostream>
#include <vector>

// namespace
using namespace llvm;
using std::vector;

static cl::opt<int>
    ObfuTimes("mba-times", cl::init(1),
              cl::desc("Run MBAObfuscation pass <mba-times> time(s)"));
static cl::opt<int>
    ObfuProb("mba-prob", cl::init(40),
             cl::desc("<mba-prob> percent chance to perform obfuscation for "
                      "each binary operation."));
static cl::opt<int>
    TermsNumber("linear-mba-terms", cl::init(10),
                cl::desc("Choose <linear-mba-terms> boolean exprs to construct "
                         "the linear MBA expr."));

PreservedAnalyses MBAObfuscation::run(Function& F, FunctionAnalysisManager& AM) {
    Function *tmp = &F; // 传入的Function
    // 判断是否需要开启线性 MBA 混淆
    // MBA -> Mixed Boolean-Arithmetic 混合布尔算术
    if (toObfuscate(flag, tmp, "mba")) {
        outs() << "\033[1;32m[MBAObfuscation] Function: " << F.getName() << "\033[0m\n"; // 打印一下被混淆函数的symbol
        INIT_CONTEXT(F);
        DoMBAObfuscation(*tmp);
    }
    return PreservedAnalyses::none();
}


void MBAObfuscation::substituteConstant(Instruction *I, int i) {
    ConstantInt *val = dyn_cast<ConstantInt>(I->getOperand(i));
    if (val && val->getBitWidth() <= 64) {
        int64_t *terms = generateLinearMBA(TermsNumber);
        terms[14] -= val->getValue().getZExtValue();
        Value *mbaExpr = insertLinearMBA(terms, I);
        if (val->getBitWidth() <= 32) {
            mbaExpr = insertPolynomialMBA(mbaExpr, I);
        }
        I->setOperand(i, mbaExpr);
    }
}

void MBAObfuscation::substitute(BinaryOperator *BI) {
    Value *mbaExpr = nullptr;
    switch (BI->getOpcode()) {
        case BinaryOperator::Add:
            mbaExpr = substituteAdd(BI);
            break;
        case BinaryOperator::Sub:
            mbaExpr = substituteSub(BI);
            break;
        case BinaryOperator::And:
            mbaExpr = substituteAnd(BI);
            break;
        case BinaryOperator::Or:
            mbaExpr = substituteOr(BI);
            break;
        case BinaryOperator::Xor:
            mbaExpr = substituteXor(BI);
            break;
        default:
            break;
    }
    if (mbaExpr) {
        if (BI->getOperand(0)->getType()->getIntegerBitWidth() <= 32) {
            mbaExpr = insertPolynomialMBA(mbaExpr, BI);
        }
        BI->replaceAllUsesWith(mbaExpr);
    }
}

Value *MBAObfuscation::substituteAdd(BinaryOperator *BI) {
    int64_t *terms = generateLinearMBA(TermsNumber);
    terms[2] += 1;
    terms[4] += 1;
    return insertLinearMBA(terms, BI);
}

Value *MBAObfuscation::substituteSub(BinaryOperator *BI) {
    int64_t *terms = generateLinearMBA(TermsNumber);
    terms[2] += 1;
    terms[4] -= 1;
    return insertLinearMBA(terms, BI);
}

Value *MBAObfuscation::substituteXor(BinaryOperator *BI) {
    int64_t *terms = generateLinearMBA(TermsNumber);
    terms[5] += 1;
    return insertLinearMBA(terms, BI);
}

Value *MBAObfuscation::substituteAnd(BinaryOperator *BI) {
    int64_t *terms = generateLinearMBA(TermsNumber);
    terms[0] += 1;
    return insertLinearMBA(terms, BI);
}

Value *MBAObfuscation::substituteOr(BinaryOperator *BI) {
    int64_t *terms = generateLinearMBA(TermsNumber);
    terms[6] += 1;
    return insertLinearMBA(terms, BI);
}

void MBAObfuscation::DoMBAObfuscation(Function &F){
    for (int i = 0; i < ObfuTimes; i++) {
        for (BasicBlock &BB : F) {
            std::vector<Instruction *> origInst;
            for (Instruction &I : BB) {
                origInst.push_back(&I);
            }
            for (Instruction *I : origInst) {
                if (isa<BinaryOperator>(I)) {
                    BinaryOperator *BI = cast<BinaryOperator>(I);
                    if (BI->getOperand(0)->getType()->isIntegerTy() &&
                        RANDOM(ObfuProb)) {
                            // Do not support 128-bit integers now
                            if (BI->getOperand(0)->getType()->getIntegerBitWidth() > 64) {
                                continue;
                            }
                            substitute(BI);
                    }
                } else {
                    for (int i = 0; i < I->getNumOperands(); i++) {
                        if (I->getOperand(0)->getType()->isIntegerTy() &&
                            RANDOM(ObfuProb)) {
                            // error occurs for unknown reasons
                            // if(isa<StoreInst>(I) || isa<CmpInst>(I) || isa<CallInst>(I)){
                            if (isa<StoreInst>(I) || isa<CmpInst>(I)) {
                                substituteConstant(I, i);
                            }
                        }
                    }
                }
            }
        }
    }
}

MBAObfuscation *llvm::createMBA(bool flag) {
    return new MBAObfuscation(flag);
}
