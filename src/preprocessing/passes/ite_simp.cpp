/******************************************************************************
 * Top contributors (to current version):
 *   Aina Niemetz, Tim King, Andrew Reynolds
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * ITE simplification preprocessing pass.
 */

#include "preprocessing/passes/ite_simp.h"

#include <vector>

#include "options/base_options.h"
#include "options/smt_options.h"
#include "preprocessing/assertion_pipeline.h"
#include "preprocessing/preprocessing_pass_context.h"
#include "smt/smt_statistics_registry.h"
#include "theory/arith/arith_ite_utils.h"
#include "theory/theory_engine.h"

using namespace std;
using namespace cvc5;
using namespace cvc5::theory;

namespace cvc5 {
namespace preprocessing {
namespace passes {

Node mkAssocAnd(const std::vector<Node>& children)
{
  NodeManager* nm = NodeManager::currentNM();
  if (children.size() == 0)
  {
    return nm->mkConst(true);
  }
  else if (children.size() == 1)
  {
    return children[0];
  }
  else
  {
    const uint32_t max = kind::metakind::getMaxArityForKind(kind::AND);
    const uint32_t min = kind::metakind::getMinArityForKind(kind::AND);

    Assert(min <= children.size());

    unsigned int numChildren = children.size();
    if (numChildren <= max)
    {
      return nm->mkNode(kind::AND, children);
    }

    typedef std::vector<Node>::const_iterator const_iterator;
    const_iterator it = children.begin();
    const_iterator end = children.end();

    /* The new top-level children and the children of each sub node */
    std::vector<Node> newChildren;
    std::vector<Node> subChildren;

    while (it != end && numChildren > max)
    {
      /* Grab the next max children and make a node for them. */
      for (const_iterator next = it + max; it != next; ++it, --numChildren)
      {
        subChildren.push_back(*it);
      }
      Node subNode = nm->mkNode(kind::AND, subChildren);
      newChildren.push_back(subNode);
      subChildren.clear();
    }

    /* If there's children left, "top off" the Expr. */
    if (numChildren > 0)
    {
      /* If the leftovers are too few, just copy them into newChildren;
       * otherwise make a new sub-node  */
      if (numChildren < min)
      {
        for (; it != end; ++it)
        {
          newChildren.push_back(*it);
        }
      }
      else
      {
        for (; it != end; ++it)
        {
          subChildren.push_back(*it);
        }
        Node subNode = nm->mkNode(kind::AND, subChildren);
        newChildren.push_back(subNode);
      }
    }

    /* It's inconceivable we could have enough children for this to fail
     * (more than 2^32, in most cases?). */
    AlwaysAssert(newChildren.size() <= max)
        << "Too many new children in mkAssociative";

    /* It would be really weird if this happened (it would require
     * min > 2, for one thing), but let's make sure. */
    AlwaysAssert(newChildren.size() >= min)
        << "Too few new children in mkAssociative";

    return nm->mkNode(kind::AND, newChildren);
  }
}

/* -------------------------------------------------------------------------- */

namespace {

/**
 * Ensures the assertions asserted after index 'before' now effectively come
 * before 'real_assertions_end'.
 */
void compressBeforeRealAssertions(AssertionPipeline* assertionsToPreprocess,
                                  size_t before)
{
  size_t cur_size = assertionsToPreprocess->size();
  if (before >= cur_size || assertionsToPreprocess->getRealAssertionsEnd() <= 0
      || assertionsToPreprocess->getRealAssertionsEnd() >= cur_size)
  {
    return;
  }

  // assertions
  // original: [0 ... assertionsToPreprocess.getRealAssertionsEnd())
  //  can be modified
  // ites skolems [assertionsToPreprocess.getRealAssertionsEnd(), before)
  //  cannot be moved
  // added [before, cur_size)
  //  can be modified
  Assert(0 < assertionsToPreprocess->getRealAssertionsEnd());
  Assert(assertionsToPreprocess->getRealAssertionsEnd() <= before);
  Assert(before < cur_size);

  std::vector<Node> intoConjunction;
  for (size_t i = before; i < cur_size; ++i)
  {
    intoConjunction.push_back((*assertionsToPreprocess)[i]);
  }
  assertionsToPreprocess->resize(before);
  size_t lastBeforeItes = assertionsToPreprocess->getRealAssertionsEnd() - 1;
  intoConjunction.push_back((*assertionsToPreprocess)[lastBeforeItes]);
  Node newLast = mkAssocAnd(intoConjunction);
  assertionsToPreprocess->replace(lastBeforeItes, newLast);
  Assert(assertionsToPreprocess->size() == before);
}

}  // namespace

/* -------------------------------------------------------------------------- */

ITESimp::Statistics::Statistics(StatisticsRegistry& reg)
    : d_arithSubstitutionsAdded(reg.registerInt(
        "preprocessing::passes::ITESimp::ArithSubstitutionsAdded"))
{
}

Node ITESimp::simpITE(util::ITEUtilities* ite_utils, TNode assertion)
{
  if (!ite_utils->containsTermITE(assertion))
  {
    return assertion;
  }
  else
  {
    Node result = ite_utils->simpITE(assertion);
    Node res_rewritten = rewrite(result);

    if (options().smt.simplifyWithCareEnabled)
    {
      verbose(2) << "starting simplifyWithCare()" << endl;
      Node postSimpWithCare = ite_utils->simplifyWithCare(res_rewritten);
      verbose(2) << "ending simplifyWithCare()"
             << " post simplifyWithCare()" << postSimpWithCare.getId() << endl;
      result = rewrite(postSimpWithCare);
    }
    else
    {
      result = res_rewritten;
    }
    return result;
  }
}

bool ITESimp::doneSimpITE(AssertionPipeline* assertionsToPreprocess)
{
  bool result = true;
  bool simpDidALotOfWork = d_iteUtilities.simpIteDidALotOfWorkHeuristic();
  if (simpDidALotOfWork)
  {
    if (options().smt.compressItes)
    {
      result = d_iteUtilities.compress(assertionsToPreprocess);
    }

    if (result)
    {
      // if false, don't bother to reclaim memory here.
      NodeManager* nm = NodeManager::currentNM();
      if (nm->poolSize() >= options().smt.zombieHuntThreshold)
      {
        verbose(2) << "..ite simplifier did quite a bit of work.. "
               << nm->poolSize() << endl;
        verbose(2) << "....node manager contains " << nm->poolSize()
               << " nodes before cleanup" << endl;
        d_iteUtilities.clear();
        d_env.getRewriter()->clearCaches();
        nm->reclaimZombiesUntil(options().smt.zombieHuntThreshold);
        verbose(2) << "....node manager contains " << nm->poolSize()
               << " nodes after cleanup" << endl;
      }
    }
  }

  // Do theory specific preprocessing passes
  if (logicInfo().isTheoryEnabled(theory::THEORY_ARITH)
      && !options().base.incrementalSolving)
  {
    if (!simpDidALotOfWork)
    {
      util::ContainsTermITEVisitor& contains =
          *(d_iteUtilities.getContainsVisitor());
      arith::ArithIteUtils aiteu(
          d_env, contains, d_preprocContext->getTopLevelSubstitutions().get());
      bool anyItes = false;
      for (size_t i = 0, size = assertionsToPreprocess->size(); i < size; ++i)
      {
        Node curr = (*assertionsToPreprocess)[i];
        if (contains.containsTermITE(curr))
        {
          anyItes = true;
          Node res = aiteu.reduceVariablesInItes(curr);
          Trace("arith::ite::red") << "@ " << i << " ... " << curr << endl
                                   << "   ->" << res << endl;
          if (curr != res)
          {
            Node more = aiteu.reduceConstantIteByGCD(res);
            Trace("arith::ite::red") << "  gcd->" << more << endl;
            Node morer = rewrite(more);
            assertionsToPreprocess->replace(i, morer);
          }
        }
      }
      if (!anyItes)
      {
        unsigned prevSubCount = aiteu.getSubCount();
        aiteu.learnSubstitutions(assertionsToPreprocess->ref());
        if (prevSubCount < aiteu.getSubCount())
        {
          d_statistics.d_arithSubstitutionsAdded +=
              aiteu.getSubCount() - prevSubCount;
          bool anySuccess = false;
          for (size_t i = 0, N = assertionsToPreprocess->size(); i < N; ++i)
          {
            Node curr = (*assertionsToPreprocess)[i];
            Node next = rewrite(aiteu.applySubstitutions(curr));
            Node res = aiteu.reduceVariablesInItes(next);
            Trace("arith::ite::red") << "@ " << i << " ... " << next << endl
                                     << "   ->" << res << endl;
            Node more = aiteu.reduceConstantIteByGCD(res);
            Trace("arith::ite::red") << "  gcd->" << more << endl;
            if (more != next)
            {
              anySuccess = true;
              break;
            }
          }
          for (size_t i = 0, N = assertionsToPreprocess->size();
               anySuccess && i < N;
               ++i)
          {
            Node curr = (*assertionsToPreprocess)[i];
            Node next = rewrite(aiteu.applySubstitutions(curr));
            Node res = aiteu.reduceVariablesInItes(next);
            Trace("arith::ite::red") << "@ " << i << " ... " << next << endl
                                     << "   ->" << res << endl;
            Node more = aiteu.reduceConstantIteByGCD(res);
            Trace("arith::ite::red") << "  gcd->" << more << endl;
            Node morer = rewrite(more);
            assertionsToPreprocess->replace(i, morer);
          }
        }
      }
    }
  }
  return result;
}

/* -------------------------------------------------------------------------- */

ITESimp::ITESimp(PreprocessingPassContext* preprocContext)
    : PreprocessingPass(preprocContext, "ite-simp"),
      d_iteUtilities(d_env),
      d_statistics(statisticsRegistry())
{
}

PreprocessingPassResult ITESimp::applyInternal(
    AssertionPipeline* assertionsToPreprocess)
{
  d_preprocContext->spendResource(Resource::PreprocessStep);

  size_t nasserts = assertionsToPreprocess->size();
  for (size_t i = 0; i < nasserts; ++i)
  {
    d_preprocContext->spendResource(Resource::PreprocessStep);
    Node simp = simpITE(&d_iteUtilities, (*assertionsToPreprocess)[i]);
    assertionsToPreprocess->replace(i, simp);
    if (simp.isConst() && !simp.getConst<bool>())
    {
      return PreprocessingPassResult::CONFLICT;
    }
  }
  bool done = doneSimpITE(assertionsToPreprocess);
  if (nasserts < assertionsToPreprocess->size())
  {
    compressBeforeRealAssertions(assertionsToPreprocess, nasserts);
  }
  return done ? PreprocessingPassResult::NO_CONFLICT
              : PreprocessingPassResult::CONFLICT;
}


/* -------------------------------------------------------------------------- */

}  // namespace passes
}  // namespace preprocessing
}  // namespace cvc5
