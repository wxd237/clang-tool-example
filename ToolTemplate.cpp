// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"


#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"

#include<iostream>
#include<string>

using namespace clang::tooling;
using namespace llvm;
using namespace clang;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("clang-ref options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");


void replacespace(char * p,size_t l){
	for(int i=0;i<l;i++){
		if(p[i]=='\n')
		p[i]=' ';
	}
}


class FindNamedClassVisitor
  : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    // For debugging, dumping the AST nodes will show which nodes are already
    // being visited.
   // Declaration->dump();

    // The return value indicates whether we want the visitation to proceed.
    // Return false to stop the traversal of the AST.
    return true;
  }
  

  std::string curfunction;

  bool VisitFunctionDecl(FunctionDecl * decl){
    // decl->dump();
   
    ASTContext & Context =decl->getASTContext();
    SourceManager & SM= Context.getSourceManager ();
    if(SM.isInMainFile(decl->getBeginLoc())==false){
		return true;
    }
	
    unsigned int paraNum = decl->getNumParams();


	
    const char * b=SM.getCharacterData(decl->getBeginLoc());
    const char * e=SM.getCharacterData(decl->getBody()->getBeginLoc());
    size_t len=e-b;
    char buf[len+1];
    strncpy(buf,b,len);

    replacespace(buf,len);
    
    std::cout<<"defun@:"<<buf<<std::endl;
    
	

    auto curFileID=SM.getFileID(decl->getBeginLoc());
    auto mainFileID=SM.getMainFileID();

    std::string t1=decl->getSourceRange().printToString(SM);

    if(curFileID==mainFileID)
    	curfunction=decl->getNameInfo().getAsString();
    else
	curfunction="";

    //std::cout<<"functiondecl:"<<decl->getNameInfo().getAsString()<<std::endl;
    //std::cout<<decl->getDeclKindName()<<std::endl;
    
    return true;
  }


bool VisitCallExpr(CallExpr * expr){
    // decl->dump();
    
    if(expr->getDirectCallee()==nullptr)  return true;

    std::string bcall=expr->getDirectCallee()->getNameInfo().getAsString();
	
    if(bcall=="size")  return true;
    if(bcall=="printf")  return true;
    if(bcall=="sprintf")  return true;
    if(bcall=="strcpy")  return true;
    if(bcall=="strncpy")  return true;
   

    if(curfunction.size()>0)
    	std::cout<<"callref@"<<curfunction<<">"<<bcall<<std::endl;
    
    

    return true;
  }

  


};

//cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
//
class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    // Traversing the translation unit decl via a RecursiveASTVisitor
    // will visit all nodes in the AST.
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  // A RecursiveASTVisitor implementation.
  FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<FindNamedClassConsumer>();
  }


};



int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

 // return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());

  return Tool.run(newFrontendActionFactory<FindNamedClassAction>().get());
}
