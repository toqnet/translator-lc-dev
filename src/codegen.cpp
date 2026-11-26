#include "codegen.hpp"



void Codegen::generate(const Program& prog)
{
    types.clear();
    for (const auto& d : prog.decls)
        for (const auto& n : d->vename)
            types[n] = d->type;

    out << "#include <stdio.h>\n\nint main(void) {\n";
    
    for (const auto& d : prog.decls)
    {
        const char* ty = (d->type == TypeKind::Float) ? "double" : "long long";
        out << ty << " ";
        for (size_t i = 0; i < d->vename.size(); i++)
        {
            if (i > 0) out << ", ";
            out << d->vename[i] << " = 0"; 
        }
        out << ";\n";
    } 
    for (const auto& s : prog.body) genStmt(*s);
    out << "return 0;\n}\n";
} 

void Codegen::genStmt(const Stmt& s)
{
    if (auto a = dynamic_cast<const AssingStmt*>(&s))
    {
        out << a->target << " = ";
        genExpr(*a->value);
        out << ";\n";
    }

    else if (auto in = dynamic_cast<const InputStmt*>(&s))
    {
        out << "if (scanf(\"";
        for (size_t i = 0; i < in->vars.size(); i++)
            out << (i ? " " : "") << (types.at(in->vars[i]) == TypeKind::Float ? "%lf": "%lld");
        out << "\"";
        for (const auto& v : in->vars)
            out << ", &" << v;
        out << ") != " << in->vars.size() <<") return 1;\n";
    }  
    else if (auto o = dynamic_cast<const OutputStmt*>(&s))
    {
        out << "printf(\"";
        for (size_t i = 0; i < o->args.size(); i++)
        {
            if (i > 0) out << " ";
            out << (typeOf(*o->args[i]) == TypeKind::Float ? "%f" : "%lld");
        }
        out << "\\n\"";
        for (const auto& e : o->args)
        {
            out << ", ";
            genExpr(*e);
        }
        out << ");\n";
    }
    else if (auto f = dynamic_cast<const ForStmt*>(&s))
    {
        out << "for (" << f->count_name << " = ";
        genExpr(*f->from);
        out << "; " << f->count_name << " <= ";
        genExpr(*f->to);
        out << "; " << f->count_name << " += ";
        if (f->step) genExpr(*f->step); else out << "1";
        out << ") ";
        genStmt(*f->body);
    }
    else if (auto b = dynamic_cast<const BlockStmt*>(&s))
    {
        out << "{\n";
        for (const auto& child : b->body)
            genStmt(*child);
        out << "}\n";
    }
}

void Codegen::genExpr(const Expr& e)
{
    if (auto lit =  dynamic_cast<const LiteralExpr*>(&e))
    {
        if (lit->type == TypeKind::Int)
        {
            out << lit->value.i << "LL";
        }
        else
        {
            double v = lit->value.f;
            out << v;
            if (v == (long long)v) out << ".0";
        }
    }
    else if (auto v = dynamic_cast<const VarExpr*>(&e))
    {
        out << v->name;
    }
    else if (auto b = dynamic_cast<const BinaryExpr*>(&e))
    {
        out << "(";
        genExpr(*b->left);
        switch (b->op)
        {
        case OpKind::Add: out << " + "; break;
        case OpKind::Sub: out << " - "; break;
        case OpKind::Mul: out << " * "; break;
        case OpKind::Div: out << " / "; break;
        default: break;
        }
        genExpr(*b->right);
        out << ")";
    }
    else if (auto u = dynamic_cast<const UnaryExpr*>(&e))
    {
        out << "(-";
        genExpr(*u->operand);
        out << ")";
    }
}

TypeKind Codegen::typeOf(const Expr& e) const
{
    if (auto lit = dynamic_cast<const LiteralExpr*>(&e)) return lit->type;
    if (auto v = dynamic_cast<const VarExpr*>(&e))
    {
        auto it = types.find(v->name);
        return it != types.end() ? it->second : TypeKind::Int;
    }
    if (auto b = dynamic_cast<const BinaryExpr*>(&e))
        return (typeOf(*b->left) == TypeKind::Float || typeOf(*b->right) == TypeKind::Float) ? TypeKind::Float : TypeKind::Int;
    if (auto u = dynamic_cast<const UnaryExpr*>(&e)) return typeOf(*u->operand);
    return TypeKind::Int;
}