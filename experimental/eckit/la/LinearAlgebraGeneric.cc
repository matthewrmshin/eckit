/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

//-----------------------------------------------------------------------------

#include "eckit/la/LinearAlgebraFactory.h"
#include "eckit/la/LinearAlgebraGeneric.h"
#include "eckit/la/Matrix.h"
#include "eckit/la/SparseMatrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

LinearAlgebraGeneric::LinearAlgebraGeneric() {
    LinearAlgebraFactory::regist("generic", this);
}

//-----------------------------------------------------------------------------

Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());
    Scalar r;
    for (size_t i = 0; i < x.size(); ++i) r += x[i] * y[i];
    return r;
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    for (size_t r = 0; r < A.rows(); ++r) {
        y[r] = 0.;
        for (size_t c = 0; c < A.cols(); ++c) y[r] += A(r, c) * x[c];
    }
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    for (size_t c = 0; c < B.cols(); ++c)
        for (size_t r = 0; r < A.rows(); ++r)
            for (size_t k = 0; k < A.cols(); ++k) C(r, c) += A(r, k) * B(k, c);
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    const Index* outer = A.outer();
    const Index* inner = A.inner();
    const Scalar* val = A.data();
    for (size_t r = 0; r < A.rows(); ++r)
        for (Index oi = outer[r]; oi < outer[r+1]; ++oi)
            y[r] += val[oi] * x[inner[oi]];

}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::spmm(const SparseMatrix&, const Matrix&, Matrix&) const {
    NOTIMP;
}

//-----------------------------------------------------------------------------

static LinearAlgebraGeneric linearAlgebraGeneric;

//-----------------------------------------------------------------------------

} // namespace la
} // namespace eckit
