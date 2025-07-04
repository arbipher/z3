/*++
Copyright (c) 2006 Microsoft Corporation

Module Name:

    theory_array.h

Abstract:

    <abstract>

Author:

    Leonardo de Moura (leonardo) 2008-06-01.

Revision History:

--*/
#pragma once

#include "smt/theory_array_base.h"
#include "params/theory_array_params.h"
#include "util/union_find.h"

namespace smt {

    struct theory_array_stats {
        unsigned   m_num_axiom1, m_num_axiom2a, m_num_axiom2b, m_num_extensionality, m_num_eq_splits;
        unsigned   m_num_map_axiom, m_num_default_map_axiom;
        unsigned   m_num_select_const_axiom, m_num_default_store_axiom, m_num_default_const_axiom, m_num_default_as_array_axiom;
        unsigned   m_num_select_as_array_axiom, m_num_default_lambda_axiom;
        void reset() { memset(this, 0, sizeof(theory_array_stats)); }
        theory_array_stats() { reset(); }
    };

    class theory_array : public theory_array_base {
        unsigned laziness() const { return ctx.relevancy()?m_params.m_array_laziness:0; }
    protected:
        typedef union_find<theory_array>  th_union_find;

        struct var_data {
            ptr_vector<enode>  m_stores;
            ptr_vector<enode>  m_parent_selects;
            ptr_vector<enode>  m_parent_stores;
            bool               m_prop_upward = false;
            bool               m_is_array = false;
            bool               m_is_select = false;
        };
        ptr_vector<var_data>            m_var_data;
        theory_array_params&            m_params;
        theory_array_stats              m_stats;
        th_union_find                   m_find;
        trail_stack                     m_trail_stack;
        unsigned                        m_final_check_idx = 0;

        theory_var mk_var(enode * n) override;
        bool internalize_atom(app * atom, bool gate_ctx) override;
        bool internalize_term(app * term) override;
        void apply_sort_cnstr(enode * n, sort * s) override;
        void new_eq_eh(theory_var v1, theory_var v2) override;
        void new_diseq_eh(theory_var v1, theory_var v2) override;
        void relevant_eh(app * n) override;
        void push_scope_eh() override;
        void pop_scope_eh(unsigned num_scopes) override;
        final_check_status final_check_eh() override;
        void reset_eh() override;
        void init_search_eh() override;

        void set_prop_upward(theory_var v) override;
        virtual void set_prop_upward(enode* n);
        virtual void set_prop_upward(theory_var v, var_data* d);

        virtual unsigned get_lambda_equiv_size(theory_var v, var_data* d);

        theory_var find(theory_var v) const { return m_find.find(v); }
        bool is_root(theory_var v) const { return m_find.is_root(v); }

        virtual void add_parent_select(theory_var v, enode * s);
        void add_parent_store(theory_var v, enode * s);
        void add_store(theory_var v, enode * s);

        bool internalize_term_core(app * term);

        void instantiate_axiom2a(enode * select, enode * store);
        bool instantiate_axiom2b(enode * select, enode * store);
        void instantiate_axiom1(enode * store);
        void instantiate_extensionality(enode * a1, enode * a2);
        void instantiate_congruent(enode * a1, enode * a2);
        bool instantiate_axiom2b_for(theory_var v);
        
        virtual final_check_status assert_delayed_axioms();
        final_check_status mk_interface_eqs_at_final_check();

        static void display_ids(std::ostream & out, unsigned n, enode * const * v);
    public:
        theory_array(context& ctx);
        ~theory_array() override;

        theory * mk_fresh(context * new_ctx) override { return alloc(theory_array, *new_ctx); }

        char const * get_name() const override { return "array"; }

        virtual void display_var(std::ostream & out, theory_var v) const;
        void display(std::ostream & out) const override;
        void collect_statistics(::statistics & st) const override;
        trail_stack & get_trail_stack() { return m_trail_stack; }
        virtual void merge_eh(theory_var v1, theory_var v2, theory_var, theory_var);
        static void after_merge_eh(theory_var r1, theory_var r2, theory_var v1, theory_var v2) {}
        void unmerge_eh(theory_var v1, theory_var v2);

        ptr_vector<enode> const& parent_selects(enode* n) { return m_var_data[find(n->get_root()->get_th_var(get_id()))]->m_parent_selects; }
    };

};


