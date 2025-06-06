/*++
Copyright (c) 2006 Microsoft Corporation

Module Name:

    bv_decl_plugin.h

Abstract:

    <abstract>

Author:

    Leonardo de Moura (leonardo) 2008-01-09.

Revision History:

--*/
#pragma once

#include "ast/ast.h"

enum bv_sort_kind {
    BV_SORT
};

enum bv_op_kind {
    OP_BV_NUM,
    OP_BIT1,
    OP_BIT0,
    OP_BNEG,
    OP_BADD,
    OP_BSUB,
    OP_BMUL,

    OP_BSDIV,
    OP_BUDIV,
    OP_BSREM,
    OP_BUREM,
    OP_BSMOD,

    // special functions to record the division by 0 cases
    // these are internal functions
    OP_BSDIV0,
    OP_BUDIV0,
    OP_BSREM0,
    OP_BUREM0,
    OP_BSMOD0,

    // special functions where division by 0 has a fixed interpretation.
    OP_BSDIV_I,
    OP_BUDIV_I,
    OP_BSREM_I,
    OP_BUREM_I,
    OP_BSMOD_I,

    OP_ULEQ,
    OP_SLEQ,
    OP_UGEQ,
    OP_SGEQ,
    OP_ULT,
    OP_SLT,
    OP_UGT,
    OP_SGT,

    OP_BAND,
    OP_BOR,
    OP_BNOT,
    OP_BXOR,
    OP_BNAND,
    OP_BNOR,
    OP_BXNOR,

    OP_CONCAT,
    OP_SIGN_EXT,
    OP_ZERO_EXT,
    OP_EXTRACT,
    OP_REPEAT,

    OP_BREDOR,
    OP_BREDAND,
    OP_BCOMP,

    OP_BSHL,
    OP_BLSHR,
    OP_BASHR,
    OP_ROTATE_LEFT,
    OP_ROTATE_RIGHT,
    OP_EXT_ROTATE_LEFT,
    OP_EXT_ROTATE_RIGHT,

    OP_BUMUL_NO_OVFL, // no unsigned multiplication overflow predicate
    OP_BSMUL_NO_OVFL, // no signed multiplication overflow predicate
    OP_BSMUL_NO_UDFL, // no signed multiplication underflow predicate

    OP_BUMUL_OVFL, // unsigned multiplication overflow predicate (negation of OP_BUMUL_NO_OVFL)
    OP_BSMUL_OVFL, // signed multiplication over/underflow predicate

    OP_BSDIV_OVFL, // signed division overflow predicate

    OP_BNEG_OVFL, // negation overflow predicate

    OP_BUADD_OVFL, // unsigned addition overflow predicate
    OP_BSADD_OVFL, // signed addition overflow predicate

    OP_BUSUB_OVFL, // unsigned subtraction overflow predicate
    OP_BSSUB_OVFL, // signed subtraction overflow predicate

    OP_BIT2BOOL, // predicate
    OP_MKBV,     // bools to bv
    OP_INT2BV,
    OP_UBV2INT,
    OP_SBV2INT,

    OP_CARRY,
    OP_XOR3,

    LAST_BV_OP
};

// Assume k is a "div" operator. It returns the div0 uninterpreted function that
// models the value of "div" it is underspecified (i.e., when the denominator is zero).
inline bv_op_kind get_div0_op(bv_op_kind k) {
    switch (k) {
    case OP_BSDIV: return OP_BSDIV0;
    case OP_BUDIV: return OP_BUDIV0;
    case OP_BSREM: return OP_BSREM0;
    case OP_BUREM: return OP_BUREM0;
    case OP_BSMOD: return OP_BSMOD0;
    default: UNREACHABLE(); return LAST_BV_OP;
    }
}

// Assume decl is the declaration of a "div" operator. It returns the div0 declaration that
// models the value of "div" it is underspecified (i.e., when the denominator is zero).
inline func_decl * get_div0_decl(ast_manager & m, func_decl * decl) {
    return m.mk_func_decl(decl->get_family_id(), get_div0_op(static_cast<bv_op_kind>(decl->get_decl_kind())),
                          0, nullptr, 1, decl->get_domain());
}

class bv_decl_plugin : public decl_plugin {
    friend class bv_util;
protected:
    symbol m_bv_sym;
    symbol m_concat_sym;
    symbol m_sign_extend_sym;
    symbol m_zero_extend_sym;
    symbol m_extract_sym;
    symbol m_rotate_left_sym;
    symbol m_rotate_right_sym;
    symbol m_repeat_sym;
    symbol m_bit2bool_sym;
    symbol m_mkbv_sym;

    func_decl * m_bit0;
    func_decl * m_bit1;
    func_decl * m_carry;
    func_decl * m_xor3;

    ptr_vector<sort>   m_bv_sorts;
    sort *             m_int_sort;

    ptr_vector<func_decl>  m_bv_neg;
    ptr_vector<func_decl>  m_bv_add;
    ptr_vector<func_decl>  m_bv_sub;
    ptr_vector<func_decl>  m_bv_mul;
    ptr_vector<func_decl>  m_bv_sdiv;
    ptr_vector<func_decl>  m_bv_udiv;
    ptr_vector<func_decl>  m_bv_srem;
    ptr_vector<func_decl>  m_bv_urem;
    ptr_vector<func_decl>  m_bv_smod;

    ptr_vector<func_decl>  m_bv_sdiv0;
    ptr_vector<func_decl>  m_bv_udiv0;
    ptr_vector<func_decl>  m_bv_srem0;
    ptr_vector<func_decl>  m_bv_urem0;
    ptr_vector<func_decl>  m_bv_smod0;

    ptr_vector<func_decl>  m_bv_sdiv_i;
    ptr_vector<func_decl>  m_bv_udiv_i;
    ptr_vector<func_decl>  m_bv_srem_i;
    ptr_vector<func_decl>  m_bv_urem_i;
    ptr_vector<func_decl>  m_bv_smod_i;

    ptr_vector<func_decl>  m_bv_uleq;
    ptr_vector<func_decl>  m_bv_sleq;
    ptr_vector<func_decl>  m_bv_ugeq;
    ptr_vector<func_decl>  m_bv_sgeq;
    ptr_vector<func_decl>  m_bv_ult;
    ptr_vector<func_decl>  m_bv_slt;
    ptr_vector<func_decl>  m_bv_ugt;
    ptr_vector<func_decl>  m_bv_sgt;

    ptr_vector<func_decl>  m_bv_and;
    ptr_vector<func_decl>  m_bv_or;
    ptr_vector<func_decl>  m_bv_not;
    ptr_vector<func_decl>  m_bv_xor;
    ptr_vector<func_decl>  m_bv_nand;
    ptr_vector<func_decl>  m_bv_nor;
    ptr_vector<func_decl>  m_bv_xnor;

    ptr_vector<func_decl>  m_bv_redor;
    ptr_vector<func_decl>  m_bv_redand;
    ptr_vector<func_decl>  m_bv_comp;

    ptr_vector<func_decl>  m_bv_mul_no_ovfl;
    ptr_vector<func_decl>  m_bv_smul_no_ovfl;
    ptr_vector<func_decl>  m_bv_smul_no_udfl;

    ptr_vector<func_decl> m_bv_mul_ovfl;
    ptr_vector<func_decl> m_bv_smul_ovfl;

    ptr_vector<func_decl> m_bv_sdiv_ovfl;

    ptr_vector<func_decl> m_bv_neg_ovfl;

    ptr_vector<func_decl> m_bv_uadd_ovfl;
    ptr_vector<func_decl> m_bv_sadd_ovfl;

    ptr_vector<func_decl> m_bv_usub_ovfl;
    ptr_vector<func_decl> m_bv_ssub_ovfl;

    ptr_vector<func_decl>  m_bv_shl;
    ptr_vector<func_decl>  m_bv_lshr;
    ptr_vector<func_decl>  m_bv_ashr;
    ptr_vector<func_decl>  m_ext_rotate_left;
    ptr_vector<func_decl>  m_ext_rotate_right;

    ptr_vector<func_decl>  m_ubv2int;
    ptr_vector<func_decl>  m_sbv2int;
    ptr_vector<func_decl>  m_int2bv;
    vector<ptr_vector<func_decl> > m_bit2bool;
    ptr_vector<func_decl>  m_mkbv;

    void set_manager(ast_manager * m, family_id id) override;
    void mk_bv_sort(unsigned bv_size);
    sort * get_bv_sort(unsigned bv_size);
    func_decl * mk_func_decl(decl_kind k, unsigned bv_size);
    func_decl * mk_binary(ptr_vector<func_decl> & decls, decl_kind k,
                          char const * name, unsigned bv_size, bool ac, bool idempotent = false);
    func_decl * mk_unary(ptr_vector<func_decl> & decls, decl_kind k, char const * name, unsigned bv_size);
    func_decl * mk_pred(ptr_vector<func_decl> & decls, decl_kind k,
                        char const * name, unsigned bv_size);
    func_decl * mk_unary_pred(ptr_vector<func_decl> & decls, decl_kind k, char const * name, unsigned bv_size);
    func_decl * mk_reduction(ptr_vector<func_decl> & decls, decl_kind k, char const * name, unsigned bv_size);
    func_decl * mk_comp(unsigned bv_size);
    bool get_bv_size(sort * t, int & result);
    bool get_bv_size(expr * t, int & result);
    bool get_concat_size(unsigned arity, sort * const * domain, int & result);
    bool get_extend_size(unsigned num_parameters, parameter const * parameters,
                         unsigned arity, sort * const * domain, int & result);
    bool get_extract_size(unsigned num_parameters, parameter const * parameters,
                          unsigned arity, sort * const * domain, int & result);

    func_decl * mk_ubv2int(unsigned bv_size, unsigned num_parameters, parameter const * parameters,
                          unsigned arity, sort * const * domain);
    func_decl * mk_sbv2int(unsigned bv_size, unsigned num_parameters, parameter const * parameters,
                          unsigned arity, sort * const * domain);

    func_decl * mk_int2bv(unsigned bv_size, unsigned num_parameters, parameter const * parameters,
                          unsigned arity, sort * const * domain);

    func_decl * mk_bit2bool(unsigned bv_size, unsigned num_parameters, parameter const * parameters,
                            unsigned arity, sort * const * domain);

    func_decl * mk_mkbv(unsigned arity, sort * const * domain);


    func_decl * mk_num_decl(unsigned num_parameters, parameter const * parameters, unsigned arity);

    void get_offset_term(app * a, expr * & t, rational & offset) const;
public:
    bv_decl_plugin();

    void finalize() override;

    decl_plugin * mk_fresh() override { return alloc(bv_decl_plugin); }

    sort * mk_sort(decl_kind k, unsigned num_parameters, parameter const * parameters) override;

    func_decl * mk_func_decl(decl_kind k, unsigned num_parameters, parameter const * parameters,
                             unsigned arity, sort * const * domain, sort * range) override;

    func_decl * mk_func_decl(decl_kind k, unsigned num_parameters, parameter const * parameters,
                             unsigned num_args, expr * const * args, sort * range) override;

    bool is_value(app * e) const override;

    bool is_unique_value(app * e) const override { return is_value(e); }

    void get_op_names(svector<builtin_name> & op_names, symbol const & logic) override;

    void get_sort_names(svector<builtin_name> & sort_names, symbol const & logic) override;

    bool are_distinct(app* a, app* b) const override;

    expr * get_some_value(sort * s) override;

    bool get_int2bv_size(unsigned num_parameters, parameter const * parameters, int & result);

    bool is_considered_uninterpreted(func_decl * f) override {
        if (f->get_family_id() != get_family_id())
            return false;
        switch (f->get_decl_kind()) {
        case OP_BSDIV0:
        case OP_BUDIV0:
        case OP_BSREM0:
        case OP_BUREM0:
        case OP_BSMOD0:
            return true;
        default:
            return false;
        }
        return false;
    }
};

class bv_recognizers {
    family_id m_afid;
public:
    bv_recognizers(family_id fid):m_afid(fid) {}

    family_id get_fid() const { return m_afid; }
    family_id get_family_id() const { return get_fid(); }

    bool is_numeral(expr const * n, rational & val) const;
    bool is_numeral(expr const * n, rational & val, unsigned & bv_size) const;
    bool is_numeral(expr const * n) const { return is_app_of(n, get_fid(), OP_BV_NUM); }
    bool is_allone(expr const * e) const;
    bool is_zero(expr const * e) const;
    bool is_one(expr const* e) const;
    bool is_bv_sort(sort const * s) const;
    bool is_bv(expr const* e) const {  return is_bv_sort(e->get_sort()); }

    bool is_concat(expr const * e) const { return is_app_of(e, get_fid(), OP_CONCAT); }
    bool is_extract(func_decl const * f) const { return is_decl_of(f, get_fid(), OP_EXTRACT); }
    bool is_extract(expr const * e) const { return is_app_of(e, get_fid(), OP_EXTRACT); }
    unsigned get_extract_high(func_decl const * f) const { return f->get_parameter(0).get_int(); }
    unsigned get_extract_low(func_decl const * f) const { return f->get_parameter(1).get_int(); }
    unsigned get_extract_high(expr const * n) const { SASSERT(is_extract(n)); return get_extract_high(to_app(n)->get_decl()); }
    unsigned get_extract_low(expr const * n) const { SASSERT(is_extract(n)); return get_extract_low(to_app(n)->get_decl()); }
    bool is_extract(expr const * e, unsigned & low, unsigned & high, expr * & b) const;
    bool is_repeat(expr const * e, expr*& arg, unsigned& n) const;
    bool is_ubv2int(expr const * e, expr * & r) const;
    bool is_sbv2int(expr const * e, expr * & r) const;
    bool is_bv_add(expr const * e) const { return is_app_of(e, get_fid(), OP_BADD); }
    bool is_bv_sub(expr const * e) const { return is_app_of(e, get_fid(), OP_BSUB); }
    bool is_bv_mul(expr const * e) const { return is_app_of(e, get_fid(), OP_BMUL); }
    bool is_bv_neg(expr const * e) const { return is_app_of(e, get_fid(), OP_BNEG); }

    bool is_bv_sdiv(expr const * e) const { return is_app_of(e, get_fid(), OP_BSDIV); }
    bool is_bv_udiv(expr const * e) const { return is_app_of(e, get_fid(), OP_BUDIV); }
    bool is_bv_srem(expr const * e) const { return is_app_of(e, get_fid(), OP_BSREM); }
    bool is_bv_urem(expr const * e) const { return is_app_of(e, get_fid(), OP_BUREM); }
    bool is_bv_smod(expr const * e) const { return is_app_of(e, get_fid(), OP_BSMOD); }

    bool is_bv_sdiv0(expr const * e) const { return is_app_of(e, get_fid(), OP_BSDIV0); }
    bool is_bv_udiv0(expr const * e) const { return is_app_of(e, get_fid(), OP_BUDIV0); }
    bool is_bv_srem0(expr const * e) const { return is_app_of(e, get_fid(), OP_BSREM0); }
    bool is_bv_urem0(expr const * e) const { return is_app_of(e, get_fid(), OP_BUREM0); }
    bool is_bv_smod0(expr const * e) const { return is_app_of(e, get_fid(), OP_BSMOD0); }

    bool is_bv_sdivi(expr const * e) const { return is_app_of(e, get_fid(), OP_BSDIV_I); }
    bool is_bv_udivi(expr const * e) const { return is_app_of(e, get_fid(), OP_BUDIV_I); }
    bool is_bv_sremi(expr const * e) const { return is_app_of(e, get_fid(), OP_BSREM_I); }
    bool is_bv_uremi(expr const * e) const { return is_app_of(e, get_fid(), OP_BUREM_I); }
    bool is_bv_smodi(expr const * e) const { return is_app_of(e, get_fid(), OP_BSMOD_I); }

    bool is_bv_and(expr const * e) const { return is_app_of(e, get_fid(), OP_BAND); }
    bool is_bv_or(expr const * e) const { return is_app_of(e, get_fid(), OP_BOR); }
    bool is_bv_xor(expr const * e) const { return is_app_of(e, get_fid(), OP_BXOR); }
    bool is_bv_nand(expr const * e) const { return is_app_of(e, get_fid(), OP_BNAND); }
    bool is_bv_nor(expr const * e) const { return is_app_of(e, get_fid(), OP_BNOR); }
    bool is_bv_not(expr const * e) const { return is_app_of(e, get_fid(), OP_BNOT); }
    bool is_bv_ule(expr const * e) const { return is_app_of(e, get_fid(), OP_ULEQ); }
    bool is_bv_sle(expr const * e) const { return is_app_of(e, get_fid(), OP_SLEQ); }
    bool is_ule(expr const * e) const { return is_app_of(e, get_fid(), OP_ULEQ); }
    bool is_sle(expr const * e) const { return is_app_of(e, get_fid(), OP_SLEQ); }
    bool is_ult(expr const * e) const { return is_app_of(e, get_fid(), OP_ULT); }
    bool is_slt(expr const * e) const { return is_app_of(e, get_fid(), OP_SLT); }
    bool is_ugt(expr const * e) const { return is_app_of(e, get_fid(), OP_UGT); }
    bool is_sgt(expr const * e) const { return is_app_of(e, get_fid(), OP_SGT); }
    bool is_uge(expr const * e) const { return is_app_of(e, get_fid(), OP_UGEQ); }
    bool is_sge(expr const * e) const { return is_app_of(e, get_fid(), OP_SGEQ); }
    bool is_bit2bool(expr const * e) const { return is_app_of(e, get_fid(), OP_BIT2BOOL); }
    bool is_ubv2int(expr const* e) const { return is_app_of(e, get_fid(), OP_UBV2INT); }
    bool is_sbv2int(expr const* e) const { return is_app_of(e, get_fid(), OP_SBV2INT); }
    bool is_int2bv(expr const* e) const { return is_app_of(e, get_fid(), OP_INT2BV); }
    bool is_mkbv(expr const * e) const { return is_app_of(e, get_fid(), OP_MKBV); }
    bool is_bv_ashr(expr const * e) const { return is_app_of(e, get_fid(), OP_BASHR); }
    bool is_bv_lshr(expr const * e) const { return is_app_of(e, get_fid(), OP_BLSHR); }
    bool is_bv_shl(expr const * e) const { return is_app_of(e, get_fid(), OP_BSHL); }
    bool is_sign_ext(expr const * e) const { return is_app_of(e, get_fid(), OP_SIGN_EXT); }
    bool is_bv_umul_no_ovfl(expr const* e) const { return is_app_of(e, get_fid(), OP_BUMUL_NO_OVFL); }
    bool is_redand(expr const* e) const { return is_app_of(e, get_fid(), OP_BREDAND); }
    bool is_redor(expr const* e) const { return is_app_of(e, get_fid(), OP_BREDOR); }
    bool is_comp(expr const* e) const { return is_app_of(e, get_fid(), OP_BCOMP); }
    bool is_rotate_left(expr const* e) const { return is_app_of(e, get_fid(), OP_ROTATE_LEFT); }
    bool is_rotate_right(expr const* e) const { return is_app_of(e, get_fid(), OP_ROTATE_RIGHT); }
    bool is_ext_rotate_left(expr const* e) const { return is_app_of(e, get_fid(), OP_EXT_ROTATE_LEFT); }
    bool is_ext_rotate_right(expr const* e) const { return is_app_of(e, get_fid(), OP_EXT_ROTATE_RIGHT); }

    bool is_rotate_left(expr const* e, unsigned& n, expr*& x) const {
        return is_rotate_left(e) && (n = to_app(e)->get_parameter(0).get_int(), x = to_app(e)->get_arg(0), true);
    }
    bool is_rotate_right(expr const* e, unsigned& n, expr*& x) const {
        return is_rotate_right(e) && (n = to_app(e)->get_parameter(0).get_int(), x = to_app(e)->get_arg(0), true);
    }
    bool is_int2bv(expr const* e, unsigned& n, expr*& x) const {
        return is_int2bv(e) && (n = to_app(e)->get_parameter(0).get_int(), x = to_app(e)->get_arg(0), true);
    }

    MATCH_UNARY(is_bv_not);
    MATCH_UNARY(is_redand);
    MATCH_UNARY(is_redor);

    MATCH_BINARY(is_ext_rotate_left);
    MATCH_BINARY(is_ext_rotate_right);
    MATCH_BINARY(is_comp);
    MATCH_BINARY(is_bv_add);
    MATCH_BINARY(is_bv_sub);
    MATCH_BINARY(is_bv_mul);
    MATCH_BINARY(is_bv_sle);
    MATCH_BINARY(is_bv_ule);
    MATCH_BINARY(is_ule);
    MATCH_BINARY(is_sle);
    MATCH_BINARY(is_ult);
    MATCH_BINARY(is_slt);
    MATCH_BINARY(is_uge);
    MATCH_BINARY(is_sge);
    MATCH_BINARY(is_ugt);
    MATCH_BINARY(is_sgt);
    MATCH_BINARY(is_bv_umul_no_ovfl);
    MATCH_BINARY(is_bv_ashr);
    MATCH_BINARY(is_bv_lshr);
    MATCH_BINARY(is_bv_shl);
    MATCH_BINARY(is_bv_urem);
    MATCH_BINARY(is_bv_srem);
    MATCH_BINARY(is_bv_sdiv);
    MATCH_BINARY(is_bv_udiv);
    MATCH_BINARY(is_bv_smod);
    MATCH_BINARY(is_bv_and);
    MATCH_BINARY(is_bv_or);
    MATCH_BINARY(is_bv_xor);
    MATCH_BINARY(is_bv_nand);
    MATCH_BINARY(is_bv_nor);
    MATCH_BINARY(is_concat);


    MATCH_BINARY(is_bv_uremi);
    MATCH_BINARY(is_bv_sremi);
    MATCH_BINARY(is_bv_sdivi);
    MATCH_BINARY(is_bv_udivi);
    MATCH_BINARY(is_bv_smodi);
    MATCH_BINARY(is_bv_urem0);
    MATCH_BINARY(is_bv_srem0);
    MATCH_BINARY(is_bv_sdiv0);
    MATCH_BINARY(is_bv_udiv0);
    MATCH_BINARY(is_bv_smod0);
    MATCH_UNARY(is_bit2bool);
    MATCH_UNARY(is_int2bv);
    bool is_bit2bool(expr* e, expr*& bv, unsigned& idx) const;

    rational norm(rational const & val, unsigned bv_size, bool is_signed) const ;
    rational norm(rational const & val, unsigned bv_size) const { return norm(val, bv_size, false); }
    bool has_sign_bit(rational const & n, unsigned bv_size) const;
};

class bv_util : public bv_recognizers {
    ast_manager &    m_manager;
    bv_decl_plugin * m_plugin;

public:
    bv_util(ast_manager & m);

    ast_manager & get_manager() const { return m_manager; }

    app * mk_numeral(rational const & val, sort* s) const;
    app * mk_numeral(rational const & val, unsigned bv_size) const;
    app * mk_numeral(uint64_t u, unsigned bv_size) const { return mk_numeral(rational(u, rational::ui64()), bv_size); }
    app * mk_zero(sort* s) const { return mk_numeral(rational::zero(), s); }
    app * mk_zero(unsigned bv_size) const { return mk_numeral(rational::zero(), bv_size); }
    app * mk_one(sort* s) const { return mk_numeral(rational::one(), s); }
    app * mk_one(unsigned bv_size) const { return mk_numeral(rational::one(), bv_size); }

    sort * mk_sort(unsigned bv_size);

    unsigned get_bv_size(sort const * s) const {
        SASSERT(is_bv_sort(s));
        return static_cast<unsigned>(s->get_parameter(0).get_int());
    }
    unsigned get_bv_size(expr const * n) const { return get_bv_size(n->get_sort()); }
    unsigned get_int2bv_size(parameter const& p);


    app * mk_ule(expr * arg1, expr * arg2) { return m_manager.mk_app(get_fid(), OP_ULEQ, arg1, arg2); }
    app * mk_sle(expr * arg1, expr * arg2) { return m_manager.mk_app(get_fid(), OP_SLEQ, arg1, arg2); }
    app * mk_slt(expr * arg1, expr * arg2) { return m_manager.mk_app(get_fid(), OP_SLT, arg1, arg2); }
    app * mk_extract(unsigned high, unsigned low, expr * n) {
        parameter params[2] = { parameter(high), parameter(low) };
        return m_manager.mk_app(get_fid(), OP_EXTRACT, 2, params, 1, &n);
    }
    app * mk_concat(unsigned num, expr * const * args) { return m_manager.mk_app(get_fid(), OP_CONCAT, num, args);  }
    app * mk_concat(expr_ref_vector const& es) { return m_manager.mk_app(get_fid(), OP_CONCAT, es.size(), es.data());  }
    app * mk_concat(expr_ref_buffer const& es) { return m_manager.mk_app(get_fid(), OP_CONCAT, es.size(), es.data());  }
    app * mk_concat(ptr_buffer<expr> const& es) { return m_manager.mk_app(get_fid(), OP_CONCAT, es.size(), es.data());  }
    app * mk_concat(ptr_vector<expr> const& es) { return m_manager.mk_app(get_fid(), OP_CONCAT, es.size(), es.data());  }
    app * mk_bv_or(unsigned num, expr * const * args) { return m_manager.mk_app(get_fid(), OP_BOR, num, args);  }
    app * mk_bv_and(unsigned num, expr * const * args) { return m_manager.mk_app(get_fid(), OP_BAND, num, args);  }
    app * mk_bv_xor(unsigned num, expr * const * args) { return m_manager.mk_app(get_fid(), OP_BXOR, num, args);  }

    app * mk_concat(expr * arg1, expr * arg2) { expr * args[2] = { arg1, arg2 }; return mk_concat(2, args); }
    app * mk_bv_and(expr* x, expr* y) { expr* args[2] = { x, y }; return mk_bv_and(2, args); }
    app * mk_bv_or(expr* x, expr* y) { expr* args[2] = { x, y }; return mk_bv_or(2, args); }
    app * mk_bv_xor(expr* x, expr* y) { expr* args[2] = { x, y }; return mk_bv_xor(2, args); }

    app * mk_bv_not(expr * arg) { return m_manager.mk_app(get_fid(), OP_BNOT, arg); }
    app * mk_bv_neg(expr * arg) { return m_manager.mk_app(get_fid(), OP_BNEG, arg); }
    app * mk_bv_urem(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BUREM, arg1, arg2); }
    app * mk_bv_srem(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSREM, arg1, arg2); }
    app * mk_bv_smod(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSMOD, arg1, arg2); }
    app * mk_bv_add(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BADD, arg1, arg2); }
    app * mk_bv_add(ptr_buffer<expr> const & args) const { return m_manager.mk_app(get_fid(), OP_BADD, args.size(), args.data()); }
    app * mk_bv_add(ptr_vector<expr> const & args) const { return m_manager.mk_app(get_fid(), OP_BADD, args.size(), args.data()); }
    app * mk_bv_add(expr_ref_vector const & args) const { return m_manager.mk_app(get_fid(), OP_BADD, args.size(), args.data()); }
    app * mk_bv_add(expr_ref_buffer const & args) const { return m_manager.mk_app(get_fid(), OP_BADD, args.size(), args.data()); }
    app * mk_bv_sub(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSUB, arg1, arg2); }
    app * mk_bv_mul(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BMUL, arg1, arg2); }
    app * mk_bv_mul(unsigned n, expr* const* args) const { return m_manager.mk_app(get_fid(), OP_BMUL, n, args); }
    app* mk_bv_mul(ptr_buffer<expr> const& args) const { return m_manager.mk_app(get_fid(), OP_BMUL, args.size(), args.data()); }
    app* mk_bv_mul(ptr_vector<expr> const& args) const { return m_manager.mk_app(get_fid(), OP_BMUL, args.size(), args.data()); }
    app* mk_bv_mul(expr_ref_vector const& args) const { return m_manager.mk_app(get_fid(), OP_BMUL, args.size(), args.data()); }
    app* mk_bv_mul(expr_ref_buffer const& args) const { return m_manager.mk_app(get_fid(), OP_BMUL, args.size(), args.data()); }
    app * mk_bv_udiv(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BUDIV, arg1, arg2); }
    app * mk_bv_udiv_i(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BUDIV_I, arg1, arg2); }
    app * mk_bv_udiv0(expr * arg) const { return m_manager.mk_app(get_fid(), OP_BUDIV0, arg); }
    app * mk_bv_sdiv(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSDIV, arg1, arg2); }
    app * mk_bv_sdiv_i(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSDIV_I, arg1, arg2); }
    app * mk_bv_sdiv0(expr * arg) const { return m_manager.mk_app(get_fid(), OP_BSDIV0, arg); }
    app * mk_bv_srem_i(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSREM_I, arg1, arg2); }
    app * mk_bv_srem0(expr * arg) const { return m_manager.mk_app(get_fid(), OP_BSREM0, arg); }
    app * mk_bv_urem_i(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BUREM_I, arg1, arg2); }
    app * mk_bv_urem0(expr * arg) const { return m_manager.mk_app(get_fid(), OP_BUREM0, arg); }
    app * mk_bv_smod_i(expr * arg1, expr * arg2) const { return m_manager.mk_app(get_fid(), OP_BSMOD_I, arg1, arg2); }
    app * mk_bv_smod0(expr * arg) const { return m_manager.mk_app(get_fid(), OP_BSMOD0, arg); }
    app * mk_zero_extend(unsigned n, expr* e) {
        parameter p(n);
        return m_manager.mk_app(get_fid(), OP_ZERO_EXT, 1, &p, 1, &e);
    }
    app * mk_sign_extend(unsigned n, expr* e) {
        parameter p(n);
        return m_manager.mk_app(get_fid(), OP_SIGN_EXT, 1, &p, 1, &e);
    }
    app * mk_bv_shl(expr* arg1, expr* arg2) { return m_manager.mk_app(get_fid(), OP_BSHL, arg1, arg2); }
    app * mk_bv_ashr(expr* arg1, expr* arg2) { return m_manager.mk_app(get_fid(), OP_BASHR, arg1, arg2); }
    app * mk_bv_lshr(expr* arg1, expr* arg2) { return m_manager.mk_app(get_fid(), OP_BLSHR, arg1, arg2); }

    app * mk_ubv2int(expr* e) const;
    app * mk_sbv2int(expr* e) const;
    app * mk_sbv2int_as_ubv2int(expr* e);
    app * mk_int2bv(unsigned sz, expr* e) const;

    app* mk_bv_rotate_left(expr* arg1, expr* arg2) { return m_manager.mk_app(get_fid(), OP_EXT_ROTATE_LEFT, arg1, arg2); }
    app* mk_bv_rotate_right(expr* arg1, expr* arg2) { return m_manager.mk_app(get_fid(), OP_EXT_ROTATE_RIGHT, arg1, arg2); }
    app* mk_bv_rotate_left(expr* arg, unsigned n);
    app* mk_bv_rotate_right(expr* arg, unsigned n);

    // TODO: all these binary ops commute (right?) but it'd be more logical to swap `n` & `m` in the `return`
    app * mk_bvsmul_no_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSMUL_NO_OVFL, m, n); }
    app * mk_bvsmul_no_udfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSMUL_NO_UDFL, m, n); }
    app * mk_bvumul_no_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BUMUL_NO_OVFL, m, n); }
    app * mk_bvsmul_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSMUL_OVFL, m, n); }
    app * mk_bvumul_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BUMUL_OVFL, m, n); }
    app * mk_bvsdiv_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSDIV_OVFL, m, n); }
    app * mk_bvneg_ovfl(expr* m) { return m_manager.mk_app(get_fid(), OP_BNEG_OVFL, m); }
    app * mk_bvuadd_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BUADD_OVFL, m, n); }
    app * mk_bvsadd_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSADD_OVFL, m, n); }
    app * mk_bvusub_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BUSUB_OVFL, m, n); }
    app * mk_bvssub_ovfl(expr* m, expr* n) { return m_manager.mk_app(get_fid(), OP_BSSUB_OVFL, m, n); }

    app * mk_bit2bool(expr* e, unsigned idx) { parameter p(idx); return m_manager.mk_app(get_fid(), OP_BIT2BOOL, 1, &p, 1, &e); }

    private:
    void log_bv_from_exprs(app * r, unsigned n, expr* const* es) {
        if (m_manager.has_trace_stream()) {
            for (unsigned i = 0; i < n; ++i) {
                if (!m_manager.is_true(es[i]) && !m_manager.is_false(es[i]))
                    return;
            } 

            if (m_plugin->log_constant_meaning_prelude(r)) {
                if (n % 4 == 0) {
                    m_manager.trace_stream() << " #x";

                    m_manager.trace_stream() << std::hex;
                    uint8_t hexDigit = 0;
                    unsigned curLength = (4 - n % 4) % 4;
                    for (unsigned i = 0; i < n; ++i) {
                        hexDigit <<= 1;
                        ++curLength;
                        if (m_manager.is_true(es[i])) {
                            hexDigit |= 1;
                        }
                        if (curLength == 4) {
                            m_manager.trace_stream() << hexDigit;
                            hexDigit = 0;
                        }
                    }
                    m_manager.trace_stream() << std::dec;
                } else {
                    m_manager.trace_stream() << " #b";
                    for (unsigned i = 0; i < n; ++i) {
                        m_manager.trace_stream() << (m_manager.is_true(es[i]) ? 1 : 0);
                    }
                }

                m_manager.trace_stream() << ")\n";
            }
        }
    }

public:
    app * mk_bv(unsigned n, expr* const* es) {
        app * r = m_manager.mk_app(get_fid(), OP_MKBV, n, es);

        log_bv_from_exprs(r, n, es);

        return r;
    }

};


