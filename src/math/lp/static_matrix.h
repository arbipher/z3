/*++
Copyright (c) 2017 Microsoft Corporation

Author:
    Lev Nachmanson (levnach)
--*/

#pragma once
#include "util/vector.h"
#include <set>
#include <unordered_map>
#include <utility>
#include "math/lp/indexed_vector.h"
#include "math/lp/permutation_matrix.h"
#include <stack>
namespace lp {

template <typename T>
class row_cell {
    unsigned m_j;         // points to the column
    unsigned m_offset;    // offset in column
    T        m_coeff;     // coefficient
public:
    row_cell(unsigned j, unsigned offset, T const & val) : m_j(j), m_offset(offset), m_coeff(val) {
    }
    row_cell(unsigned j, unsigned offset) : m_j(j), m_offset(offset) {
    }
    inline const T & coeff() const { return m_coeff; }
    inline T & coeff() { return m_coeff; }
    inline unsigned var() const { return m_j; }
    inline unsigned & var() { return m_j; }
    inline unsigned offset() const { return m_offset; }
    inline unsigned & offset() { return m_offset; }            
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const row_cell<T>& rc) {
    return out << "(j=" << rc.var() << ", offset= " << rc.offset() << ", coeff=" << rc.coeff() << ")";   
}
struct empty_struct {};
typedef row_cell<empty_struct> column_cell;
typedef std_vector<column_cell> column_strip;

template <typename T>
using row_strip = std_vector<row_cell<T>>; 
template <typename K> mpq get_denominators_lcm(const K & row) {
    SASSERT(row.size() > 0);
    mpq r = mpq(1);
    for (const auto & c : row) 
        r = lcm(r, denominator(c.coeff()));
    return r;
}


    
template <typename T>
std::ostream& operator<<(std::ostream& out, const row_strip<T>& r) {
    for (auto const& c : r)
        out << c << " ";
    return out << "\n";
}

// each assignment for this matrix should be issued only once!!!
template <typename T, typename X>
class static_matrix
#ifdef Z3DEBUG
    : public matrix<T, X>
#endif
{
    struct dim {
        unsigned m_m;
        unsigned m_n;
        dim(unsigned m, unsigned n) :m_m(m), m_n(n) {}
    };
    std::stack<dim> m_stack;
public:
    
    vector<int> m_work_vector_of_row_offsets;
    indexed_vector<T> m_work_vector;
    std_vector<row_strip<T>> m_rows;
    std_vector<column_strip> m_columns;
    // starting inner classes
    class ref {
        static_matrix & m_matrix;
        unsigned m_row;
        unsigned m_col;
    public:
        ref(static_matrix & m, unsigned row, unsigned col):m_matrix(m), m_row(row), m_col(col) {}
        ref & operator=(T const & v) { m_matrix.set( m_row, m_col, v); return *this; }

        ref operator=(ref & v) { m_matrix.set(m_row, m_col, v.m_matrix.get_elem(v.m_row, v.m_col)); return *this; }

        operator T () const { return m_matrix.get_elem(m_row, m_col); }
    };

public:
    const auto & operator[](unsigned i) const { return m_rows[i]; }
    
    const T & get_val(const column_cell & c) const {
        return m_rows[c.var()][c.offset()].coeff();
    }

    column_cell & get_column_cell(const row_cell<T> &rc) {
        return m_columns[rc.var()][rc.offset()];
    }
    
    void init_row_columns(unsigned m, unsigned n);

        // constructor with no parameters
    static_matrix() = default;
    // constructor
    static_matrix(unsigned m, unsigned n): m_work_vector_of_row_offsets(n, -1)  {
        init_row_columns(m, n);
    }
    
    void clear();

    void init_vector_of_row_offsets();

    void init_empty_matrix(unsigned m, unsigned n);

    unsigned row_count() const { return static_cast<unsigned>(m_rows.size()); }

    unsigned column_count() const { return static_cast<unsigned>(m_columns.size()); }

    unsigned lowest_row_in_column(unsigned col);

    void add_new_element(unsigned i, unsigned j, const T & v);

    // adds row i muliplied by coeff to row k
    void add_rows(const mpq& coeff, unsigned i, unsigned k);
    void add_row() {m_rows.push_back(row_strip<T>());}
    void add_column() {
        m_columns.push_back(column_strip());
        m_work_vector_of_row_offsets.push_back(-1);
    }

    void add_columns_up_to(unsigned j) { while (j >= column_count()) add_column(); }

    void remove_element(std_vector<row_cell<T>> & row, row_cell<T> & elem_to_remove);

    void remove_element(unsigned ei, row_cell<T> & elem_to_remove) {
        remove_element(m_rows[ei], elem_to_remove);
    }
    
    
    void multiply_column(unsigned column, T const & alpha) {
        for (auto & t : m_columns[column]) {
            auto & r = m_rows[t.var()][t.offset()];
            r.coeff() *= alpha;
        }
    }
    

#ifdef Z3DEBUG
    void regen_domain();
#endif

    // offs - offset in columns
    row_cell<T> make_row_cell(unsigned row, unsigned offs, T const & val) {
        return row_cell<T>(row, offs, val);
    }

    column_cell make_column_cell(unsigned column, unsigned offset) {
        return column_cell(column, offset);
    }

    void set(unsigned row, unsigned col, T const & val);

    ref operator()(unsigned row, unsigned col) { return ref(*this, row, col); }

    std::set<std::pair<unsigned, unsigned>>  get_domain();

    T get_max_abs_in_row(unsigned row) const;
    void add_column_to_vector (const T & a, unsigned j, T * v) const {
        for (const auto & it : m_columns[j]) {
            v[it.var()] += a * get_val(it);
        }
    }

    T get_min_abs_in_row(unsigned row) const;
    T get_max_abs_in_column(unsigned column) const;

    T get_min_abs_in_column(unsigned column) const;

#ifdef Z3DEBUG
    void check_consistency();
#endif


    void cross_out_row(unsigned k);

    //
    void fix_row_indices_in_each_column_for_crossed_row(unsigned k);

    void cross_out_row_from_columns(unsigned k, row_strip<T> & row);

    void cross_out_row_from_column(unsigned col, unsigned k);

    T get_elem(unsigned i, unsigned j) const;


    unsigned number_of_non_zeroes_in_column(unsigned j) const { return static_cast<unsigned>(m_columns[j].size()); }

    unsigned number_of_non_zeroes_in_row(unsigned i) const { return m_rows[i].size(); }

    unsigned number_of_non_zeroes() const {
        unsigned ret = 0;
        for (unsigned i = 0; i < row_count(); i++)
            ret += number_of_non_zeroes_in_row(i);
        return ret;
    }
    
    void scan_row_strip_to_work_vector(const row_strip<T> & rvals);
    void clean_row_work_vector(unsigned i);


#ifdef Z3DEBUG
    unsigned get_number_of_rows() const { return row_count(); }
    unsigned get_number_of_columns() const { return column_count(); }
    virtual void set_number_of_rows(unsigned /*m*/) { }
    virtual void set_number_of_columns(unsigned /*n*/) { }
#endif

    T get_balance() const;

    T get_row_balance(unsigned row) const;

    bool is_correct() const;
    void push() {
        dim d(row_count(), column_count());
        m_stack.push(d);
    }

    void pop_row_columns(const std_vector<row_cell<T>> & row) {
        for (auto & c : row) {
            unsigned j = c.var();
            auto & col = m_columns[j];
            SASSERT(col[col.size() - 1].var() == m_rows.size() -1 ); // todo : start here!!!!
            col.pop_back();
        }
    }

    
    
    void pop(unsigned k) {
#ifdef Z3DEBUG
        std::set<std::pair<unsigned, unsigned>> pairs_to_remove_from_domain;
#endif
        
        
        while (k-- > 0) {
            if (m_stack.empty()) break;
            unsigned m = m_stack.top().m_m;
            while (m < row_count()) {
                unsigned i = static_cast<unsigned>(m_rows.size() -1);
                auto & row = m_rows[i];
                pop_row_columns(row);
                m_rows.pop_back(); // delete the last row
            }
            unsigned n = m_stack.top().m_n;
            while (n < column_count())
                m_columns.pop_back(); // delete the last column
            m_stack.pop();
        }
        SASSERT(is_correct());
    }

    void multiply_row(unsigned row, T const & alpha) {
        for (auto & t : m_rows[row]) {
            t.coeff() *= alpha;
        }
    }

    void divide_row(unsigned row, T const & alpha) {
        for (auto & t : m_rows[row]) {
            t.coeff() /= alpha;
        }
    }
    
    T dot_product_with_column(const std_vector<T> & y, unsigned j) const {
        SASSERT(j < column_count());
        T ret = numeric_traits<T>::zero();
        for (auto & it : m_columns[j]) {
            ret += y[it.var()] * get_val(it); // get_value_of_column_cell(it);
        }
        return ret;
    }
    template <typename TTerm>
    void pivot_term_to_row_given_cell(TTerm const & term, column_cell&c, unsigned j, int j_sign);
    template <typename TTerm>
    void add_term_to_row(const mpq& coeff, TTerm const & term, unsigned i);


// pivot row i to row ii
    bool pivot_row_to_row_given_cell(unsigned i, column_cell& c, unsigned j);
    void pivot_row_to_row_given_cell_with_sign(unsigned piv_row_index, column_cell& c, unsigned j, int j_sign);
    void transpose_rows(unsigned i, unsigned ii) {
        auto t = m_rows[i];
        m_rows[i] = m_rows[ii];
        m_rows[ii] = t;
        // now fix the columns
        for (auto & rc : m_rows[i]) {
            column_cell & cc = m_columns[rc.var()][rc.offset()];
            SASSERT(cc.var() == ii);
            cc.var() = i;
        }
        for (auto & rc : m_rows[ii]) {
            column_cell & cc = m_columns[rc.var()][rc.offset()];
            SASSERT(cc.var() == i);
            cc.var() = ii;
        }
    
    }
    void fill_last_row_with_pivoting_loop_block(unsigned j, const std_vector<int> & basis_heading) {
        int row_index = basis_heading[j];
        if (row_index < 0)
            return;
        T & alpha = m_work_vector[j]; // the pivot alpha
        if (is_zero(alpha))
            return;
        
        for (const auto & c : m_rows[row_index]) {
            if (c.var() == j) {
                continue;
            }
            T & wv = m_work_vector.m_data[c.var()];
            bool was_zero = is_zero(wv);
            wv -= alpha * c.coeff();
            if (was_zero)
                m_work_vector.m_index.push_back(c.var());
            else {
                if (is_zero(wv)) {
                    m_work_vector.erase_from_index(c.var());
                }
            }
        }
        alpha = zero_of_type<T>();
        m_work_vector.erase_from_index(j);
    }

    
    
    template <typename term>
    void fill_last_row_with_pivoting(const term& row,
                                     unsigned bj, // the index of the basis column
                                     const std_vector<int> & basis_heading) {
        SASSERT(row_count() > 0);
        m_work_vector.clear();
        m_work_vector.resize(column_count());
        T a;
         // we use the form -it + 1 = 0
        m_work_vector.set_value(one_of_type<T>(), bj);
        for (auto p : row) {
            m_work_vector.set_value(-p.coeff(), p.j());
            // but take care of the basis 1 later
        }
    
        // now iterate with pivoting
        fill_last_row_with_pivoting_loop_block(bj, basis_heading);
        for (auto p : row) {
            fill_last_row_with_pivoting_loop_block(p.j(), basis_heading);
        }
        unsigned last_row = row_count() - 1;
    
        for (unsigned j : m_work_vector.m_index) {
            set (last_row, j, m_work_vector.m_data[j]);
        }
        SASSERT(column_count() > 0);
        set(last_row, column_count() - 1, one_of_type<T>());
    }

    void copy_column_to_vector (unsigned j, std_vector<T> & v) const {
        v.resize(row_count(), numeric_traits<T>::zero());
        for (auto & it : m_columns[j]) {
            const T& val = get_val(it);
            if (!is_zero(val))
                v[it.var()] = val;
        }
    }
    
    template <typename L>
    L dot_product_with_row(unsigned row, const std_vector<L> & w) const {
        L ret = zero_of_type<L>();
        SASSERT(row < m_rows.size());
        for (auto & it : m_rows[row]) {
            ret += w[it.var()] * it.coeff();
        }
        return ret;
    }

    struct column_cell_plus {
        const column_cell & m_c;
        const static_matrix& m_A;
        // constructor
        column_cell_plus(const column_cell & c, const static_matrix& A) :
            m_c(c), m_A(A) {}
        unsigned var() const { return m_c.var(); }
        const T & coeff() const { return m_A.m_rows[var()][m_c.offset()].coeff(); }
        
    };
    
    struct column_container {
        unsigned              m_j; // the column index
        const static_matrix & m_A;
        column_container(unsigned j, const static_matrix& A) : m_j(j), m_A(A) {
        }
        struct const_iterator {
            // fields
            const column_cell *m_c;
            const static_matrix& m_A;

            //typedefs
            
            
            typedef const_iterator self_type;
            typedef column_cell_plus value_type;
            typedef const column_cell_plus reference;
            //            typedef const column_cell* pointer;
            typedef int difference_type;
            typedef std::forward_iterator_tag iterator_category;

            reference operator*() const {
                return column_cell_plus(*m_c, m_A);
            }        
            self_type operator++() {  self_type i = *this; m_c++; return i;  }
            self_type operator++(int) { m_c++; return *this; }

            const_iterator(const column_cell* it, const static_matrix& A) :
                m_c(it),
                m_A(A)
            {}
            bool operator==(const self_type &other) const {
                return m_c == other.m_c;
            }
            bool operator!=(const self_type &other) const { return !(*this == other); }
        };

        const_iterator begin() const {
            return const_iterator(m_A.m_columns[m_j].data(), m_A);
        }
        
        const_iterator end() const {
            const auto & column = m_A.m_columns[m_j];
            return const_iterator(column.data() + column.size(), m_A);
        }
    };

    column_container column(unsigned j) const {
        return column_container(j, *this);
    }

    typedef T coefftype;
    typedef X argtype;
};

}
