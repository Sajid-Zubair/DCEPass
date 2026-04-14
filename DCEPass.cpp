#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include <set>

using namespace llvm;

struct DCEPass : public PassInfoMixin<DCEPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

        std::set<Value*> allocas;
        std::set<Value*> usedAllocas;

        // 🔹 Step 1: Collect allocas
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *AI = dyn_cast<AllocaInst>(&I)) {
                    allocas.insert(AI);
                }
            }
        }

        // 🔹 Step 2: Track loads (reads)
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *LI = dyn_cast<LoadInst>(&I)) {
                    usedAllocas.insert(LI->getPointerOperand());
                }
            }
        }

        // 🔹 Step 3: Remove dead stores
        for (auto &BB : F) {
            for (auto it = BB.begin(); it != BB.end(); ) {

                Instruction &I = *it++;

                if (auto *SI = dyn_cast<StoreInst>(&I)) {

                    Value *ptr = SI->getPointerOperand();

                    if (allocas.count(ptr) && !usedAllocas.count(ptr)) {
                        SI->eraseFromParent(); // remove dead store
                    }
                }
            }
        }

        // 🔹 Step 4: Remove dead allocas
        for (auto &BB : F) {
            for (auto it = BB.begin(); it != BB.end(); ) {

                Instruction &I = *it++;

                if (auto *AI = dyn_cast<AllocaInst>(&I)) {

                    if (AI->use_empty()) {
                        AI->eraseFromParent(); // remove unused variable
                    }
                }
            }
        }

        return PreservedAnalyses::none();
    }
};

// Registration (same)
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "DCEPass",
        LLVM_VERSION_STRING,

        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                    if (Name == "dce-pass") {
                        FPM.addPass(DCEPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}



// #include "llvm/IR/PassManager.h"
// #include "llvm/IR/Function.h"
// #include "llvm/IR/Instruction.h"
// #include "llvm/Passes/PassBuilder.h"
// #include "llvm/Passes/PassPlugin.h"

// using namespace llvm;

// // Step 1: Define the pass
// struct DCEPass : public PassInfoMixin<DCEPass> {

//     // This function runs on each function in IR
//     PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
//         errs() << "DCE running on function: " << F.getName() << "\n";

//         for (auto &BB : F) {   // Loop over basic blocks

//             for (auto it = BB.begin(); it != BB.end(); ) {

//                 Instruction &I = *it++;   // Move iterator safely

//                 // Step 2: Check if instruction is dead
//                 if (I.use_empty() && !I.isTerminator() && !I.mayHaveSideEffects()) {
//                     I.eraseFromParent();   // Delete instruction
//                 }
//             }
//         }

//         return PreservedAnalyses::none();
//     }
// };

// // Step 3: Register the pass with LLVM
// extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
//     return {
//         LLVM_PLUGIN_API_VERSION,   // LLVM version compatibility
//         "DCEPass",                 // Plugin name
//         LLVM_VERSION_STRING,

//         [](PassBuilder &PB) {
//             PB.registerPipelineParsingCallback(
//                 [](StringRef Name, FunctionPassManager &FPM,
//                    ArrayRef<PassBuilder::PipelineElement>) {

//                     if (Name == "dce-pass") {
//                         FPM.addPass(DCEPass());   // Add our pass
//                         return true;
//                     }
//                     return false;
//                 });
//         }
//     };
// }

