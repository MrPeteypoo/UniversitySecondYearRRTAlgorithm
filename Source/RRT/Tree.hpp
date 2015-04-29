#ifndef GEC_TREE_HPP
#define GEC_TREE_HPP


// STL headers.
#include <cassert>
#include <memory>
#include <vector>


/// <summary>
/// A tree data structure containing a parent and an unlimited number of children.
/// </summary>
template <typename T> class Tree final
{
    public:
        
        ///////////// 
        // Aliases //
        /////////////

        /// <summary> The parent of the current Tree/Branch object. </summary>
        using Parent = Tree<T>*;

        /// <summary> A child branch of a Tree object. </summary>
        using Branch = Tree<T>*;

        /// <summary> A collection of Branch objects which branch off from the Tree object. </summary>
        using Branches = std::vector<Branch>;


        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Construct a tree with a reserved branch capacity. </summary>
        /// <param name="reserve"> How much capacity to reserve. </param>
        Tree (const unsigned int reserve = 0U);
        
        /// <summary> 
        /// Create a copy of a Tree, this copies in the same sense that a vector copies all dynamic memory. This cannot
        /// copy the parent node so the current point in the tree will become a root.
        /// </summary>
        Tree (const Tree<T>& copy);
        Tree& operator= (const Tree<T>& copy);

        Tree (Tree<T>&& move);
        Tree& operator= (Tree<T>&& move);
        ~Tree();


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Checks if the Tree is a root node. </summary>
        bool isRoot() const                                         { return m_parent == nullptr; }

        /// <summary> Checks if the Tree has any children. </summary>
        bool isTip() const                                          { return m_branches.empty(); }

        /// <summary> Gets the data associated with this node. </summary>
        /// <returns> A read-only reference to the data. </returns>
        const T& getData() const                                    { return m_data; }

        /// <summary> Gets the data associated with this node. </summary>
        /// <returns> A modifiable reference to the data. </returns>
        T& getData()                                                { return m_data; }

        /// <summary> Short hand for Tree::getBranch(). </summary>
        Branch operator[] (const unsigned int index) const          { return getBranch (index); }

        /// <summary> Gets the total number of Branch objects for the Tree. </summary>
        unsigned int getBranchCount() const                         { return m_branches.size(); }

        /// <summary> Gets the parent of the current Tree branch. </summary>
        const Parent& getParent() const                             { return m_parent; }

        /// <summary> Gets the collection of Branch objects tracked by the Tree. </summary>
        /// <returns> A read-only reference. </returns>
        const Branches& getBranches() const                         { return m_branches; }

        /// <summary> Gets the collection of Branch objects tracked by the Tree. </summary>
        /// <returns> A modifiable reference. </returns>
        Branches& getBranches()                                     { return m_branches; }

        /// <summary> Gets the branch at the current index. </summary>
        Branch getBranch (const unsigned int index) const;

        /// <summary> Sets the data associated with this node. </summary>
        void setData (const T& data)                                { m_data = data; }

        /// <summary> Sets the data associated with this node using move semantics. </summary>
        void setData (T&& move)                                     { m_data = std::move (data); }

        /// <summary> Sets the Branch at the desired index. </summary>
        /// <param name="index"> The index to set the Branch to. </param>
        /// <param name="branch"> The branch to use. </param>
        void setBranch (const unsigned int index, const Branch branch);


        //////////////////////////
        // Addition and removal //
        //////////////////////////

        /// <summary> Clears the Branches object of all data. </summary>
        void clear()    { m_branches.clear(); }

        /// <summary> Adds a Branch to the end of the collection of Branch objects. </summary>
        /// <param name="branch"> The Branch object to add. </param>
        void addBranch (const Branch branch);

        /// <summary> Adds a Branch at the specified index, this can be very slow. </summary>
        /// <param name="index"> The index to add the branch at. </param>
        /// <param name="branch"> The branch to add. </param>
        void addBranch (const unsigned int index, const Branch branch);

        /// <summary> Removes a Branch from the Branches object, this is fast but changes the order. </summary>
        /// <param name="index"> The index of the Branch to remove. </param>
        void removeBranch (const unsigned int index);

        /// <summary> Removes a Branch from the Branches object, this is fast but changes the order. </summary>
        /// <param name="branch"> The Branch to remove. </param>
        void removeBranch (const Branch branch);

        /// <summary> Removes a Branch from the Branches object whilst maintaining the order. </summary>
        /// <param name="index"> The index of the Branch to remove. </param>
        void removeBranchOrdered (const unsigned int index);

        /// <summary> Removes a Branch from the Branches object whilst maintaining the order. </summary>
        /// <param name="branch"> The Branch to remove. </param>
        void removeBranchOrdered (const Branch branch);


        ///////////////
        // Utilities //
        ///////////////

        /// <summary> Finds the index of the given branch. </summary>
        /// <param name="branch"> The branch to look for. </param>
        /// <returns> The index, Tree::getBranchCount() if it does not exist. </returns>
        unsigned int findIndex (const Branch branch) const;

    private:


        ///////////////////
        // Internal data //
        ///////////////////

        T           m_data      { };    //!< The data of the node.
        Parent      m_parent    { };    //!< The parent of the current branch of a Tree.
        Branches    m_branches  { };    //!< The child branches of the current branch of a Tree.
};


/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

template <typename T>
Tree<T>::Tree (const unsigned int reserve)
{
    m_branches.reserve (reserve);
}


template <typename T>
Tree<T>::Tree (const Tree& copy)
{
    *this = copy;
}


template <typename T>
Tree<T>::Tree (Tree&& move)
{
    *this = std::move (move);
}


template <typename T>
Tree<T>& Tree<T>::operator= (const Tree<T>& copy)
{
    if (this != &copy)
    {
        branches = copy.m_branches;        
    }

    return *this;
}


template <typename T>
Tree<T>& Tree<T>::operator= (Tree<T>&& move)
{
    if (this != &move)
    {
        m_parent   = move.m_parent;
        m_branches = move.m_branches;

        move.m_parent   = nullptr;
        move.m_branches = nullptr;
    }

    return *this;
}


template <typename T>
Tree<T>::~Tree()
{
    for (auto& branch : m_branches)
    {
        if (branch)
        {
            delete branch;
        }

        branch = nullptr;
    }
}


/////////////////////////
// Getters and setters //
/////////////////////////

template <typename T>
typename Tree<T>::Branch Tree<T>::getBranch (const unsigned int index) const
{
    // Pre-condition: Index is in range.
    assert (index < getBranchCount());

    return m_branches[index];
}


template <typename T>
void Tree<T>::setBranch (const unsigned int index, const Branch branch)
{
    // Pre-condition: Index is in range.
    assert (index < getBranchCount());

    return m_branches[index] = branch;
}


////////////////////////////
// Additional and removal //
////////////////////////////

template <typename T>
void Tree<T>::addBranch (const Branch branch)
{
    m_branches.push_back (branch);
    m_branches.back()->m_parent = this;
}

template <typename T>
void Tree<T>::addBranch (const unsigned int index, const Branch branch)
{
    // Pre-condition: Index is in range of the new size.
    assert (index <= getBranchCount());

    m_branches.insert (m_branches.cbegin() + index, branch);
    m_branches[index]->m_parent = this;
}


template <typename T>
void Tree<T>::removeBranch (const unsigned int index)
{
    // Pre-condition: Index is in range.
    assert (index < getBranchCount());

    // Swap and pop!
    std::swap (m_branches[index], m_branches.back());

    delete m_branches.back();
    m_branches.pop_back();
}


template <typename T>
void Tree<T>::removeBranch (const Branch branch)
{
    removeBranch (findIndex (branch));
}


template <typename T>
void Tree<T>::removeBranchOrdered (const unsigned int index)
{
    // Pre-condition: Index is in range.
    assert (index < getBranchCount());

    delete m_branches[index];
    m_branches.erase (m_branches.cbegin() + index);
}


template <typename T>
void Tree<T>::removeBranchOrdered (const Branch branch)
{    
    removeBranchOrdered (findIndex (branch));
}


/////////////
// Utility //
/////////////

template <typename T>
unsigned int Tree<T>::findIndex (const Branch branch) const
{
    // Keep track of this baby.
    auto i = 0U;

    // Loop through the branches to find the correct index.
    for (; i < m_branches.size(); ++i)
    {
        if (m_branches[i] == branch)
        {
            break;
        }
    }

    return i;
}


#endif