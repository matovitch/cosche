#ifndef __TOPOSORT_H__
#define __TOPOSORT_H__

#include <unordered_set>
#include <functional>
#include <memory>
#include <vector>

#include "RobinHoodHashtable.hpp"

template <typename N, typename H>
struct NodePtrHasher
{
    std::size_t operator()(const N& node) const
    {
        return H()(node->_t);
    }
};

template <typename N, typename H, typename E = std::equal_to<N>>
using NodePtrSet = RobinHoodHashtable<N, NodePtrHasher<N, H>, E>;

template <typename T, typename H>
struct Node
{
    Node(const T& t) : _t(t) {}

    T _t;

    NodePtrSet<Node<T, H>*, H> _ins;
    NodePtrSet<Node<T, H>*, H> _outs;
};

template <typename T, typename H>
struct NodePtrEquals
{
    bool operator() (const std::unique_ptr<Node<T, H>>& lhs, 
                     const std::unique_ptr<Node<T, H>>& rhs) const
    {
        return lhs->_t == rhs->_t;
    }
};

template <typename T, typename H = std::hash<T>>
struct Toposort
{

    Toposort() { clear(); }

    void clear()
    {
        _heap.clear();
        _pendings.clear();
        _blockeds.clear();
    }

    void push(const T& t)
    {
        std::unique_ptr<Node<T, H>> node(std::make_unique<Node<T, H>>(T(t)));
        _pendings.insert(&(*node));
        _heap.insert(std::move(node));
    }

    void push(T&& t)
    {
        std::unique_ptr<Node<T, H>> node(std::make_unique<Node<T, H>>(std::move(t)));
        _pendings.insert(&(*node));
        _heap.insert(std::move(node));
    }

    void detach(const T& lhs,
                const T& rhs)
    {
        auto lfit = _heap.find(std::make_unique<Node<T, H>>(lhs));
        auto rfit = _heap.find(std::make_unique<Node<T, H>>(rhs));

        if (lfit != _heap.end() &&
            rfit != _heap.end())
        {
            Node<T, H>* lhsNode = &(*(*lfit));
            Node<T, H>* rhsNode = &(*(*rfit));

            lhsNode->_ins.erase(rhsNode);
            rhsNode->_outs.erase(lhsNode);

            if (lhsNode->_ins.empty() &&
                _waitings.find(lhsNode) == _waitings.end())
            {
                _blockeds.erase(lhsNode);
                _pendings.insert(lhsNode);
            }
        }
    }

    void attach(const T& lhs,
                const T& rhs)
    {
        auto lfit = _heap.find(std::make_unique<Node<T, H>>(lhs));
        auto rfit = _heap.find(std::make_unique<Node<T, H>>(rhs));

        if (lfit != _heap.end() &&
            rfit != _heap.end())
        {
            Node<T, H>* lhsNode = &(*(*lfit));
            Node<T, H>* rhsNode = &(*(*rfit));

            if (lhsNode->_ins.empty() &&
                _waitings.find(lhsNode) == _waitings.end())
            {
                _blockeds.insert(lhsNode);
                _pendings.erase(lhsNode);
            }

            lhsNode->_ins.insert(rhsNode);
            rhsNode->_outs.insert(lhsNode);
        }
    }

    void erase(const T& t)
    {
        auto fit = _heap.find(std::make_unique<Node<T, H>>(t));

        if (fit != _heap.end())
        {
            Node<T, H>* const top = &(*(*fit));

            for (Node<T, H>* const out : top->_outs)
            {
                out->_ins.erase(top);

                if (out->_ins.empty())
                {
                    _pendings.insert(out);
                    _blockeds.erase(out);
                }
            }

            for (Node<T, H>* const in : top->_ins)
            {
                in->_outs.erase(top);
            }

            _pendings.erase(top);
            _blockeds.erase(top);
            _heap.erase(std::make_unique<Node<T, H>>(*top));
        }
    }

    void halt(const T& t)
    {
        auto fit = _heap.find(std::make_unique<Node<T, H>>(t));

        if (fit != _heap.end())
        {
            Node<T, H>* tNode = &(*(*fit));

            _waitings.insert(tNode);

            if (tNode->_ins.empty())
            {
                _blockeds.insert(tNode);
                _pendings.erase(tNode);
            }
        }
    }

    void wake(const T& t)
    {
        auto fit = _heap.find(std::make_unique<Node<T, H>>(t));

        if (fit != _heap.end())
        {
            Node<T, H>* tNode = &(*(*fit));

            _waitings.erase(tNode);

            if (tNode->_ins.empty())
            {
                _blockeds.erase(tNode);
                _pendings.insert(tNode);
            }
        }
    }

          T&  top()       { return (*(_pendings.begin()))->_t; }
    const T& ctop() const { return (*(_pendings.begin()))->_t; }

    bool empty() const { return _pendings.empty(); }

    bool cyclic() const
    {
        return empty() && !_blockeds.empty();
    }

    const std::vector<T>& cycle()
    {
        _cycle.clear();

        if (cyclic())
        {
            Node<T, H>* node_1 = *(_blockeds.begin());
            NodePtrSet<Node<T, H>*, H> visiteds;

            while (visiteds.find(node_1) == visiteds.end())
            {
                visiteds.insert(node_1);
                node_1 = *(node_1->_ins.begin());
            }

            Node<T, H>* node_2 = node_1;

            do
            {
                _cycle.push_back(node_2->_t);
                node_2 = *(node_2->_ins.begin());
            } while (node_2 != node_1);
        }

        return _cycle;
    }

    NodePtrSet<Node<T, H>*, H> _pendings;
    NodePtrSet<Node<T, H>*, H> _blockeds;
    NodePtrSet<Node<T, H>*, H> _waitings;

    NodePtrSet<std::unique_ptr<Node<T, H>>, H, NodePtrEquals<T, H>> _heap;

    std::vector<T> _cycle;
};

#endif // end __TOPOSORT_H__
