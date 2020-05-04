#include <iterator>

#include "src/obfuscator.h"

#include "src/cast.h"


namespace wabt{

void SetRandomGenerator(){
    std::srand(std::time(0));
}
double GetRandom(){
    return (double)std::rand() / (RAND_MAX);
}
uint32_t GetRandomInt(){
    return (uint32_t)std::rand();
}

Obfuscator::Obfuscator() :
insert_rate_(0.7), cur_(0), instr_lines_(0){}

Obfuscator::Obfuscator(double insert_rate) :
insert_rate_(insert_rate), cur_(0), instr_lines_(0){}

void Obfuscator::CreateRandomGarbageExprs(ExprList* exprs){
    //create i32.const 0 & add to an existing local
    // FuncDeclaration* decl = func->decl;  
    // FuncSignature decl->sig;
    // param_types

    // LocalTypes local_types;
    // BindingHash bindings;
    // ExprList exprs;    
    Location loc = Location();
    int intervals = 5;
    double roll_val = GetRandom(); 
    std::unique_ptr<Expr> expr;
    if (roll_val < 0.8){
        expr.reset(new LocalGetExpr(Var(0,loc)));
        exprs->push_back(std::move(expr));
        if(roll_val < 0.6){
            if (roll_val < 0.4){
                if (roll_val < 0.2){
                    expr.reset(new ConstExpr(Const(), loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new BinaryExpr(Opcode::I32Add,loc));
                    exprs->push_back(std::move(expr));
                }else{
                    uint32_t rand_val = GetRandomInt();
                    Const const_ = Const();
                    const_.set_u32(rand_val);
                    expr.reset(new ConstExpr(const_, loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new BinaryExpr(Opcode::I32Xor, loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new ConstExpr(const_, loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new BinaryExpr(Opcode::I32Xor, loc));
                    exprs->push_back(std::move(expr));
                }
            }else{
                    uint32_t rand_val = GetRandomInt();
                    Const const_ = Const();
                    const_.set_u32(rand_val);
                    expr.reset(new ConstExpr(const_,loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new ConstExpr(Const(),loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new BinaryExpr(Opcode::I32Mul, loc));
                    exprs->push_back(std::move(expr));
                    expr.reset(new BinaryExpr(Opcode::I32Add, loc));
                    exprs->push_back(std::move(expr));

            }
        }
        expr.reset(new LocalSetExpr(Var(0,loc)));
        exprs->push_back(std::move(expr));
    }else{
        expr.reset(new NopExpr(loc));
        // std::unique_ptr<Expr>* out_expr
        exprs->push_back(std::move(expr));
    }
}

bool Obfuscator::MatchObfuscateExprType(intrusive_list<Expr>::iterator expr){
    switch(expr->type()){
        //block instructions
        case ExprType::Block:
        case ExprType::If:
        case ExprType::Try:
        case ExprType::Loop:
        //plain insrtuctions
        case ExprType::Br:
        case ExprType::BrIf:
        case ExprType::BrOnExn:
        case ExprType::BrTable:
        case ExprType::Call:
        case ExprType::ReturnCall:
        case ExprType::CallIndirect:
        case ExprType::ReturnCallIndirect:
        case ExprType::Compare:
        case ExprType::Const:     
        case ExprType::LocalGet:
        case ExprType::LocalSet:
        case ExprType::GlobalGet:
        case ExprType::GlobalSet:
        case ExprType::Nop:    
        case ExprType::Unreachable:
        // unchecked validity
        case ExprType::Load: //LoadStoreExpr
        case ExprType::Store: //LoadStoreExpr
        case ExprType::LoadSplat: //LoadStoreExpr
        case ExprType::AtomicNotify: //LoadStoreExpr  
        case ExprType::AtomicRmwCmpxchg: //LoadStoreExpr
        case ExprType::AtomicLoad: //LoadStoreExpr
        case ExprType::AtomicRmw: //LoadStoreExpr
        case ExprType::AtomicStore: //LoadStoreExpr
        case ExprType::AtomicWait: //LoadStoreExpr
        case ExprType::Binary:
        case ExprType::Convert:
        case ExprType::Drop:
        case ExprType::LocalTee:
        case ExprType::MemoryCopy:
        case ExprType::DataDrop:
        case ExprType::MemoryFill:
        case ExprType::MemoryGrow:
        case ExprType::MemoryInit:
        case ExprType::MemorySize:
        case ExprType::TableCopy:
        case ExprType::ElemDrop:
        case ExprType::TableInit:
        case ExprType::TableGet:
        case ExprType::TableSet:
        case ExprType::TableGrow:
        case ExprType::TableSize:
        case ExprType::TableFill:
        case ExprType::RefFunc:
        case ExprType::RefNull:
        case ExprType::RefIsNull:
        case ExprType::Rethrow:
        case ExprType::Return:
        case ExprType::Select:
        case ExprType::Throw:
        case ExprType::Unary:
        case ExprType::Ternary:
        case ExprType::SimdLaneOp:
        case ExprType::SimdShuffleOp:
            return 1;
    }
    return 0;
}

size_t Obfuscator::ObfuscateExpr(ExprList* exprs, intrusive_list<Expr>::iterator iter){
    ExprList garbageExprs;
    size_t garbageExprsSize = 0;
    // intrusive_list<Expr>::iterator next_iter = iter;
    // next_iter++;
    switch(iter->type()){
        //block instructions
        case ExprType::Block:{
            BlockExpr* blk_expr = cast<BlockExpr>(&(*iter));
            if(!blk_expr->block.exprs.empty())
                ObfuscateExprList(&(blk_expr->block.exprs));
            if (GetRandom() < insert_rate_){
                CreateRandomGarbageExprs(&garbageExprs);
                garbageExprsSize = garbageExprs.size();
                exprs->splice(++iter, garbageExprs);
            }
            break;
        }
        case ExprType::If:{
            IfExpr* if_expr = cast<IfExpr>(&(*iter));
            if(!if_expr->true_.exprs.empty())
                ObfuscateExprList(&(if_expr->true_.exprs));
            if(!if_expr->false_.empty())
                ObfuscateExprList(&(if_expr->false_));
            if (GetRandom() < insert_rate_){
                CreateRandomGarbageExprs(&garbageExprs);
                garbageExprsSize = garbageExprs.size();
                exprs->splice(++iter, garbageExprs);
            }
            break;
        }
        case ExprType::Try: {
            TryExpr* try_expr = cast<TryExpr>(&(*iter));
            ObfuscateExprList(&(try_expr->block.exprs));
            ObfuscateExprList(&(try_expr->catch_));
            if (GetRandom() < insert_rate_){
                CreateRandomGarbageExprs(&garbageExprs);
                garbageExprsSize = garbageExprs.size();
                exprs->splice(++iter, garbageExprs);
            }
            break;
        }
        case ExprType::Loop:{
            LoopExpr* loop_expr = cast<LoopExpr>(&(*iter));
            if(!loop_expr->block.exprs.empty()){
                ObfuscateExprList(&(loop_expr->block.exprs));
            }
            if (GetRandom() < insert_rate_){
                CreateRandomGarbageExprs(&garbageExprs);
                garbageExprsSize = garbageExprs.size();
                exprs->splice(++iter, garbageExprs);
            }
            break;
        }
        // plain insrtuctions
        case ExprType::Br:
        case ExprType::BrIf:
        case ExprType::BrOnExn:
        case ExprType::BrTable:
        case ExprType::Call:
        case ExprType::ReturnCall:
        case ExprType::CallIndirect:
        case ExprType::ReturnCallIndirect:
        case ExprType::Compare:
        case ExprType::Const:
            // switch (const_.type()) {
            //     case Type::I32:
            //         break;
            //     case Type::I64:
            //         break;
            //     case Type::F32:
            //         break;
            //     case Type::F64:
            //         break;
            //     case Type::V128:
            //         break;
            //     default:
            // }         
        case ExprType::LocalGet:
        case ExprType::LocalSet:
        case ExprType::GlobalGet:
        case ExprType::GlobalSet:
        case ExprType::Nop:    
        case ExprType::Unreachable:
        // unchecked validity
        case ExprType::Load: //LoadStoreExpr
        case ExprType::Store: //LoadStoreExpr
        case ExprType::LoadSplat: //LoadStoreExpr
        case ExprType::AtomicNotify: //LoadStoreExpr  
        case ExprType::AtomicRmwCmpxchg: //LoadStoreExpr
        case ExprType::AtomicLoad: //LoadStoreExpr
        case ExprType::AtomicRmw: //LoadStoreExpr
        case ExprType::AtomicStore: //LoadStoreExpr
        case ExprType::AtomicWait: //LoadStoreExpr
        case ExprType::Binary:
        case ExprType::Convert:
        case ExprType::Drop:
        case ExprType::LocalTee:
        case ExprType::MemoryCopy:
        case ExprType::DataDrop:
        case ExprType::MemoryFill:
        case ExprType::MemoryGrow:
        case ExprType::MemoryInit:
        case ExprType::MemorySize:
        case ExprType::TableCopy:
        case ExprType::ElemDrop:
        case ExprType::TableInit:
        case ExprType::TableGet:
        case ExprType::TableSet:
        case ExprType::TableGrow:
        case ExprType::TableSize:
        case ExprType::TableFill:
        case ExprType::RefFunc:
        case ExprType::RefNull:
        case ExprType::RefIsNull:
        case ExprType::Rethrow:
        case ExprType::Return:
        case ExprType::Select:
        case ExprType::Throw:
        case ExprType::Unary:
        case ExprType::Ternary:
        case ExprType::SimdLaneOp:
        case ExprType::SimdShuffleOp:
        {
            if (GetRandom() < insert_rate_){
                CreateRandomGarbageExprs(&garbageExprs);
                garbageExprsSize = garbageExprs.size();
                exprs->splice(++iter, garbageExprs);
            }
            break;
        }
        default:
            break;
    }
    return garbageExprsSize;
}

void Obfuscator::ObfuscateExprList(ExprList* exprs){
    intrusive_list<Expr>::iterator it = exprs->begin();
    if(exprs->empty()){
        return;
    }
    while(it != exprs->end()){
        if(MatchObfuscateExprType(it)){
            size_t junk_size = ObfuscateExpr(exprs, it);
            for (size_t i = 0; i < junk_size; ++i){
                ++it;
            }
        }
        it++;
    }
    return;
}

bool Obfuscator::IsValidFuncForJunkCodeInsertion(Func* func){
    ExprList* exprs = &func->exprs;
    FuncDeclaration* decl = &func->decl;
    if (exprs->empty() || decl->GetNumParams() <= 0){
        return false;
    }
    // if (local_types->decls().front().first.GetName() != "i32"){
    //     return false;
    // }
    if (decl->GetParamType(0).GetName() != "i32"){
        return false;
    }
    return true;
}
void Obfuscator::ObfuscateFuncs(Func* func){
    ExprList* exprs = &func->exprs;
    if (IsValidFuncForJunkCodeInsertion(func)){
        ObfuscateExprList(exprs);
    }
}

Result Obfuscator::ObfuscateModule(Module* module){
    for (size_t i=0; i<module->funcs.size(); ++i){
        Func* func = module->funcs[i]; // <- problem
        if (func == nullptr){
            return Result::Error;
        }
        if (!func->exprs.empty()){
            //Check condition and insert garbage
            ObfuscateFuncs(func);
        }
    }
    return Result::Ok;
}

size_t Obfuscator::GetExprSize(const Expr* expr){
    size_t lines = 0;
    switch (expr->type()){
        case ExprType::Block:{
            auto* blk_expr = cast<BlockExpr>(expr);
            lines += GetExpsListSize(blk_expr->block.exprs);
            break;
        }
        case ExprType::If:{
            auto* if_expr = cast<IfExpr>(expr);
            lines += GetExpsListSize(if_expr->true_.exprs);
            lines += GetExpsListSize(if_expr->false_);
            break;
        }
        case ExprType::Try: {
            auto* try_expr = cast<TryExpr>(expr);
            lines += GetExpsListSize(try_expr->block.exprs);
            lines += GetExpsListSize(try_expr->catch_);
            break;
        }
        case ExprType::Loop:{
            auto* loop_expr = cast<LoopExpr>(expr);
            lines += GetExpsListSize(loop_expr->block.exprs);
            break;
        }
        default:
            lines = 1;
            break;
    }
    return lines;
}

size_t Obfuscator::GetExpsListSize(const ExprList& exprs){
    size_t lines = 0;
    for (const Expr& expr : exprs) {
        lines += GetExprSize(&expr);
    }
    return lines;
}

size_t Obfuscator::GetFuncSize(const Func* func){
    return GetExpsListSize(func->exprs);
}

size_t Obfuscator::GetModuleSize(const Module* module){
    size_t lines = 0;
    for (size_t i = 0; i < module->funcs.size(); ++i){
        const Func* func = module->funcs[i + module->num_func_imports]; 
        lines += GetFuncSize(func);
    }
    return lines;
}

void Obfuscator::CreateInsertSet(const Module* module){
    instr_lines_ = GetModuleSize(module);
    insert_lines_ = size_t( floor( double(instr_lines_) * insert_rate_ ) );
    insert_array_ = new (std::nothrow) int [instr_lines_];
    std::srand( std::time(0) ) ;
    for(size_t i = 0; i < insert_lines_; ++i){
        int n = std::rand() % (instr_lines_-1) + 1; 
        insert_array_[n] += 1 ; 
    }
}

Result Obfuscator::Obfuscate(std::unique_ptr<Module>* out_module){
    std::unique_ptr<Module> module;
    if (insert_rate_ < 0){
        return Result::Ok;
    }
    SetRandomGenerator();
    module = std::move(*out_module);
    //CreateInsertSet(module.get());
    ObfuscateModule(module.get());
    *out_module = std::move(module);
    return Result::Ok;
}







} //wabt namespace

