/*********************                                                        */
/*! \file lfsc_term_process.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of rewrite db term processor.
 **/

#include "proof/lfsc/lfsc_term_process.h"

#include "expr/attribute.h"

using namespace CVC4::kind;

namespace CVC4 {
namespace proof {

struct LfscInternalAttributeId
{
};
typedef expr::Attribute<LfscInternalAttributeId, Node> LfscInternalAttribute;

struct LfscExternalAttributeId
{
};
typedef expr::Attribute<LfscExternalAttributeId, Node> LfscExternalAttribute;

Node LfscTermProcess::toInternal(Node n) { return convert(n, true); }
Node LfscTermProcess::toExternal(Node n) { return convert(n, false); }

Node LfscTermProcess::convert(Node n, bool toInternal)
{
  if (n.isNull())
  {
    return n;
  }
  Trace("rdtp-debug") << "LfscTermProcess::convert: " << toInternal << " " << n
                      << std::endl;
  LfscInternalAttribute ria;
  LfscExternalAttribute rea;
  NodeManager* nm = NodeManager::currentNM();
  std::unordered_map<TNode, Node, TNodeHashFunction> visited;
  std::unordered_map<TNode, Node, TNodeHashFunction>::iterator it;
  std::vector<TNode> visit;
  TNode cur;
  visit.push_back(n);
  do
  {
    cur = visit.back();
    visit.pop_back();
    it = visited.find(cur);

    if (it == visited.end())
    {
      if (toInternal && cur.hasAttribute(ria))
      {
        visited[cur] = cur.getAttribute(ria);
      }
      else if (!toInternal && cur.hasAttribute(rea))
      {
        visited[cur] = cur.getAttribute(rea);
      }
      else
      {
        visited[cur] = Node::null();
        visit.push_back(cur);
        if (cur.getMetaKind() == metakind::PARAMETERIZED)
        {
          visit.push_back(cur.getOperator());
        }
        visit.insert(visit.end(), cur.begin(), cur.end());
      }
    }
    else if (it->second.isNull())
    {
      Node ret = cur;
      bool childChanged = false;
      std::vector<Node> children;
      if (cur.getMetaKind() == metakind::PARAMETERIZED)
      {
        it = visited.find(cur.getOperator());
        Assert(it != visited.end());
        Assert(!it->second.isNull());
        childChanged = childChanged || cur.getOperator() != it->second;
        children.push_back(it->second);
      }
      for (const Node& cn : cur)
      {
        it = visited.find(cn);
        Assert(it != visited.end());
        Assert(!it->second.isNull());
        childChanged = childChanged || cn != it->second;
        children.push_back(it->second);
      }
      if (childChanged)
      {
        ret = nm->mkNode(cur.getKind(), children);
      }
      if (toInternal)
      {
        ret = computeInternal(ret);
        cur.setAttribute(ria, ret);
      }
      else
      {
        ret = computeExternal(ret);
        cur.setAttribute(rea, ret);
      }
      visited[cur] = ret;
    }
  } while (!visit.empty());
  Assert(visited.find(n) != visited.end());
  Assert(!visited.find(n)->second.isNull());
  return visited[n];
}

Node LfscTermProcess::computeInternal(Node n)
{
  Kind ck = n.getKind();
  if (ck == CONST_STRING)
  {
    NodeManager* nm = NodeManager::currentNM();
    // "ABC" is (str.++ "A" (str.++ "B" "C"))
    const std::vector<unsigned>& vec = n.getConst<String>().getVec();
    if (vec.size() <= 1)
    {
      return n;
    }
    std::vector<unsigned> v(vec.begin(), vec.end());
    std::reverse(v.begin(), v.end());
    std::vector<unsigned> tmp;
    tmp.push_back(v[0]);
    Node ret = nm->mkConst(String(tmp));
    tmp.pop_back();
    for (unsigned i = 1, size = v.size(); i < size; i++)
    {
      tmp.push_back(v[i]);
      ret = nm->mkNode(STRING_CONCAT, nm->mkConst(String(tmp)), ret);
      tmp.pop_back();
    }
    return ret;
  }
  else if (ExprManager::isNAryKind(ck) && n.getNumChildren() >= 2)
  {
    NodeManager* nm = NodeManager::currentNM();
    Assert(n.getMetaKind() != kind::metakind::PARAMETERIZED);
    // convert to binary
    std::vector<Node> children(n.begin(), n.end());
    std::reverse(children.begin(), children.end());
    Node ret = children[0];
    for (unsigned i = 1, nchild = n.getNumChildren(); i < nchild; i++)
    {
      ret = nm->mkNode(ck, children[i], ret);
    }
    return ret;
  }
  return n;
}

Node LfscTermProcess::computeExternal(Node n)
{
  Kind ck = n.getKind();
  if (ExprManager::isNAryKind(ck))
  {
    Assert(n.getNumChildren() == 2);
    if (n[1].getKind() == ck)
    {
      // flatten to n-ary
      std::vector<Node> children;
      children.push_back(n[0]);
      children.insert(children.end(), n[1].begin(), n[1].end());
      NodeManager* nm = NodeManager::currentNM();
      return nm->mkNode(ck, children);
    }
    else if (n[1].getKind() == CONST_STRING && n[0].getKind() == CONST_STRING)
    {
      // flatten (non-empty) constants
      const std::vector<unsigned>& v0 = n[0].getConst<String>().getVec();
      const std::vector<unsigned>& v1 = n[1].getConst<String>().getVec();
      if (v0.size() == 1 && !v1.empty())
      {
        std::vector<unsigned> vres;
        vres.push_back(v0[0]);
        vres.insert(vres.end(), v1.begin(), v1.end());
        NodeManager* nm = NodeManager::currentNM();
        return nm->mkConst(String(vres));
      }
    }
  }
  return n;
}
TypeNode LfscTermProcess::toInternalType(TypeNode tn)
{
  return convertType(tn, true);
}
TypeNode LfscTermProcess::toExternalType(TypeNode tn)
{
  return convertType(tn, false);
}

TypeNode LfscTermProcess::convertType(TypeNode tn, bool toInternal)
{
  // TODO
  return tn;
}
TypeNode LfscTermProcess::computeInternalType(TypeNode tn)
{
  // TODO
  return tn;
}
TypeNode LfscTermProcess::computeExternalType(TypeNode tn)
{
  // TODO
  return tn;
}

}  // namespace proof
}  // namespace CVC4
