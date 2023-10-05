#ifndef LUA_GENERATION_H
#define LUA_GENERATION_H

#include <format>
#include <map>
#include <stack>

#include "parser_old.h"

#define NEW_LINE "\n"

// #define RENAME_VARIABLES

#pragma region MACRO_FUNCTIONS
#define double_cast(class_1, class_2, var, ptr, field)            \
    do {                                                              \
        if (auto new_ptr = dynamic_cast<class class_1*>(ptr.get())) { \
            var = dynamic_cast<class_2*>(new_ptr->field.get());       \
        }                                                             \
        else {                                                        \
            var = dynamic_cast<class class_2*>(ptr.get());            \
        }                                                             \
    } while (false)
#pragma endregion


class generator
{
private:
    std::map<std::string, std::pair<std::size_t, std::string>> memory;
    std::stack<std::map<std::string, std::string>> scope;
    std::vector<char> variable_chars;
    unsigned int variable_count = 0;

    std::string create_variable(unsigned int id)
    {
        std::string str = "";

        while (id > 0)
        {
            id--;
            str = variable_chars[id % variable_chars.size()] + str;
            id /= variable_chars.size();
        }
        return str;
    }

    std::string get_variable(const std::string& variable)
    {
        auto copy = scope;

        // COUT("ACCESSED");

        while (!copy.empty())
        {
            // COUT("RAN");
            auto& current_scope = copy.top();
            // for (auto n : current_scope) {
            //     COUT("NAME: " << n.first << " -> " << n.second);
            // }
            if (current_scope.find(variable) != current_scope.end())
            {
                return current_scope[variable];
            }
            copy.pop();
        }
        return variable;
    }

    void new_scope()
    {
        scope.push({});
    }

    void end_scope()
    {
        variable_count -= scope.top().size();
        scope.pop();
    }

    void add_variable(base_ptr name, std::size_t depth)
    {
        auto& curr_scope = scope.top();
        auto variable = dynamic_cast<identifier_expr*>(name.get())->value;

        COUT("TOSTR: " << depth << " " << variable);

        // auto it = current_scope.find(variable);
        // if (it != current_scope.end()) {
        //     COUT("ALREADY EXISTS");
        //     current_scope.erase(it);
        // }
        if (auto var = get_variable(variable); var != variable)
        {
            return;
        }

        curr_scope[variable] = create_variable(++variable_count);
    }

    void add_variable(base_ptr_arr ptr_arr, std::size_t depth)
    {
        for (auto name: ptr_arr)
        {
            add_variable(name, depth);
        }
    }

public:
    generator()
    {
        for (int i = 'a'; i < 'z'; i++)
        {
            variable_chars.push_back(i);
        }
        for (int i = 'A'; i < 'Z'; i++)
        {
            variable_chars.push_back(i);
        }
    }

    std::string generate(const std::string& source)
    {
        parser parser;
        auto chunk = parser.parse(source);
        auto generated = tostring(chunk, 0);

        if (!generated.empty() && generated.back() == '\n')
        {
            generated.pop_back();
        }
        return generated;
    }

    std::string concat(base_ptr_arr array, const std::string& separator, std::size_t depth, bool skip_last = true)
    {
        std::string merged_string;
        auto it = array.begin();

        while (it != array.end())
        {
            merged_string += tostring(*it, depth);
            it++;
            if (!(skip_last && it == array.end()))
            {
                merged_string += separator;
            }
        }
        return merged_string;
    }

    std::string space(std::size_t depth)
    {
        return std::string(depth * 4, ' ');
    }


    #ifdef RENAME_VARIABLES
    std::string tostring(const base_ptr& ast, std::size_t depth = 0)
    {
        if (!ast)
        {
            return "";
        }

        switch (ast->kind)
        {
            case base::kind::table_constructor_expr:
            {
                auto expr = dynamic_cast<table_constructor_expr*>(ast.get());

                if (!expr->field_list)
                {
                    return "{}";
                }
                return std::format(
                    "{{{0}{1}{2}{3}}}",
                    NEW_LINE,
                    tostring(expr->field_list, depth + 1),
                    NEW_LINE,
                    space(depth)
                );
            }
            case base::kind::binary_operator_expr:
            {
                auto expr = dynamic_cast<binary_operator_expr*>(ast.get());

                std::string str;

                auto bin_lhs = dynamic_cast<binary_operator_expr*>(expr->lhs.get());
                auto bin_rhs = dynamic_cast<binary_operator_expr*>(expr->rhs.get());

                if (bin_lhs)
                {
                    str += std::format("({0})", tostring(expr->lhs, depth));
                }
                else
                {
                    str += tostring(expr->lhs, depth);
                }

                str += " " + expr->binary_operator + " ";

                if (bin_rhs)
                {
                    str += std::format("({0})", tostring(expr->rhs, depth));
                }
                else
                {
                    str += tostring(expr->rhs, depth);
                }
                return str;
            }
            case base::kind::unary_operator_expr:
            {
                auto expr = dynamic_cast<unary_operator_expr*>(ast.get());
                return expr->unary_operator + (expr->unary_operator == "not" ? " " : "") + tostring(expr->expr, depth);
            }

            case base::kind::numeric_literal_expr:
            {
                auto expr = dynamic_cast<numeric_literal_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::identifier_expr:
            {
                auto expr = dynamic_cast<identifier_expr*>(ast.get());
                return get_variable(expr->value);
            }
            case base::kind::boolean_expr:
            {
                auto expr = dynamic_cast<boolean_expr*>(ast.get());
                return expr->boolean;
            }
            case base::kind::varargs_expr:
            {
                auto expr = dynamic_cast<varargs_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::string_expr:
            {
                auto expr = dynamic_cast<string_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::null_expr:
            {
                auto expr = dynamic_cast<null_expr*>(ast.get());
                return expr->value;
            }

            case base::kind::attrib_expr:
            {
                auto attribute = dynamic_cast<attrib_expr*>(ast.get());
                return std::format("{0}<{1}>", tostring(attribute->name, depth), tostring(attribute->attrib, depth));
            }
            case base::kind::member_expr:
            {
                auto expr = dynamic_cast<member_expr*>(ast.get());
                return std::format("{0}.{1}", tostring(expr->root, depth), tostring(expr->index, depth));
            }
            case base::kind::method_expr:
            {
                auto expr = dynamic_cast<method_expr*>(ast.get());
                return std::format("{0}:{1}", tostring(expr->root, depth), tostring(expr->index, depth));
            }
            case base::kind::index_expr:
            {
                auto expr = dynamic_cast<index_expr*>(ast.get());
                return std::format("{0}[{1}]", tostring(expr->root, depth), tostring(expr->index, depth));
            }

            case base::kind::table_index_value_expr:
            {
                auto expr = dynamic_cast<table_index_value_expr*>(ast.get());
                return std::format(
                    "{0}[{1}] = {2}",
                    space(depth),
                    tostring(expr->index, depth),
                    tostring(expr->value, depth)
                );
            }
            case base::kind::table_name_value_expr:
            {
                auto expr = dynamic_cast<table_name_value_expr*>(ast.get());
                return std::format(
                    "{0}{1} = {2}",
                    space(depth),
                    tostring(expr->index, depth),
                    tostring(expr->value, depth)
                );
            }
            case base::kind::table_value_expr:
            {
                auto expr = dynamic_cast<table_value_expr*>(ast.get());
                return std::format(
                    "{0}{1}",
                    space(depth),
                    tostring(expr->value, depth)
                );
            }

            case base::kind::attnamelist:
            {
                auto list = dynamic_cast<attnamelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::explist:
            {
                auto list = dynamic_cast<explist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::parlist:
            {
                auto list = dynamic_cast<parlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::varlist:
            {
                auto list = dynamic_cast<class varlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::namelist:
            {
                auto list = dynamic_cast<namelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::fieldlist:
            {
                auto list = dynamic_cast<fieldlist*>(ast.get());
                return concat(list->value, ",\n", depth);
            }

            case base::kind::functioncall:
            {
                auto call = dynamic_cast<functioncall*>(ast.get());
                return tostring(call->root, depth) + tostring(call->args, depth);
            }
            case base::kind::functiondef:
            {
                auto func = dynamic_cast<functiondef*>(ast.get());
                return std::format(
                    "function{0}{1}{2}",
                    (!func->name) ? "" : " ",
                    tostring(func->name, depth),
                    tostring(func->body, depth)
                );
            }
            case base::kind::funcname:
            {
                auto name = dynamic_cast<funcname*>(ast.get());
                return tostring(name->value, depth);
            }
            case base::kind::funcbody:
            {
                auto func = dynamic_cast<funcbody*>(ast.get());
                return std::format(
                    "({0}){1}{2}{3}end",
                    tostring(func->parameters, depth),
                    NEW_LINE,
                    tostring(func->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::label:
            {
                auto label = dynamic_cast<class label*>(ast.get());
                return std::format("::{0}::", tostring(label->value, depth));
            }
            case base::kind::args:
            {
                auto args = dynamic_cast<class args*>(ast.get());
                return std::format(
                    "({0})",
                    tostring(args->value, depth)
                );
            }
            case base::kind::block:
            {
                auto block = dynamic_cast<class block*>(ast.get());
                auto statements = block->statements;
                std::string merged_string;

                for (auto it = statements.begin(); it != statements.end(); it++)
                {
                    std::string stringified = tostring(*it, depth);

                    if (stringified == ";" && !merged_string.empty() && merged_string.back() == '\n')
                    {
                        merged_string.pop_back();
                    }
                    else
                    {
                        merged_string += space(depth);
                    }
                    merged_string += stringified;
                    merged_string += "\n";
                }
                return merged_string;
            }
            case base::kind::semicolon:
            {
                return ";";
            }

            case base::kind::generic_for_stat:
            {
                auto stat = dynamic_cast<generic_for_stat*>(ast.get());
                return std::format(
                    "for {0} in {1} do\n{2}{3}end",
                    tostring(stat->name_list, depth),
                    tostring(stat->expr_list, depth),
                    tostring(stat->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::numeric_for_stat:
            {
                auto stat = dynamic_cast<numeric_for_stat*>(ast.get());

                if (stat->step)
                {
                    return std::format(
                        "for {0} = {1}, {2}, {3} do{4}{5}{6}end",
                        tostring(stat->name, 0),
                        tostring(stat->init, depth),
                        tostring(stat->goal, depth),
                        tostring(stat->step, depth),
                        NEW_LINE,
                        tostring(stat->block, depth + 1),
                        space(depth)
                    );
                }
                return std::format(
                    "for {0} = {1}, {2} do{3}{4}{5}end",
                    tostring(stat->name, 0),
                    tostring(stat->init, depth),
                    tostring(stat->goal, depth),
                    NEW_LINE,
                    tostring(stat->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::return_stat:
            {
                auto stat = dynamic_cast<return_stat*>(ast.get());
                return std::format(
                    "return {0}",
                    tostring(stat->value, depth)
                );
            }
            case base::kind::repeat_stat:
            {
                auto stat = dynamic_cast<repeat_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return std::format(
                    "repeat{0}{1}{2}until {3}",
                    NEW_LINE,
                    tostring(cond->block, depth + 1),
                    space(depth),
                    tostring(cond->condition, depth)
                );
            }
            case base::kind::while_stat:
            {
                auto stat = dynamic_cast<while_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return std::format(
                    "while {0} do\n{1}{2}end",
                    tostring(cond->condition, depth),
                    tostring(cond->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::if_stat:
            {
                auto stat = dynamic_cast<if_stat*>(ast.get());
                std::string str;

                for (int i = 0; i < stat->statements.size(); i++)
                {
                    auto cond = dynamic_cast<block_conditional*>(stat->statements[i].get());

                    if (i == 0) {
                        str += std::format(
                            "if {0} then{1}{2}",
                            tostring(cond->condition, depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                    else if (cond->condition)
                    {
                        str += std::format(
                            "{0}elseif {1} then{2}{3}",
                            space(depth),
                            tostring(cond->condition, depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                    else
                    {
                        str += std::format(
                            "{0}else{1}{2}",
                            space(depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                }

                str += std::format("{0}end", space(depth));
                return str;
            }
            case base::kind::do_stat:
            {
                auto stat = dynamic_cast<do_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->block.get());
                return std::format(
                    "do{0}{1}{2}end",
                    NEW_LINE,
                    tostring(cond->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::goto_stat:
            {
                auto stat = dynamic_cast<goto_stat*>(ast.get());
                return std::format(
                    "goto {0}",
                    tostring(stat->label, depth)
                );
            }
            case base::kind::break_stat:
            {
                return "break";
            }
            case base::kind::local_stat:
            {
                auto stat = dynamic_cast<local_stat*>(ast.get());
                return std::format(
                    "local {0}",
                    tostring(stat->declaration, depth)
                );
            }
            case base::kind::assignment_stat:
            {
                auto stat = dynamic_cast<assignment_stat*>(ast.get());
                return std::format(
                    "{0} = {1}",
                    tostring(stat->lhs, depth),
                    tostring(stat->rhs, depth)
                );
            }

            default:
            {
                return "";
            }
        }
    }
    #else
    std::string tostring(const base_ptr& ast, std::size_t depth = 0)
    {
        if (!ast)
        {
            return "";
        }

        switch (ast->kind)
        {
            case base::kind::table_constructor_expr:
            {
                auto expr = dynamic_cast<table_constructor_expr*>(ast.get());

                if (!expr->field_list)
                {
                    return "{}";
                }
                return std::format(
                    "{{{0}{1}{2}{3}}}",
                    NEW_LINE,
                    tostring(expr->field_list, depth + 1),
                    NEW_LINE,
                    space(depth)
                );
            }
            case base::kind::binary_operator_expr:
            {
                auto expr = dynamic_cast<binary_operator_expr*>(ast.get());

                std::string str;

                auto bin_lhs = dynamic_cast<binary_operator_expr*>(expr->lhs.get());
                auto bin_rhs = dynamic_cast<binary_operator_expr*>(expr->rhs.get());

                if (bin_lhs)
                {
                    str += std::format("({0})", tostring(expr->lhs, depth));
                }
                else
                {
                    str += tostring(expr->lhs, depth);
                }

                str += " " + expr->binary_operator + " ";

                if (bin_rhs)
                {
                    str += std::format("({0})", tostring(expr->rhs, depth));
                }
                else
                {
                    str += tostring(expr->rhs, depth);
                }
                return str;
            }
            case base::kind::unary_operator_expr:
            {
                auto expr = dynamic_cast<unary_operator_expr*>(ast.get());
                return expr->unary_operator + (expr->unary_operator == "not" ? " " : "") + tostring(expr->expr, depth);
            }

            case base::kind::numeric_literal_expr:
            {
                auto expr = dynamic_cast<numeric_literal_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::identifier_expr:
            {
                auto expr = dynamic_cast<identifier_expr*>(ast.get());
                return get_variable(expr->value);
            }
            case base::kind::boolean_expr:
            {
                auto expr = dynamic_cast<boolean_expr*>(ast.get());
                return expr->boolean;
            }
            case base::kind::varargs_expr:
            {
                auto expr = dynamic_cast<varargs_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::string_expr:
            {
                auto expr = dynamic_cast<string_expr*>(ast.get());
                return expr->value;
            }
            case base::kind::null_expr:
            {
                auto expr = dynamic_cast<null_expr*>(ast.get());
                return expr->value;
            }

            case base::kind::attrib_expr:
            {
                auto attribute = dynamic_cast<attrib_expr*>(ast.get());
                return std::format("{0}<{1}>", tostring(attribute->name, depth), tostring(attribute->attrib, depth));
            }
            case base::kind::member_expr:
            {
                auto expr = dynamic_cast<member_expr*>(ast.get());
                return std::format("{0}.{1}", tostring(expr->root, depth), tostring(expr->index, depth));
            }
            case base::kind::method_expr:
            {
                auto expr = dynamic_cast<method_expr*>(ast.get());
                return std::format("{0}:{1}", tostring(expr->root, depth), tostring(expr->index, depth));
            }
            case base::kind::index_expr:
            {
                auto expr = dynamic_cast<index_expr*>(ast.get());
                return std::format("{0}[{1}]", tostring(expr->root, depth), tostring(expr->index, depth));
            }

            case base::kind::table_index_value_expr:
            {
                auto expr = dynamic_cast<table_index_value_expr*>(ast.get());
                return std::format(
                    "{0}[{1}] = {2}",
                    space(depth),
                    tostring(expr->index, depth),
                    tostring(expr->value, depth)
                );
            }
            case base::kind::table_name_value_expr:
            {
                auto expr = dynamic_cast<table_name_value_expr*>(ast.get());
                return std::format(
                    "{0}{1} = {2}",
                    space(depth),
                    tostring(expr->index, depth),
                    tostring(expr->value, depth)
                );
            }
            case base::kind::table_value_expr:
            {
                auto expr = dynamic_cast<table_value_expr*>(ast.get());
                return std::format(
                    "{0}{1}",
                    space(depth),
                    tostring(expr->value, depth)
                );
            }

            case base::kind::attnamelist:
            {
                auto list = dynamic_cast<attnamelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::explist:
            {
                auto list = dynamic_cast<explist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::parlist:
            {
                auto list = dynamic_cast<parlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::varlist:
            {
                auto list = dynamic_cast<class varlist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::namelist:
            {
                auto list = dynamic_cast<namelist*>(ast.get());
                return concat(list->value, ", ", depth);
            }
            case base::kind::fieldlist:
            {
                auto list = dynamic_cast<fieldlist*>(ast.get());
                return concat(list->value, ",\n", depth);
            }

            case base::kind::functioncall:
            {
                auto call = dynamic_cast<functioncall*>(ast.get());
                return tostring(call->root, depth) + tostring(call->args, depth);
            }
            case base::kind::functiondef:
            {
                auto func = dynamic_cast<functiondef*>(ast.get());
                return std::format(
                    "function{0}{1}{2}",
                    (!func->name) ? "" : " ",
                    tostring(func->name, depth),
                    tostring(func->body, depth)
                );
            }
            case base::kind::funcname:
            {
                auto name = dynamic_cast<funcname*>(ast.get());
                return tostring(name->value, depth);
            }
            case base::kind::funcbody:
            {
                auto func = dynamic_cast<funcbody*>(ast.get());
                return std::format(
                    "({0}){1}{2}{3}end",
                    tostring(func->parameters, depth),
                    NEW_LINE,
                    tostring(func->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::label:
            {
                auto label = dynamic_cast<class label*>(ast.get());
                return std::format("::{0}::", tostring(label->value, depth));
            }
            case base::kind::args:
            {
                auto args = dynamic_cast<class args*>(ast.get());
                return std::format(
                    "({0})",
                    tostring(args->value, depth)
                );
            }
            case base::kind::block:
            {
                auto block = dynamic_cast<class block*>(ast.get());
                auto statements = block->statements;
                std::string merged_string;

                for (auto it = statements.begin(); it != statements.end(); it++)
                {
                    std::string stringified = tostring(*it, depth);

                    if (stringified == ";" && !merged_string.empty() && merged_string.back() == '\n')
                    {
                        merged_string.pop_back();
                    }
                    else
                    {
                        merged_string += space(depth);
                    }
                    merged_string += stringified;
                    merged_string += "\n";
                }
                return merged_string;
            }
            case base::kind::semicolon:
            {
                return ";";
            }

            case base::kind::generic_for_stat:
            {
                auto stat = dynamic_cast<generic_for_stat*>(ast.get());
                return std::format(
                    "for {0} in {1} do\n{2}{3}end",
                    tostring(stat->name_list, depth),
                    tostring(stat->expr_list, depth),
                    tostring(stat->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::numeric_for_stat:
            {
                auto stat = dynamic_cast<numeric_for_stat*>(ast.get());

                if (stat->step)
                {
                    return std::format(
                        "for {0} = {1}, {2}, {3} do{4}{5}{6}end",
                        tostring(stat->name, 0),
                        tostring(stat->init, depth),
                        tostring(stat->goal, depth),
                        tostring(stat->step, depth),
                        NEW_LINE,
                        tostring(stat->block, depth + 1),
                        space(depth)
                    );
                }
                return std::format(
                    "for {0} = {1}, {2} do{3}{4}{5}end",
                    tostring(stat->name, 0),
                    tostring(stat->init, depth),
                    tostring(stat->goal, depth),
                    NEW_LINE,
                    tostring(stat->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::return_stat:
            {
                auto stat = dynamic_cast<return_stat*>(ast.get());
                return std::format(
                    "return {0}",
                    tostring(stat->value, depth)
                );
            }
            case base::kind::repeat_stat:
            {
                auto stat = dynamic_cast<repeat_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return std::format(
                    "repeat{0}{1}{2}until {3}",
                    NEW_LINE,
                    tostring(cond->block, depth + 1),
                    space(depth),
                    tostring(cond->condition, depth)
                );
            }
            case base::kind::while_stat:
            {
                auto stat = dynamic_cast<while_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->statement.get());
                return std::format(
                    "while {0} do\n{1}{2}end",
                    tostring(cond->condition, depth),
                    tostring(cond->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::if_stat:
            {
                auto stat = dynamic_cast<if_stat*>(ast.get());
                std::string str;

                COUT((int) ast->parent->kind);

                for (int i = 0; i < stat->statements.size(); i++)
                {
                    auto cond = dynamic_cast<block_conditional*>(stat->statements[i].get());

                    if (i == 0) {
                        str += std::format(
                            "if {0} then{1}{2}",
                            tostring(cond->condition, depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                    else if (cond->condition)
                    {
                        str += std::format(
                            "{0}elseif {1} then{2}{3}",
                            space(depth),
                            tostring(cond->condition, depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                    else
                    {
                        str += std::format(
                            "{0}else{1}{2}",
                            space(depth),
                            NEW_LINE,
                            tostring(cond->block, depth + 1)
                        );
                    }
                }

                str += std::format("{0}end", space(depth));
                return str;
            }
            case base::kind::do_stat:
            {
                auto stat = dynamic_cast<do_stat*>(ast.get());
                auto cond = dynamic_cast<block_conditional*>(stat->block.get());
                return std::format(
                    "do{0}{1}{2}end",
                    NEW_LINE,
                    tostring(cond->block, depth + 1),
                    space(depth)
                );
            }
            case base::kind::goto_stat:
            {
                auto stat = dynamic_cast<goto_stat*>(ast.get());
                return std::format(
                    "goto {0}",
                    tostring(stat->label, depth)
                );
            }
            case base::kind::break_stat:
            {
                return "break";
            }
            case base::kind::local_stat:
            {
                auto stat = dynamic_cast<local_stat*>(ast.get());
                return std::format(
                    "local {0}",
                    tostring(stat->declaration, depth)
                );
            }
            case base::kind::assignment_stat:
            {
                auto stat = dynamic_cast<assignment_stat*>(ast.get());
                return std::format(
                    "{0} = {1}",
                    tostring(stat->lhs, depth),
                    tostring(stat->rhs, depth)
                );
            }

            default:
            {
                return "";
            }
        }
    }
    #endif
};


#endif