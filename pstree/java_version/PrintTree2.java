package com.test.lsp.tree2;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.LinkedList;
import java.util.List;

import com.test.lsp.tree1.TreeNode;

public class PrintTree2 {

//************************************水平树**************************************************************** 	

	/**
	 * 打印的思路就是 在打印当前节点时，同是要计算出下一行的前置串
	 * 
	 * 深度优先来遍历处理树节点，每递归调用一层就在用链表追加一层，跳出递归时移除链表末尾的节点
	 * 
	 */
	public static String traversePreOrder(TreeNode root) {
		if (root == null) {
			return "";
		}

		StringBuffer sb = new StringBuffer();
		sb.append(root.val);

		String currentNodePrefix;
		if (root.right != null && root.left != null) {
			currentNodePrefix = "─┬─";
		} else {
			currentNodePrefix = "───";
		}

		String rootString = toBlankString(String.valueOf(root.val));
		LinkedList<String> nextLevelPrefix = new LinkedList<String>();
		nextLevelPrefix.add(rootString);
		// 至少有一个子节点的情况
		traverseNodes(sb, nextLevelPrefix, currentNodePrefix, root.left == null ? root.right : root.left,
				root.right != null && root.left != null);

		if (root.right != null && root.left != null) {
			String currentNodePrefix2 = "";
			currentNodePrefix2 += rootString;
			currentNodePrefix2 += " └─";
			traverseNodes(sb, nextLevelPrefix, currentNodePrefix2, root.right, false);
		}
		return sb.toString();
	}

	private static String toBlankString(String str) {
		if (null == str || "".equals(str)) {
			return "";

		}
		int strLen = str.length();
		StringBuffer blankStr = new StringBuffer();
		for (int i = 0; i < strLen; i++) {
			blankStr.append(" ");
		}
		return blankStr.toString();

	}

	/**
	 * 
	 * @param res
	 * @param padding           前置的 │
	 * @param currentNodePrefix 当前要打印值得前缀
	 * @param node
	 * @param hasRightSibling
	 */
	private static void traverseNodes(StringBuffer res, LinkedList<String> nextLevelPrefix, String currentNodePrefix,
			TreeNode node, boolean hasRightSibling) {
		if (node == null) {
			return;
		}

		res.append(currentNodePrefix);
		res.append(node.val);
		if (node.right == null && node.left == null) {
			// 叶子节点换行
			res.append("\n");
			return;
		}

		currentNodePrefix = (node.right != null && node.left != null) ? "─┬─" : "───";
		if (hasRightSibling) {
			nextLevelPrefix.add(" │ ");
		} else {
			nextLevelPrefix.add("   ");
		}
		String nodeString = toBlankString(String.valueOf(node.val));
		nextLevelPrefix.add(nodeString);

		
		
		// 这里至少有个一个子节点
		// 因为打印的节点与父节点在同一层 所以currentNodePrefix不需要拼接nextLevelPrefix
		traverseNodes(res, nextLevelPrefix, currentNodePrefix, node.left == null ? node.right : node.left,
				node.right != null && node.left != null);
		nextLevelPrefix.removeLast();
		nextLevelPrefix.removeLast();

		
		
		// 判断是否有两个子节点
		if (node.right != null && node.left != null) {
			if (hasRightSibling) {
				nextLevelPrefix.add(" │ ");
			} else {
				nextLevelPrefix.add("   ");
			}
			nextLevelPrefix.add(nodeString);

			// 打印下一层的节点 需要在currentNodePrefix前面拼接上nextLevelPrefix
			currentNodePrefix = "";
			for (String pre : nextLevelPrefix) {
				currentNodePrefix += pre;
			}

			currentNodePrefix += " └─";
			traverseNodes(res, nextLevelPrefix, currentNodePrefix, node.right, false);

			nextLevelPrefix.removeLast();
			nextLevelPrefix.removeLast();
		}
	}

	public static void printTreeHorizontal(TreeNode root) {
		System.out.print(traversePreOrder(root));
	}

//************************************垂直树**************************************************************** 

	public static int getTreeDepth(TreeNode root) {
		if (root == null) {
			return 0;
		}

		return 1 + Math.max(getTreeDepth(root.left), getTreeDepth(root.right));
	}

	public static void printTree(TreeNode root) {
		int maxLevel = getTreeDepth(root);
		printNodeInternal(Collections.singletonList(root), 1, maxLevel);
	}

	private static void printNodeInternal(List<TreeNode> nodes, int level, int maxLevel) {
		if (nodes == null || nodes.isEmpty() || isAllElementsNull(nodes)) {
			return;
		}

		int floor = maxLevel - level;
		int endgeLines = (int) Math.pow(2, (Math.max(floor - 1, 0)));
		int firstSpaces = (int) Math.pow(2, (floor)) - 1;
		int betweenSpaces = (int) Math.pow(2, (floor + 1)) - 1;

		printWhitespaces(firstSpaces);

		List<TreeNode> newNodes = new ArrayList<TreeNode>();
		for (TreeNode node : nodes) {
			if (node != null) {
				System.out.print(node.val);
				newNodes.add(node.left);
				newNodes.add(node.right);
			} else {
				newNodes.add(null);
				newNodes.add(null);
				System.out.print(" ");
			}

			printWhitespaces(betweenSpaces);
		}
		System.out.println("");

		for (int i = 1; i <= endgeLines; i++) {
			for (int j = 0; j < nodes.size(); j++) {
				printWhitespaces(firstSpaces - i);
				if (nodes.get(j) == null) {
					printWhitespaces(endgeLines + endgeLines + i + 1);
					continue;
				}

				if (nodes.get(j).left != null) {
					System.out.print("/");
				} else {
					printWhitespaces(1);
				}

				printWhitespaces(i + i - 1);
				if (nodes.get(j).right != null) {
					System.out.print("\\");
				} else {
					printWhitespaces(1);
				}
				printWhitespaces(endgeLines + endgeLines - i);
			}

			System.out.println("");
		}

		printNodeInternal(newNodes, level + 1, maxLevel);
	}

	private static void printWhitespaces(int count) {
		for (int i = 0; i < count; i++) {
			System.out.print(" ");
		}
	}

	private static <T> boolean isAllElementsNull(List<T> list) {
		for (Object object : list) {
			if (object != null) {
				return false;
			}
		}
		return true;
	}

//************************************构建树****************************************************************

	public static TreeNode constructTree(Integer[] array) {
		if (array == null || array.length == 0 || array[0] == null) {
			return null;
		}

		int index = 0;
		int length = array.length;

		TreeNode root = new TreeNode(array[0]);
		Deque<TreeNode> nodeQueue = new LinkedList<>();
		nodeQueue.offer(root);
		TreeNode currNode;
		while (index < length) {
			index++;
			if (index >= length) {
				return root;
			}
			currNode = nodeQueue.poll();
			Integer leftChild = array[index];
			if (leftChild != null) {
				currNode.left = new TreeNode(leftChild);
				nodeQueue.offer(currNode.left);
			}
			index++;
			if (index >= length) {
				return root;
			}
			Integer rightChild = array[index];
			if (rightChild != null) {
				currNode.right = new TreeNode(rightChild);
				nodeQueue.offer(currNode.right);
			}
		}

		return root;
	}

//************************************测试代码****************************************************************

	public static void main(String[] args) {
//        Integer[] tstData1 = {1, null, 2, 2, 32, 31, 3, 23, 1, 23, 123, 12, 3, 12, 31, 23, 2};
//		Integer[] tstData1 = { 1, 2, 3, 4, 5, 6, 7,8,9,10,11,12,13,14,16,17,18,19,20,21,22,23,24  };
		Integer[] tstData1 = new Integer[1];
		for (int i = 0; i < tstData1.length; i++) {
			tstData1[i] = i + 1;
		}

		TreeNode tstNode1 = constructTree(tstData1);
//        System.out.println("\nTree:");
//        printTree(tstNode1);
//        System.out.println("\nHorizontal Tree:");
		printTreeHorizontal(tstNode1);
//        System.out.println("\nPreOrder:");
//        preOrderPrint(tstNode1);
//		Integer[] tstData2 = { 1, 2, 3213 , null, 5, null, null,null,9,4210,11,null,null,14,null,2510,18,19,20,21,22,23,24  };
		Integer[] tstData2 = { 1, 2, 3, 4, 5, null, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, null, 19, null, 21, 22, 23, 24,
				25, null, 27, null, 29, null, 31 };
//        Integer[] tstData2 = {1, 2, 3, null, 4, 5, 6, 7, null};
		TreeNode tstNode2 = constructTree(tstData2);
		System.out.println("\nTree:");
//		printTree(tstNode2);
		System.out.println("\nHorizontal Tree:");
		printTreeHorizontal(tstNode2);
//        System.out.println("\nPreOrder:");
//        preOrderPrint(tstNode2);
//        System.out.println("\nInOrder:");
//        inOrderPrint(tstNode2);
//        System.out.println("\nPostOrder:");
//        postOrderPrint(tstNode2);
	}
}
