#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T value;
        int dist;
        Node* left;
        Node* right;

        Node(const T& val) : value(val), dist(0), left(nullptr), right(nullptr) {}
        Node(const Node& node) : value(node.value), dist(node.dist), left(nullptr), right(nullptr) {
            if (node.left) left = new Node(*node.left);
            if (node.right) right = new Node(*node.right);
        }
    };

    Node* root;
    Compare cmp;
    size_t count;

    // Helper function to merge two heaps
    Node* merge_nodes(Node* h1, Node* h2) {
        // Handle null cases
        if (!h1) return h2;
        if (!h2) return h1;

        // Make sure h1 has higher priority (larger value for max heap)
        try {
            if (cmp(h1->value, h2->value)) {
                std::swap(h1, h2);
            }
        } catch (...) {
            throw;
        }

        // Save h1's right subtree
        Node* h1_right = h1->right;
        h1->right = nullptr;

        // Merge h2 with h1's right subtree
        Node* merged_right = nullptr;
        try {
            merged_right = merge_nodes(h1_right, h2);
        } catch (...) {
            // Restore h1's right subtree and rethrow
            h1->right = h1_right;
            throw;
        }

        // Swap children if needed to maintain leftist property
        try {
            if (!h1->left) {
                // If no left child, make merged_right the left child
                h1->left = merged_right;
            } else if (!merged_right) {
                // If no merged_right, keep left child as is
                h1->right = merged_right;
            } else if (h1->left->dist < merged_right->dist) {
                // If left child has smaller dist, swap
                h1->right = h1->left;
                h1->left = merged_right;
            } else {
                // Otherwise, keep left child and set merged_right as right
                h1->right = merged_right;
            }
        } catch (...) {
            // If exception occurs during swap, we can't easily restore
            // But the tree structure is still valid, just not optimal
            // We'll let the exception propagate
            throw;
        }

        // Update dist
        try {
            h1->dist = h1->right ? h1->right->dist + 1 : 1;
        } catch (...) {
            throw;
        }
        
        return h1;
    }

    // Copy a tree
    Node* copy_tree(Node* node) {
        if (!node) return nullptr;
        
        Node* new_node = nullptr;
        try {
            new_node = new Node(node->value);
            new_node->dist = node->dist;
        } catch (...) {
            delete new_node;
            throw;
        }
        
        Node* left_child = nullptr;
        Node* right_child = nullptr;
        
        try {
            if (node->left) left_child = copy_tree(node->left);
            if (node->right) right_child = copy_tree(node->right);
        } catch (...) {
            // Clean up any allocated children
            if (left_child) clear_tree(left_child);
            if (right_child) clear_tree(right_child);
            delete new_node;
            throw;
        }
        
        // Assign children
        new_node->left = left_child;
        new_node->right = right_child;
        
        return new_node;
    }

    // Clear a tree
    void clear_tree(Node* node) {
        if (!node) return;
        clear_tree(node->left);
        clear_tree(node->right);
        delete node;
    }

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), cmp(Compare()), count(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : cmp(other.cmp), count(other.count), root(nullptr) {
        if (other.root) {
            root = copy_tree(other.root);
        }
    }

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
        clear_tree(root);
    }

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;
        
        // Save current state
        Node* old_root = root;
        Compare old_cmp = cmp;
        size_t old_count = count;
        
        // Try to copy from other
        try {
            // Clear current tree
            clear_tree(root);
            
            // Copy from other
            cmp = other.cmp;
            count = other.count;
            root = other.root ? copy_tree(other.root) : nullptr;
        } catch (...) {
            // Restore original state
            root = old_root;
            cmp = old_cmp;
            count = old_count;
            throw;
        }
        
        // If we get here, the copy was successful
        // Delete the old tree
        if (old_root) {
            clear_tree(old_root);
        }
        
        return *this;
    }

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->value;
    }

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
        Node* new_node = nullptr;
        try {
            new_node = new Node(e);
            root = merge_nodes(root, new_node);
            count++;
        } catch (...) {
            // If exception occurs, delete the new node if it was created
            if (new_node) {
                delete new_node;
            }
            throw;
        }
    }

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
        if (empty()) {
            throw container_is_empty();
        }
        
        Node* old_root = root;
        Node* left_subtree = root->left;
        Node* right_subtree = root->right;
        
        // Detach children to prevent double deletion
        root->left = nullptr;
        root->right = nullptr;
        
        try {
            root = merge_nodes(left_subtree, right_subtree);
            count--;
        } catch (...) {
            // Restore the original root if merge fails
            // But don't restore children since they might have been modified
            root = old_root;
            root->left = left_subtree;
            root->right = right_subtree;
            throw;
        }
        
        delete old_root;
    }

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
        return count;
    }

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
        return root == nullptr;
    }

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
        if (this == &other) return;
        
        // Save current state in case of exception
        Node* old_root = root;
        size_t old_count = count;
        
        try {
            root = merge_nodes(root, other.root);
            count += other.count;
            
            // Clear the other queue
            other.root = nullptr;
            other.count = 0;
        } catch (...) {
            // Restore original state
            root = old_root;
            count = old_count;
            throw;
        }
    }
};

}

#endif