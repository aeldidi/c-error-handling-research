#include <cstdint>
#include <cstdlib>

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"

#include "sqlite3.h"

#ifndef CLANG_RESOURCE_DIR
#error "CLANG_RESOURCE_DIR must be defined by the build system."
#endif

using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory Category("analyze-error-handling options");

// The main function just initializes and drives libTooling. Most of the work
// is done in the various classes defined in this file.
//
// The main work is done by defining a FrontendAction, which as the name
// implies defines an action done by the frontend (clang). In our case, we just
// want to analyze the AST, so we just delegate to our ASTConsumer,
// ErrorCheckConsumer.
//
// An ASTConsumer is a class which consumes an AST, and does various things
// with it. Again, all we want to do is analyze it and not actually modify it
// or write to anything so we just delegate to our RecursiveASTVisitor, which
// allows running some code whenever certain AST nodes are visited by clang.
int main(int argc, const char **argv) {
  auto pRes = CommonOptionsParser::create(argc, argv, Category);
  if (!pRes) {
    llvm::logAllUnhandledErrors(pRes.takeError(), llvm::errs());
    return EXIT_FAILURE;
  }

  CommonOptionsParser &OptionsParser = pRes.get();
  std::vector<std::string> SourcePaths = OptionsParser.getSourcePathList();
  if (SourcePaths.empty()) {
    SourcePaths = OptionsParser.getCompilations().getAllFiles();
  }
  ClangTool Tool(OptionsParser.getCompilations(), SourcePaths);
  const std::string ResourceDir = CLANG_RESOURCE_DIR;
  if (!ResourceDir.empty()) {
    // Ensure builtin headers come from the LLVM install, not the host
    // toolchain. This is to prevent the "cannot find stddef.h" errors.
    const std::string ResourceArg = "-resource-dir=" + ResourceDir;
    Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster(
        ResourceArg.c_str(), ArgumentInsertPosition::BEGIN));
  }

  return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
}
