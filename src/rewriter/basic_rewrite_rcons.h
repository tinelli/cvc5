/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds, Hans-Jörg Schurr
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2024 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * The module for basic (non-DSL-dependent) automatic reconstructing proofs of
 * THEORY_REWRITE steps.
 */

#include "cvc5_private.h"

#ifndef CVC5__REWRITER__BASIC_REWRITE_RCONS_H
#define CVC5__REWRITER__BASIC_REWRITE_RCONS_H

#include <unordered_set>

#include "expr/node.h"
#include "proof/proof.h"
#include "proof/proof_node_manager.h"
#include "smt/env_obj.h"
#include "theory/builtin/proof_checker.h"
#include "theory/rewriter.h"

namespace cvc5::internal {
namespace rewriter {

/**
 * The module for basic (non-DSL-dependent) automatic reconstructing proofs of
 * THEORY_REWRITE steps. It handles special cases that are independent
 * of the user-provided DSL rules, including EVALUATE, REFL, BETA_REDUCE.
 */
class BasicRewriteRCons : protected EnvObj
{
 public:
  BasicRewriteRCons(Env& env);
  ~BasicRewriteRCons() {}
  /**
   * Try to prove (= a b), where a ---> b was a theory rewrite from theory
   * tid with the given method. If this method returns true, then a proof
   * of (= a b) was added to cdp.
   * @param cdp The proof to add to.
   * @param a The left hand side of the equality.
   * @param b The left hand side of the equality.
   * @param tid The theory that was the source of the rewrite (if any).
   * @param tid The method that was the source of the rewrite (if any).
   * @param subgoals The list of proofs introduced when proving eq that
   * are trusted steps.
   * @return true if we successfully added a proof of (= a b) to cdp.
   */
  bool prove(CDProof* cdp,
             Node a,
             Node b,
             theory::TheoryId tid,
             MethodId mid,
             std::vector<std::shared_ptr<ProofNode>>& subgoals);
  /**
   * There are theory rewrites which cannot be expressed in RARE rules. In this
   * case we need to use proof rules which are not written in RARE. It is only
   * used as a last resort method so this is executed only when other rules
   * fail.
   * @param cdp The proof to add to.
   * @param a The left hand side of the equality.
   * @param b The left hand side of the equality.
   * @param tid The theory that was the source of the rewrite (if any).
   * @param tid The method that was the source of the rewrite (if any).
   * @param subgoals The list of proofs introduced when proving eq that
   * are trusted steps.
   * @return true if we successfully added a proof of (= a b) to cdp.
   */
  bool postProve(CDProof* cdp,
                 Node a,
                 Node b,
                 theory::TheoryId tid,
                 MethodId mid,
                 std::vector<std::shared_ptr<ProofNode>>& subgoals);
  /**
   * Ensure we have a proof for theory rewrite id of eq in cdp. This typically
   * adds a single THEORY_REWRITE step to cdp. However, for rules with prefix
   * MACRO_, we perform elaboration.
   * @param cdp The proof to add to.
   * @param id The theory rewrite that proves eq.
   * @param eq The conclusion of the theory rewrite.
   * @param subgoals The list of proofs introduced when proving eq that
   * are trusted steps.
   */
  void ensureProofForTheoryRewrite(
      CDProof* cdp,
      ProofRewriteRule id,
      const Node& eq,
      std::vector<std::shared_ptr<ProofNode>>& subgoals);

 private:
  /**
   * Is proof-granularity set to dsl-rewrite-strict? This impacts when
   * THEORY_REWRITE are tried.
   */
  bool d_isDslStrict;
  /**
   * Try rule r, return true if eq could be proven by r with arguments args.
   * If this method returns true, a proof of eq was added to cdp.
   * If addStep is true, we add the proof to cdp. Otherwise, the caller is
   * responsible for adding the proof.
   */
  bool tryRule(CDProof* cdp,
               Node eq,
               ProofRule r,
               const std::vector<Node>& args,
               bool addStep = true);
  /**
   * Elaborate a rewrite eq that was proven by
   * ProofRewriteRule::MACRO_BOOL_NNF_NORM.
   *
   * @param cdp The proof to add to.
   * @param eq The rewrite proven by ProofRewriteRule::MACRO_BOOL_NNF_NORM.
   * @param subgoals The list of proofs introduced when proving eq that
   * are trusted steps. These are small step rewrites corresponding to NNF
   * flattening of operators, and other simple inferences.
   * @return true if added a closed proof of eq to cdp.
   */
  bool ensureProofMacroBoolNnfNorm(
      CDProof* cdp,
      const Node& eq,
      std::vector<std::shared_ptr<ProofNode>>& subgoals);
  /**
   * Try THEORY_REWRITE with theory::TheoryRewriteCtx ctx.
   */
  bool tryTheoryRewrite(CDProof* cdp,
                        const Node& eq,
                        theory::TheoryRewriteCtx ctx,
                        std::vector<std::shared_ptr<ProofNode>>& subgoals);
};

}  // namespace rewriter
}  // namespace cvc5::internal

#endif
