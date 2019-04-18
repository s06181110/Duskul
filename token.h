/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__TOKEN_H__)
#define __TOKEN_H__

#include "commontype.h"

enum token {
    tok_id, tok_num, tok_str,
    sym_xxx, // characters not used ( '!', '^', '`', '~' )
    /* ---- characters ---- */
    sym_quo, sym_shp, sym_doll, sym_pcnt, sym_amp, sym_squo,
    sym_lpar, sym_rpar, sym_ast, sym_plus, sym_comma, sym_minus, sym_dot,
    sym_sls, sym_col, sym_scol, sym_lt, sym_eq, sym_gt, sym_ques,
    sym_at, sym_lbk, sym_bsls, sym_rbk, sym_us,
    sym_lbrace, sym_vbar, sym_rbrace,
    /* ---- combined symbols ---- */
    combined_symbol_0,
    sym_leq = combined_symbol_0 /* <= */,
    sym_geq /* >= */, sym_equal /* == */, sym_neq /* <> */,
    /* ---- the followings are reserved words ---- */
    reserved_word_0,
    sym_and = reserved_word_0,
    sym_break, sym_call, sym_decl, sym_do, sym_else, sym_elsif,
    sym_end, sym_for, sym_func, sym_if, sym_input, sym_not,
    sym_or, sym_print, sym_println, sym_proc, sym_return,
    sym_step, sym_then, sym_to, sym_var, sym_while,
    all_normal_symbols,
    /* ---- Special Tokens ---- */
    tok_ILL = all_normal_symbols,
    tok_EOD,
    total_num_of_symbols,
};

#if total_num_of_symbols < 256
typedef unsigned char token_t;
#else
typedef unsigned short token_t;
#endif

#endif /* !__TOKEN_H__ */
