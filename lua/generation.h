#ifndef LUA_GENERATION_H
#define LUA_GENERATION_H

#include "parser.h"



class generator {
public:

    std::string generate(const std::string& source) {
        parser parser;
        auto chunk = parser.parse(source);
        auto generated = tostring(chunk, 0);

        if (!generated.empty() && generated.back() == '\n') {
            generated.pop_back();
        }
        return generated;
    }

    std::string concat(base_ptr_arr array, const std::string& separator, std::size_t depth, bool skip_last = true) {
        std::string merged_string;
        auto it = array.begin();

        while (it != array.end()) {
            merged_string += tostring(*it, depth);
            it++;
            if (!(skip_last && it == array.end())) {
                merged_string += separator;
            }
        }
        return merged_string;
    }

    std::string space(std::size_t depth) {
        return std::string(depth * 4, ' ');
    }

    std::string tostring(const base_ptr& ast, std::size_t depth) {
        if (!ast) {
            return "";
        }

        switch (ast->kind) {
            case base::kind::binary_operator_expr: {
                auto expr = dynamic_cast<binary_operator_expr*>(ast.get());

                std::string str;

                if (dynamic_cast<binary_operator_expr*>(expr->lhs.get())) {
                    str += "(" + tostring(expr->lhs, depth) + ")";
                }
                else {
                    str += tostring(expr->lhs, depth);
                }

                str += " " + expr->binary_operator + " ";

                if (dynamic_cast<binary_operator_expr*>(expr->rhs.get())) {
                    str += "(" + tostring(expr->rhs, depth) + ")";
                }
                else {
                    str += tostring(expr->rhs, depth);
                }

                return str;
            }
            case base::kind::unary_operator_expr: {
                auto expr = dynamic_cast<unary_operator_expr*>(ast.get());
                return expr->unary_operator + (expr->unary_operator == "not" ? " " : "") + tostring(expr->expr, depth);
            }
            case base::kind::table_constructor_expr: {
                auto expr = dynamic_cast<table_constructor_expr*>(ast.get());

                if (!expr->field_list) {
                    return "{}";
                }
                return "{\n" + tostring(expr->field_list, depth + 1) + "\n" + space(depth) + "}";
            }

            case base::kind::boolean_expr: {
                auto expr = dynamic_cast<boolean_expr*>(ast.get());
                return expr->boolean;
            }
            case base::kind::null_expr: {
                auto expr = dynamic_cast<null_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::varargs_expr: {
                auto expr = dynamic_cast<varargs_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::string_expr: {
                auto expr = dynamic_cast<string_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::numeric_literal_expr: {
                auto expr = dynamic_cast<numeric_literal_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::identifier_expr: {
                auto expr = dynamic_cast<identifier_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::member_expr: {
                auto expr = dynamic_cast<member_expr*>(ast.get());
                return tostring(expr->root, depth) + "." + tostring(expr->index, depth);
            }
            case base::kind::method_expr: {
                auto expr = dynamic_cast<method_expr*>(ast.get());
                return tostring(expr->root, depth) + ":" + tostring(expr->index, depth);
            }
            case base::kind::index_expr: {
                auto expr = dynamic_cast<index_expr*>(ast.get());
                return tostring(expr->root, depth) + "[" + tostring(expr->index, depth) + "]";
            }
            case base::kind::attrib_expr: {
                auto attribute = dynamic_cast<attrib_expr*>(ast.get());
                return tostring(attribute->name, depth) + "<" + tostring(attribute->attrib, depth) + ">";
            }
            case base::kind::table_index_value_expr: {
                auto expr = dynamic_cast<table_index_value_expr*>(ast.get());
                return space(depth) + "[" + tostring(expr->index, depth) + "] = " + tostring(expr->value, depth);
            }
            case base::kind::table_name_value_expr: {
                auto expr = dynamic_cast<table_name_value_expr*>(ast.get());
                return space(depth) + tostring(expr->index, depth) + " = " + tostring(expr->value, depth);
            }
            case base::kind::table_value_expr: {
                auto expr = dynamic_cast<table_value_expr*>(ast.get());
                return space(depth) + tostring(expr->value, depth);
            }
            case base::kind::attnamelist: {
                auto list = dynamic_cast<attnamelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::retstat: {
                auto stat = dynamic_cast<retstat*>(ast.get());
                return "return " + tostring(stat->value, depth);
            }
            case base::kind::explist: {
                auto list = dynamic_cast<explist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::parlist: {
                auto list = dynamic_cast<parlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::varlist: {
                auto list = dynamic_cast<class varlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::namelist: {
                auto list = dynamic_cast<namelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::fieldlist: {
                auto list = dynamic_cast<fieldlist*>(ast.get());
                return concat(list->value, ",\n", depth);
            }
            case base::kind::label: {
                auto label = dynamic_cast<class label*>(ast.get());
                return "::" + tostring(label->value, depth) + "::";
            }
            case base::kind::args: {
                auto args = dynamic_cast<class args*>(ast.get());
                return "(" + tostring(args->value, depth) + ")";
            }
            case base::kind::funcname: {
                auto name = dynamic_cast<funcname*>(ast.get());
                return tostring(name->value, depth);
            }
            case base::kind::functioncall: {
                auto call = dynamic_cast<functioncall*>(ast.get());
                return tostring(call->root, depth) + tostring(call->args, depth);
            }

            case base::kind::funcbody: {
                auto func = dynamic_cast<funcbody*>(ast.get());
                return "(" + tostring(func->parameters, depth) + ")\n" + tostring(func->block, depth + 1) + space(depth) + "end";
            }
            case base::kind::semicolon: {
                return ";";
            }
            case base::kind::block: {
                auto block = dynamic_cast<class block*>(ast.get());

                std::string merged_string;
                auto it = block->statements.begin();

                while (it != block->statements.end()) {
                    std::string stringified = tostring(*it++, depth);

                    if (stringified == ";" && !merged_string.empty() && merged_string.back() == '\n') {
                        merged_string.pop_back();
                    }
                    else {
                        merged_string += space(depth);
                    }
                    merged_string += stringified;
                    merged_string += "\n";
                }
                return merged_string;
            }
            case base::kind::functiondef: {
                auto func = dynamic_cast<functiondef*>(ast.get());
                return "function " + tostring(func->name, depth) + tostring(func->body, depth);
            }
            case base::kind::functiondef_anon: {
                auto func = dynamic_cast<functiondef_anon*>(ast.get());
                return "function" + tostring(func->body, depth);
            }
            case base::kind::assignment_stat: {
                auto stat = dynamic_cast<assignment_stat*>(ast.get());
                return tostring(stat->lhs, depth) + " = " + tostring(stat->rhs, depth);
            }
            case base::kind::break_stat: {
                return "break";
            }
            case base::kind::goto_stat: {
                auto stat = dynamic_cast<goto_stat*>(ast.get());
                return "goto " + tostring(stat->label, depth);
            }
            case base::kind::do_stat: {
                auto stat = dynamic_cast<do_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->block.get());
                return "do\n" + tostring(cond->block, depth + 1) + space(depth) + "end";
            }
            case base::kind::while_stat: {
                auto stat = dynamic_cast<while_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return "while " + tostring(cond->condition, depth) + " do\n" + tostring(cond->block, depth + 1) + space(depth) + "end";
            }
            case base::kind::repeat_stat: {
                auto stat = dynamic_cast<repeat_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return "repeat\n" + tostring(cond->block, depth + 1) + space(depth) + "until " + tostring(cond->condition, depth);
            }
            case base::kind::if_stat: {
                auto stat = dynamic_cast<if_stat*>(ast.get());
                std::string str;

                for (int i = 0; i < stat->statements.size(); i++) {
                    auto cond = dynamic_cast<block_conditional*>(stat->statements[i].get());

                    if (cond->condition) {
                        str += (i == 0 ? "if " : space(depth) + "elseif ") + tostring(cond->condition, depth) + " then\n";
                        str += tostring(cond->block, depth + 1);
                    }
                    else {
                        str += space(depth) + "else\n";
                        str += tostring(cond->block, depth + 1);
                    }
                }

                str += space(depth) + "end";
                return str;
            }
            case base::kind::generic_for_stat: {
                auto stat = dynamic_cast<generic_for_stat*>(ast.get());
                return "for " + tostring(stat->name_list, depth) + " in " + tostring(stat->expr_list, depth) + " do\n" + tostring(stat->block, depth + 1) + space(depth) + "end";
            }
            case base::kind::numeric_for_stat: {
                auto stat = dynamic_cast<numeric_for_stat*>(ast.get());
                std::string str;
                str += "for " + tostring(stat->name, 0) + " = " + tostring(stat->init, 0);
                str += ", " + tostring(stat->goal, depth);

                if (stat->step) {
                    str += ", " + tostring(stat->step, depth);
                }

                str += " do\n" + tostring(stat->block, depth + 1) + "end";
                return str;
            }
            case base::kind::local_stat: {
                auto stat = dynamic_cast<local_stat*>(ast.get());
                return "local " + tostring(stat->declaration, depth);
            }

            case base::kind::conditional_expr:
            case base::kind::block_conditional:
            case base::kind::prefixexp: {
                return "???";
            }
        }
        return "";
    }
};





#endif