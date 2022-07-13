package com.test.lsp.tree2;

public class TreeNode2 {
    public int val;

    public TreeNode2 left;
    public TreeNode2 right;

    public TreeNode2(int x) {
        this.val = x;
    }

    public TreeNode2(int val, TreeNode2 left, TreeNode2 right) {
        this.val = val;
        this.left = left;
        this.right = right;
    }
}