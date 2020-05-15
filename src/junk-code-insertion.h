#include "src/binary-reader-ir.h"
#include "src/binary-reader.h"
#include "src/common.h"
#include "src/expr-visitor.h"
#include "src/make-unique.h"
#include "src/resolve-names.h"
#include "src/stream.h"
#include "src/utf8.h"
#include "src/validator.h"

#include "src/circular-array.h"
#include "src/error.h"
#include "src/feature.h"
#include "src/intrusive-list.h"
#include "src/ir.h"
#include "src/result.h"
#include "src/wast-lexer.h"
#include "src/wast-parser.h"

using namespace wabt;

class JunkCodeInsertion{
  public:
  JunkCodeInsertion();
  JunkCodeInsertion(double);
  Result Obfuscate(std::unique_ptr<Module>* out_module);

  private:
  Result ObfuscateModule(Module* module);
  void ObfuscateFuncs(Func* func);
  void ObfuscateExprList(ExprList* exprs);
  size_t ObfuscateExpr(ExprList* exprs, intrusive_list<Expr>::iterator iter);
  // size_t GetExprSize(const Expr* expr);
  // size_t GetExpsListSize(const ExprList& exprs);
  // size_t GetFuncSize(const Func* func);
  // size_t GetModuleSize(const Module* module);
  // void CreateInsertSet(const Module* module);
  bool IsValidFuncForJunkCodeInsertion(Func* func);
  void CreateGarbageExprs(ExprList* exprs);
  bool MatchObfuscateExprType(intrusive_list<Expr>::iterator expr);

  double insert_rate_; // 0-1
};