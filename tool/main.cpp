#include <cstdint>
#include <cstdlib>
#include <memory>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"

#include "sqlite3.h"

#ifndef CLANG_RESOURCE_DIR
#error "CLANG_RESOURCE_DIR must be defined by the build system."
#endif

using namespace clang::tooling;
using namespace llvm;

// The entrypoint for analysis. The VisitFunctionDecl function will run for
// each function declaration found in the source code we are analyzing.
class ErrorHandlingAnalysisVisitor
    : public clang::RecursiveASTVisitor<ErrorHandlingAnalysisVisitor> {
public:
  bool VisitFunctionDecl(clang::FunctionDecl *d) {
    // Logic for every function found
    d->dump();
    return true;
  }
};

// An ASTConsumer which delegates to the RecursiveASTVisitor
// ErrorHandlingAnalysisVisitor.
class ErrorHandlingAnalysisASTConsumer : public clang::ASTConsumer {
private:
  ErrorHandlingAnalysisVisitor _visitor;

public:
  // This method is called when a translation unit is parsed. Return false to
  // abort parsing.
  bool HandleTopLevelDecl(clang::DeclGroupRef declGroupRef) override {
    for (auto *decl : declGroupRef) {
      _visitor.TraverseDecl(decl);
    }

    return true;
  }
};

// An ASTFrontendAction which just returns the ASTConsumer for our error
// handling analysis.
class ErrorHandlingAnalysisAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &, StringRef) override {
    return std::make_unique<ErrorHandlingAnalysisASTConsumer>();
  }
};

static cl::OptionCategory Category("analyze-error-handling options");

// The main function just initializes and drives libTooling. Most of the work
// is done in the various classes defined in this file.
//
// The main work is done by defining an ASTFrontendAction, which as the name
// implies defines an action done by the frontend (clang). In our case, we just
// want to analyze the AST, so we just delegate to our ASTConsumer,
// ErrorHandlingAnalysisASTConsumer.
//
// An ASTConsumer is a class which consumes an AST, and does various things
// with it. Again, all we want to do is analyze it and not actually modify it
// or write to anything so we just delegate to our RecursiveASTVisitor, which
// allows running some code whenever certain AST nodes are visited by clang.
//
// And this is done through our ASTFrontendAction, ErrorHandlingAnalysisAction.
int main(int argc, const char **argv) {
  auto pRes = CommonOptionsParser::create(argc, argv, Category);
  if (!pRes) {
    llvm::logAllUnhandledErrors(pRes.takeError(), llvm::errs());
    return EXIT_FAILURE;
  }

  CommonOptionsParser &optionsParser = pRes.get();
  std::vector<std::string> sourcePaths = optionsParser.getSourcePathList();
  if (sourcePaths.empty()) {
    sourcePaths = optionsParser.getCompilations().getAllFiles();
  }
  ClangTool Tool(optionsParser.getCompilations(), sourcePaths);
  const std::string ResourceDir = CLANG_RESOURCE_DIR;
  if (!ResourceDir.empty()) {
    // Ensure builtin headers come from the LLVM install, not the host
    // toolchain. This is to prevent the "cannot find stddef.h" errors.
    const std::string ResourceArg = "-resource-dir=" + ResourceDir;
    Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster(
        ResourceArg.c_str(), ArgumentInsertPosition::BEGIN));
  }

  return Tool.run(
      newFrontendActionFactory<ErrorHandlingAnalysisAction>().get());
}
