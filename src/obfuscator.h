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

namespace wabt{


class Obfuscator{
  public:
  Obfuscator();
  Obfuscator(double);
  Result Obfuscate(std::unique_ptr<Module>* out_module);
  Result ObfuscateModule(Module* module);
  void ObfuscateFuncs(Func* func);
  void ObfuscateExprList(ExprList* exprs);
  size_t ObfuscateExpr(ExprList* exprs, intrusive_list<Expr>::iterator iter);
  size_t GetExprSize(const Expr* expr);
  size_t GetExpsListSize(const ExprList& exprs);
  size_t GetFuncSize(const Func* func);
  size_t GetModuleSize(const Module* module);
  bool IsValidFuncForJunkCodeInsertion(Func* func);
  void CreateInsertSet(const Module* module);
  void CreateRandomGarbageExprs(ExprList* exprs);
  bool MatchObfuscateExprType(intrusive_list<Expr>::iterator expr);

  private:
  double insert_rate_; // 0-1
  size_t insert_lines_;
  size_t instr_lines_; //lines that can insert garbage instructions, now only instructions in a func
  // size_t inserted_count_;
  int cur_;
  int* insert_array_;
};
}