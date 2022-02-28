/******************************************************************************
 * Top contributors (to current version):
 *   Gereon Kremer
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Utilities for rewriting atoms in the arithmetic rewriter.
 */

#include "theory/arith/rewriter/rewrite_atom.h"

#include "base/check.h"
#include "theory/arith/rewriter/node_utils.h"

namespace cvc5 {
namespace theory {
namespace arith {
namespace rewriter {

namespace {

/**
 * Evaluate the given relation based on values l and r. Expects that the
 * relational operators `operator<(L,R)`, `operator==(L,R)`, etc are defined.
 */
template <typename L, typename R>
bool evaluateRelation(Kind rel, const L& l, const R& r)
{
  switch (rel)
  {
    case Kind::LT: return l < r;
    case Kind::LEQ: return l <= r;
    case Kind::EQUAL: return l == r;
    case Kind::DISTINCT: return l != r;
    case Kind::GEQ: return l >= r;
    case Kind::GT: return l > r;
    default: Unreachable(); return false;
  }
}

auto getLTermIt(Sum& sum)
{
  auto ltermit = sum.begin();
  if (ltermit->first.isConst())
  {
    ++ltermit;
  }
  return ltermit;
}

auto& getLTerm(Sum& sum)
{
  Assert(getLTermIt(sum) != sum.end());
  return *getLTermIt(sum);
}

/**
 * Normalize the sum, making the leading coefficient to be one or minus one.
 */
void normalizeLCoeffAbsOne(Sum& sum)
{
  if (sum.empty()) return;
  if (sum.size() == 1)
  {
    auto& front = *sum.begin();
    // Trivial if there is only one summand
    front.second = Integer(sgn(front.second) > 0 ? 1 : -1);
    return;
  }
  // LCoeff is first coefficient of non-constant monomial
  RealAlgebraicNumber lcoeff = getLTerm(sum).second;
  ;
  if (sgn(lcoeff) < 0)
  {
    lcoeff = -lcoeff;
  }
  if (isOne(lcoeff)) return;
  for (auto& s : sum)
  {
    s.second = s.second / lcoeff;
  }
}

/**
 * Normalize the sum, making all coefficients integral and their gcd one.
 * If followLCoeffSign is true, the leading coefficient is made positive,
 * possibly negating all other coefficients. If this is the case return true to
 * indicate that the relational operator needs to be negated.
 * Otherwise return false.
 */
bool normalizeGCDLCM(Sum& sum, bool followLCoeffSign = false)
{
  if (sum.empty()) return false;
  Integer denLCM(1);
  Integer numGCD;
  auto it = sum.begin();
  if (!it->first.isConst())
  {
    Rational r = it->second.toRational();
    denLCM = r.getDenominator();
    numGCD = r.getNumerator().abs();
  }
  ++it;
  for (; it != sum.end(); ++it)
  {
    if (it->first.isConst()) continue;
    Assert(it->second.isRational());
    Rational r = it->second.toRational();
    denLCM = denLCM.lcm(r.getDenominator());
    if (numGCD.isZero())
      numGCD = r.getNumerator().abs();
    else
      numGCD = numGCD.gcd(r.getNumerator().abs());
  }
  if (numGCD.isZero()) return false;
  Rational mult(denLCM, numGCD);

  bool negate = false;
  if (followLCoeffSign)
  {
    if (sgn(getLTerm(sum).second) < 0)
    {
      negate = true;
      mult = -mult;
    }
  }

  for (auto& s : sum)
  {
    s.second *= mult;
  }
  return negate;
}

std::pair<Node, RealAlgebraicNumber> removeMinAbsCoeff(Sum& sum)
{
  auto minit = getLTermIt(sum);
  for (auto it = minit; it != sum.end(); ++it)
  {
    if (it->first.isConst()) continue;
    if (it->second.toRational().absCmp(minit->second.toRational()) < 0)
    {
      minit = it;
    }
  }
  if (minit == sum.end())
  {
    return std::make_pair(mkConst(Integer(1)), Integer(0));
  }
  Assert(minit != sum.end());
  auto res = *minit;
  sum.erase(minit);
  return res;
}

RealAlgebraicNumber removeConstant(Sum& sum)
{
  RealAlgebraicNumber res;
  if (!sum.empty())
  {
    auto constantit = sum.begin();
    if (constantit->first.isConst())
    {
      Assert(constantit->first.getConst<Rational>().isOne());
      res = constantit->second;
      sum.erase(constantit);
    }
  }
  return res;
}

std::pair<Node, RealAlgebraicNumber> removeLTerm(Sum& sum)
{
  auto it = getLTermIt(sum);
  if (it == sum.end())
  {
    return std::make_pair(mkConst(Integer(1)), Integer(0));
  }
  Assert(it != sum.end());
  auto res = *it;
  sum.erase(it);
  return res;
}

}  // namespace

std::optional<bool> tryEvaluateRelation(Kind rel, TNode left, TNode right)
{
  if (left.isConst())
  {
    const Rational& l = left.getConst<Rational>();
    if (right.isConst())
    {
      const Rational& r = right.getConst<Rational>();
      return evaluateRelation(rel, l, r);
    }
    else if (right.getKind() == Kind::REAL_ALGEBRAIC_NUMBER)
    {
      const RealAlgebraicNumber& r =
          right.getOperator().getConst<RealAlgebraicNumber>();
      return evaluateRelation(rel, l, r);
    }
  }
  else if (left.getKind() == Kind::REAL_ALGEBRAIC_NUMBER)
  {
    const RealAlgebraicNumber& l =
        left.getOperator().getConst<RealAlgebraicNumber>();
    if (right.isConst())
    {
      const Rational& r = right.getConst<Rational>();
      return evaluateRelation(rel, l, r);
    }
    else if (right.getKind() == Kind::REAL_ALGEBRAIC_NUMBER)
    {
      const RealAlgebraicNumber& r =
          right.getOperator().getConst<RealAlgebraicNumber>();
      return evaluateRelation(rel, l, r);
    }
  }
  return {};
}

std::optional<bool> tryEvaluateRelationReflexive(TNode atom)
{
  if (atom.getNumChildren() == 2 && atom[0] == atom[1])
  {
    switch (atom.getKind())
    {
      case Kind::LT: return false;
      case Kind::LEQ: return true;
      case Kind::EQUAL: return true;
      case Kind::DISTINCT: return false;
      case Kind::GEQ: return true;
      case Kind::GT: return false;
      default:;
    }
  }
  return {};
}

Node buildRelation(Kind kind, Node left, Node right, bool negate)
{
  if (auto response = tryEvaluateRelation(kind, left, right); response)
  {
    return mkConst(*response != negate);
  }
  if (negate)
  {
    return NodeManager::currentNM()->mkNode(kind, left, right).notNode();
  }
  return NodeManager::currentNM()->mkNode(kind, left, right);
}

Node buildIntegerEquality(Sum&& sum)
{
  normalizeGCDLCM(sum);

  const auto& constant = *sum.begin();
  if (constant.first.isConst())
  {
    Assert(constant.second.isRational());
    if (!constant.second.toRational().isIntegral())
    {
      return mkConst(false);
    }
  }

  auto minabscoeff = removeMinAbsCoeff(sum);
  if (sgn(minabscoeff.second) < 0)
  {
    // move minabscoeff goes to the right and switch lhs and rhs
    minabscoeff.second = -minabscoeff.second;
  }
  else
  {
    // move the sum to the right
    for (auto& s : sum) s.second = -s.second;
  }
  Node left = mkMultTerm(minabscoeff.second, minabscoeff.first);

  return buildRelation(Kind::EQUAL, left, collectSum(sum));
}

Node buildRealEquality(Sum&& sum)
{
  auto lterm = removeLTerm(sum);
  if (isZero(lterm.second))
  {
    return buildRelation(Kind::EQUAL, mkConst(Integer(0)), collectSum(sum));
  }
  RealAlgebraicNumber lcoeff = -lterm.second;
  for (auto& s : sum)
  {
    s.second = s.second / lcoeff;
  }
  return buildRelation(Kind::EQUAL, lterm.first, collectSum(sum));
}

Node buildIntegerInequality(Sum&& sum, Kind k)
{
  bool negate = normalizeGCDLCM(sum, true);

  if (negate)
  {
    k = (k == Kind::GEQ) ? Kind::GT : Kind::GEQ;
  }

  RealAlgebraicNumber constant = removeConstant(sum);
  Assert(constant.isRational());
  Rational rhs = -constant.toRational();

  if (rhs.isIntegral() && k == Kind::GT)
  {
    rhs += 1;
  }
  else
  {
    rhs = rhs.ceiling();
  }
  auto* nm = NodeManager::currentNM();
  return buildRelation(Kind::GEQ, collectSum(sum), nm->mkConstInt(rhs), negate);
}

Node buildRealInequality(Sum&& sum, Kind k)
{
  normalizeLCoeffAbsOne(sum);
  Node rhs = mkConst(-removeConstant(sum));
  return buildRelation(k, collectSum(sum), rhs);
}

}  // namespace rewriter
}  // namespace arith
}  // namespace theory
}  // namespace cvc5