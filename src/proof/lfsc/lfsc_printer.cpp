/******************************************************************************
 * Top contributors (to current version):
 *   Andrew Reynolds
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * The printer for LFSC proofs
 */

#include "proof/lfsc/lfsc_printer.h"

#include <sstream>

#include "expr/dtype.h"
#include "expr/dtype_cons.h"
#include "expr/dtype_selector.h"
#include "expr/node_algorithm.h"
#include "expr/skolem_manager.h"
#include "proof/lfsc/lfsc_list_sc_node_converter.h"
#include "proof/lfsc/lfsc_print_channel.h"

using namespace cvc5::kind;

namespace cvc5 {
namespace proof {

LfscPrinter::LfscPrinter(LfscNodeConverter& ltp)
    : d_tproc(ltp), d_assumpCounter(0)
{
  NodeManager* nm = NodeManager::currentNM();
  d_boolType = nm->booleanType();
  // used for the `flag` type in LFSC
  d_tt = d_tproc.mkInternalSymbol("tt", d_boolType);
  d_ff = d_tproc.mkInternalSymbol("ff", d_boolType);
}

void LfscPrinter::print(std::ostream& out,
                        const std::vector<Node>& assertions,
                        const ProofNode* pn)
{
  // to be added
}

void LfscPrinter::printProofLetify(
    LfscPrintChannel* out,
    const ProofNode* pn,
    const LetBinding& lbind,
    const std::vector<const ProofNode*>& pletList,
    std::map<const ProofNode*, size_t>& pletMap,
    std::map<Node, size_t>& passumeMap)
{
  // closing parentheses
  size_t cparen = 0;

  // define the let proofs
  if (!pletList.empty())
  {
    std::map<const ProofNode*, size_t>::iterator itp;
    for (const ProofNode* p : pletList)
    {
      itp = pletMap.find(p);
      Assert(itp != pletMap.end());
      size_t pid = itp->second;
      // print (plet _ _
      out->printOpenLfscRule(LfscRule::PLET);
      cparen++;
      out->printHole();
      out->printHole();
      out->printEndLine();
      // print the letified proof
      pletMap.erase(p);
      printProofInternal(out, p, lbind, pletMap, passumeMap);
      pletMap[p] = pid;
      // print the lambda (\ __pX
      out->printOpenLfscRule(LfscRule::LAMBDA);
      cparen++;
      out->printProofId(pid);
      out->printEndLine();
    }
    out->printEndLine();
  }

  // [2] print the proof body
  printProofInternal(out, pn, lbind, pletMap, passumeMap);

  // print the closing parenthesis
  out->printCloseRule(cparen);
}

void LfscPrinter::printProofInternal(
    LfscPrintChannel* out,
    const ProofNode* pn,
    const LetBinding& lbind,
    const std::map<const ProofNode*, size_t>& pletMap,
    std::map<Node, size_t>& passumeMap)
{
  // the stack
  std::vector<PExpr> visit;
  // whether we have to process children
  std::unordered_set<const ProofNode*> processingChildren;
  // helper iterators
  std::unordered_set<const ProofNode*>::iterator pit;
  std::map<const ProofNode*, size_t>::const_iterator pletIt;
  std::map<Node, size_t>::iterator passumeIt;
  Node curn;
  TypeNode curtn;
  const ProofNode* cur;
  visit.push_back(PExpr(pn));
  do
  {
    curn = visit.back().d_node;
    curtn = visit.back().d_typeNode;
    cur = visit.back().d_pnode;
    visit.pop_back();
    // case 1: printing a proof
    if (cur != nullptr)
    {
      PfRule r = cur->getRule();
      // maybe it is letified
      pletIt = pletMap.find(cur);
      if (pletIt != pletMap.end())
      {
        // a letified proof
        out->printProofId(pletIt->second);
        continue;
      }
      pit = processingChildren.find(cur);
      if (pit == processingChildren.end())
      {
        bool isLambda = false;
        if (r == PfRule::LFSC_RULE)
        {
          Assert(!cur->getArguments().empty());
          LfscRule lr = getLfscRule(cur->getArguments()[0]);
          isLambda = (lr == LfscRule::LAMBDA);
        }
        else if (r == PfRule::ASSUME)
        {
          // an assumption, must have a name
          passumeIt = passumeMap.find(cur->getResult());
          Assert(passumeIt != passumeMap.end());
          out->printAssumeId(passumeIt->second);
        }
        if (isLambda)
        {
          Assert(cur->getArguments().size() == 3);
          // lambdas are handled specially. We print in a self contained way
          // here.
          // allocate an assumption, if necessary
          size_t pid;
          Node assumption = cur->getArguments()[2];
          passumeIt = passumeMap.find(assumption);
          if (passumeIt == passumeMap.end())
          {
            pid = d_assumpCounter;
            d_assumpCounter++;
            passumeMap[assumption] = pid;
          }
          else
          {
            pid = passumeIt->second;
          }
          // make the node whose name is the assumption id, where notice that
          // the type of this node does not matter
          std::stringstream pidNodeName;
          LfscPrintChannelOut::printAssumeId(pidNodeName, pid);
          // must be an internal symbol so that it is not turned into (bvar ...)
          Node pidNode =
              d_tproc.mkInternalSymbol(pidNodeName.str(), d_boolType);
          // print "(\ "
          out->printOpenRule(cur);
          // print the identifier
          out->printNode(pidNode);
          // Print the body of the proof with a fresh proof letification. We can
          // keep the assumption map and the let binding (for terms).
          std::vector<const ProofNode*> pletListNested;
          std::map<const ProofNode*, size_t> pletMapNested;
          const ProofNode* curBody = cur->getChildren()[0].get();
          computeProofLetification(curBody, pletListNested, pletMapNested);
          printProofLetify(
              out, curBody, lbind, pletListNested, pletMapNested, passumeMap);
          // print ")"
          out->printCloseRule();
        }
        else
        {
          // assert that we should traverse cur when letifying
          Assert(d_lpltc.shouldTraverse(cur));
          // a normal rule application, compute the proof arguments, which
          // notice in the case of PI also may modify our passumeMap.
          std::vector<PExpr> args;
          if (computeProofArgs(cur, args))
          {
            processingChildren.insert(cur);
            // will revisit this proof node to close parentheses
            visit.push_back(PExpr(cur));
            std::reverse(args.begin(), args.end());
            visit.insert(visit.end(), args.begin(), args.end());
            // print the rule name
            out->printOpenRule(cur);
          }
          else
          {
            // could not print the rule, trust for now
            Node res = d_tproc.convert(cur->getResult());
            res = lbind.convert(res, "__t", true);
            out->printTrust(res, r);
            if (d_trustWarned.find(r) == d_trustWarned.end())
            {
              d_trustWarned.insert(r);
            }
          }
        }
      }
      else
      {
        processingChildren.erase(cur);
        out->printCloseRule();
      }
    }
    // case 2: printing a node
    else if (!curn.isNull())
    {
      // it has already been converted to internal form, we letify it here
      Node curni = lbind.convert(curn, "__t", true);
      out->printNode(curni);
    }
    // case 3: printing a type node
    else if (!curtn.isNull())
    {
      out->printTypeNode(curtn);
    }
    // case 4: a hole
    else
    {
      out->printHole();
    }
  } while (!visit.empty());
}

bool LfscPrinter::computeProofArgs(const ProofNode* pn,
                                   std::vector<PExpr>& pargs)
{
  const std::vector<std::shared_ptr<ProofNode>>& children = pn->getChildren();
  std::vector<const ProofNode*> cs;
  for (const std::shared_ptr<ProofNode>& c : children)
  {
    cs.push_back(c.get());
  }
  PfRule r = pn->getRule();
  const std::vector<Node>& args = pn->getArguments();
  std::vector<Node> as;
  for (const Node& a : args)
  {
    Node ac = d_tproc.convert(a);
    Assert(!ac.isNull());
    as.push_back(ac);
  }
  // The proof expression stream, which packs the next expressions (proofs,
  // terms, sorts, LFSC datatypes) into a print-expression vector pargs. This
  // stream can be used via "pf << e" which appends an expression to the
  // vector maintained by this stream.
  PExprStream pf(pargs, d_tt, d_ff);
  // hole
  PExpr h;
  Trace("lfsc-print-debug2")
      << "Compute proof args " << r << " #children= " << cs.size()
      << " #args=" << as.size() << std::endl;
  switch (r)
  {
    // SAT
    case PfRule::RESOLUTION:
      pf << h << h << h << cs[0] << cs[1] << args[0].getConst<bool>() << as[1];
      break;
    case PfRule::REORDERING: pf << h << as[0] << cs[0]; break;
    case PfRule::FACTORING: pf << h << h << cs[0]; break;
    // Boolean
    case PfRule::SPLIT: pf << as[0]; break;
    case PfRule::NOT_NOT_ELIM: pf << h << cs[0]; break;
    case PfRule::CONTRA: pf << h << cs[0] << cs[1]; break;
    case PfRule::MODUS_PONENS:
    case PfRule::EQ_RESOLVE: pf << h << h << cs[0] << cs[1]; break;
    case PfRule::NOT_AND: pf << h << h << cs[0]; break;
    case PfRule::NOT_OR_ELIM:
    case PfRule::AND_ELIM: pf << h << h << args[0] << cs[0]; break;
    case PfRule::IMPLIES_ELIM:
    case PfRule::NOT_IMPLIES_ELIM1:
    case PfRule::NOT_IMPLIES_ELIM2:
    case PfRule::EQUIV_ELIM1:
    case PfRule::EQUIV_ELIM2:
    case PfRule::NOT_EQUIV_ELIM1:
    case PfRule::NOT_EQUIV_ELIM2:
    case PfRule::XOR_ELIM1:
    case PfRule::XOR_ELIM2:
    case PfRule::NOT_XOR_ELIM1:
    case PfRule::NOT_XOR_ELIM2: pf << h << h << cs[0]; break;
    case PfRule::ITE_ELIM1:
    case PfRule::ITE_ELIM2:
    case PfRule::NOT_ITE_ELIM1:
    case PfRule::NOT_ITE_ELIM2: pf << h << h << h << cs[0]; break;
    // CNF
    case PfRule::CNF_AND_POS:
    case PfRule::CNF_OR_NEG:
      // print second argument as a raw integer (mpz)
      pf << h << as[0] << args[1];
      break;
    case PfRule::CNF_AND_NEG: pf << h << as[0]; break;
    case PfRule::CNF_OR_POS:
      pf << as[0];
      break;
      break;
    case PfRule::CNF_IMPLIES_POS:
    case PfRule::CNF_IMPLIES_NEG1:
    case PfRule::CNF_IMPLIES_NEG2:
    case PfRule::CNF_EQUIV_POS1:
    case PfRule::CNF_EQUIV_POS2:
    case PfRule::CNF_EQUIV_NEG1:
    case PfRule::CNF_EQUIV_NEG2:
    case PfRule::CNF_XOR_POS1:
    case PfRule::CNF_XOR_POS2:
    case PfRule::CNF_XOR_NEG1:
    case PfRule::CNF_XOR_NEG2: pf << as[0][0] << as[0][1]; break;
    case PfRule::CNF_ITE_POS1:
    case PfRule::CNF_ITE_POS2:
    case PfRule::CNF_ITE_POS3:
    case PfRule::CNF_ITE_NEG1:
    case PfRule::CNF_ITE_NEG2:
    case PfRule::CNF_ITE_NEG3: pf << as[0][0] << as[0][1] << as[0][2]; break;
    // equality
    case PfRule::REFL: pf << as[0]; break;
    case PfRule::SYMM: pf << h << h << cs[0]; break;
    case PfRule::TRANS: pf << h << h << h << cs[0] << cs[1]; break;
    case PfRule::TRUE_INTRO:
    case PfRule::FALSE_INTRO:
    case PfRule::TRUE_ELIM:
    case PfRule::FALSE_ELIM: pf << h << cs[0]; break;
    // arithmetic
    case PfRule::ARITH_MULT_POS:
    case PfRule::ARITH_MULT_NEG:
    {
      pf << h << as[0] << as[1];
    }
    break;
    case PfRule::ARITH_TRICHOTOMY:
    {
      // should be robust to different orderings
      pf << h << h << h << cs[0] << cs[1];
    }
    break;
    // strings
    case PfRule::STRING_LENGTH_POS: pf << as[0]; break;
    case PfRule::STRING_LENGTH_NON_EMPTY: pf << h << cs[0]; break;
    case PfRule::RE_INTER: pf << h << h << h << cs[0] << cs[1]; break;
    case PfRule::CONCAT_EQ:
      pf << h << h << h << args[0].getConst<bool>() << cs[0];
      break;
    case PfRule::CONCAT_UNIFY:
      pf << h << h << h << h << args[0].getConst<bool>() << cs[0] << cs[1];
      break;
    case PfRule::CONCAT_CSPLIT:
      pf << h << h << h << h << args[0].getConst<bool>() << cs[0] << cs[1];
      break;
      break;
    case PfRule::RE_UNFOLD_POS:
      if (children[0]->getResult()[1].getKind() != REGEXP_CONCAT)
      {
        return false;
      }
      pf << h << h << h << cs[0];
      break;
    case PfRule::STRING_EAGER_REDUCTION:
    {
      Kind k = as[0].getKind();
      if (k == STRING_TO_CODE || k == STRING_CONTAINS || k == STRING_INDEXOF)
      {
        pf << h << as[0] << as[0][0].getType();
      }
      else
      {
        // not yet defined for other kinds
        return false;
      }
    }
    break;
    case PfRule::STRING_REDUCTION:
    {
      Kind k = as[0].getKind();
      if (k == STRING_SUBSTR || k == STRING_INDEXOF)
      {
        pf << h << as[0] << as[0][0].getType();
      }
      else
      {
        // not yet defined for other kinds
        return false;
      }
    }
    break;
    // quantifiers
    case PfRule::SKOLEM_INTRO:
    {
      pf << h << d_tproc.convert(SkolemManager::getOriginalForm(args[0]));
    }
    break;
    // ---------- arguments of non-translated rules go here
    case PfRule::LFSC_RULE:
    {
      LfscRule lr = getLfscRule(args[0]);
      // lambda should be processed elsewhere
      Assert(lr != LfscRule::LAMBDA);
      // Note that `args` has 2 builtin arguments, thus the first real argument
      // begins at index 2
      switch (lr)
      {
        case LfscRule::SCOPE: pf << h << as[2] << cs[0]; break;
        case LfscRule::NEG_SYMM: pf << h << h << cs[0]; break;
        case LfscRule::CONG: pf << h << h << h << h << cs[0] << cs[1]; break;
        case LfscRule::AND_INTRO1: pf << h << cs[0]; break;
        case LfscRule::NOT_AND_REV: pf << h << h << cs[0]; break;
        case LfscRule::PROCESS_SCOPE: pf << h << h << as[2] << cs[0]; break;
        case LfscRule::AND_INTRO2: pf << h << h << cs[0] << cs[1]; break;
        case LfscRule::ARITH_SUM_UB: pf << h << h << h << cs[0] << cs[1]; break;
        default: return false; break;
      }
    }
    break;
    default:
    {
      return false;
      break;
    }
  }
  return true;
}

void LfscPrinter::computeProofLetification(
    const ProofNode* pn,
    std::vector<const ProofNode*>& pletList,
    std::map<const ProofNode*, size_t>& pletMap)
{
  // use callback to specify to stop at LAMBDA
  ProofLetify::computeProofLet(pn, pletList, pletMap, 2, &d_lpltc);
}

void LfscPrinter::print(std::ostream& out, Node n)
{
  Node ni = d_tproc.convert(n);
  printLetify(out, ni);
}

void LfscPrinter::printLetify(std::ostream& out, Node n)
{
  // closing parentheses
  std::stringstream cparen;

  LetBinding lbind;
  lbind.process(n);

  // [1] print the letification
  printLetList(out, cparen, lbind);

  // [2] print the body
  printInternal(out, n, lbind);

  out << cparen.str();
}

void LfscPrinter::printLetList(std::ostream& out,
                               std::ostream& cparen,
                               LetBinding& lbind)
{
  std::vector<Node> letList;
  lbind.letify(letList);
  std::map<Node, size_t>::const_iterator it;
  for (size_t i = 0, nlets = letList.size(); i < nlets; i++)
  {
    Node nl = letList[i];
    out << "(@ ";
    size_t id = lbind.getId(nl);
    Assert(id != 0);
    LfscPrintChannelOut::printId(out, id);
    out << " ";
    // remove, print, insert again
    printInternal(out, nl, lbind, false);
    out << std::endl;
    cparen << ")";
  }
}

void LfscPrinter::printInternal(std::ostream& out, Node n)
{
  LetBinding lbind;
  printInternal(out, n, lbind);
}
void LfscPrinter::printInternal(std::ostream& out,
                                Node n,
                                LetBinding& lbind,
                                bool letTop)
{
  Node nc = lbind.convert(n, "__t", letTop);
  LfscPrintChannelOut::printNodeInternal(out, nc);
}

void LfscPrinter::printType(std::ostream& out, TypeNode tn)
{
  TypeNode tni = d_tproc.convertType(tn);
  LfscPrintChannelOut::printTypeNodeInternal(out, tni);
}

}  // namespace proof
}  // namespace cvc5
