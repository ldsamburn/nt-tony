/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/

package archive.client;

import javax.swing.*;
import java.awt.*;
import javax.swing.border.*;
import java.awt.event.*;
import javax.swing.tree.*;
import javax.swing.event.*;

import archive.common.*;

/**
 * A gui in which a user can select files from a displayed list of files
 */
public class FileSelectionDialog extends JDialog
{
   private JTextField selectionField;
   private JTree fileTree;
   private String rootDir;
   private String fileSeparator;
   private FilesTab parentWindow;
   private String host;
   private DefaultTreeModel treeModel;
   private boolean ignoreExpansionRequests;

   /**
    * Constructor
    */
   public FileSelectionDialog(FilesTab parent, String hst)
   {
      super(FSArchive.getHandle());

      Dimension screenSize;
      int desiredX, desiredY;
      final int windowWidth = 400;
      final int windowHeight = 450;
      JScrollPane scrollPane;
      JButton addButton, closeButton;
      JPanel t1, t2, t3;
      JLabel selectionLabel;
      BorderLayout bl;

      try {
         setCursorType(Cursor.WAIT_CURSOR);

         parentWindow = parent;
         host = hst;
         ignoreExpansionRequests = false;

         // Set the title and whether this dialog is modal or not
         setTitle("File Selection    " + host);
         setModal(true);

         // Place the dialog in the middle of the screen
         setSize(new Dimension(windowWidth, windowHeight));
         screenSize = (Toolkit.getDefaultToolkit()).getScreenSize();
         desiredX = (screenSize.width/2) - (windowWidth/2);
         desiredY = (screenSize.height/2) - (windowHeight/2);
         setLocation(new Point(desiredX, desiredY));

         // Get the root directory and file separator from the specified host
         rootDir = ServerInterface.getRootDirectory(host);
         fileSeparator = ServerInterface.getFileSeparator(host);

         // Create the file tree
         treeModel = new DefaultTreeModel(createInitialBranches());
         fileTree = new JTree(treeModel);
         fileTree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.SINGLE_TREE_SELECTION);

         // Create the listener which will listen for the user to select
         //  a file
         fileTree.addTreeSelectionListener(new TreeSelectionListener() {
            public void valueChanged(TreeSelectionEvent e) {
               DefaultMutableTreeNode node;
               TreeNode[] nodePath;
               String path = null;

               try {
                  // Get the selected node
                  node = (DefaultMutableTreeNode)
                                        fileTree.getLastSelectedPathComponent();

                  if (node != null) {
                     if ((node.toString().equals("(Empty)") == false) &&
                         (node.toString().equals("(Access Denied)") == false)) {
                        // Get the path components
                        nodePath = node.getPath();

                        // Create a path string
                        for (int i = 0; i < nodePath.length; i++) {
                           if (path != null)
                              path += nodePath[i].toString();
                           else
                              path = nodePath[i].toString();

                           if ((i != 0) && (i != (nodePath.length - 1)))
                              path += fileSeparator;
                        }

                        // Set the selection field with the path string
                        selectionField.setText(path);
                     }
                     else
                        selectionField.setText(null);
                  }
               }
               catch (Exception ex) {
                  OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                               "valueChanged", ex.toString());
               }
            }
         });

         // Create the tree listener which will listen for the user to
         //  expand one of the branches
         fileTree.addTreeWillExpandListener(new TreeWillExpandListener() {
            public void treeWillExpand(TreeExpansionEvent e)
                                                   throws ExpandVetoException {
               if (ignoreExpansionRequests == false)
                  expandBranch(e.getPath());
            }

            public void treeWillCollapse(TreeExpansionEvent e) {}
         });

         // Create the components to hold the file list and current selection
         scrollPane = new JScrollPane(fileTree);
         selectionField = new JTextField();
         selectionLabel = new JLabel("Selection");

         // Create a panel for the file list and selection components
         t3 = new JPanel();
         bl = new BorderLayout();
         bl.setHgap(5);
         t3.setLayout(bl);
         t3.add(selectionLabel, BorderLayout.WEST);
         t3.add(selectionField, BorderLayout.CENTER);
         t2 = new JPanel();
         t2.setLayout(new BorderLayout());
         t2.setBorder(new EmptyBorder(5, 5, 5, 5));
         t2.add(scrollPane, BorderLayout.CENTER);
         t2.add(t3, BorderLayout.SOUTH);

         // Create the add button
         addButton = new JButton("Add Selection");
         addButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         addButton.setPreferredSize(new Dimension(90, 25));
         addButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               addButtonSelected();
  	         }
	      });

         // Create the close button
         closeButton = new JButton("Close");
         closeButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         closeButton.setPreferredSize(new Dimension(53, 25));
         closeButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               dispose();
  	         }
	      });

         // Create a button panel
         t1 = new JPanel();
         t1.add(addButton);
         t1.add(closeButton);

         // Add the components to the window
         getContentPane().add(t2, BorderLayout.CENTER);
         getContentPane().add(t1, BorderLayout.SOUTH);

         setVisible(true);

         setCursorType(Cursor.DEFAULT_CURSOR);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "FileSelectionDialog", e.toString());
      }
   }

   /**
    * This is called when the user clicks on the "Add" button
    */
   private void addButtonSelected()
   {
      String selection;

      try {
         // Get the current selection
         selection = selectionField.getText();

         // If a selection has been made send it to the files tab to be
         //  added to the file list
         if ((selection != null) && ((selection.trim()).length() != 0))
            parentWindow.addFile(selection);
         else
            JOptionPane.showMessageDialog((FSArchive.getHandle()),
                                          "No selection made", null,
                                          JOptionPane.ERROR_MESSAGE);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "addButtonSelected", e.toString());
      }
   }

   /**
    * Creates a series of tree nodes representing the root level files on the
    *  host
    */
   private DefaultMutableTreeNode createInitialBranches()
   {
      String [][] files;
      DefaultMutableTreeNode root = null;
      DefaultMutableTreeNode node;

      try {
         // Get the files at the root directory on the specified host
         files = ServerInterface.getFileList(host, null);

         root = new DefaultMutableTreeNode(new NodeInfo(rootDir, true));

         if (files != null) {

            // Add all the files to the root node
            if (files.length != 0) {
               for (int i = 0; i < files.length; i++) {
                  // Create a node
                  if (files[i][0].equals("D") == true) {
                     node = new DefaultMutableTreeNode(
                                                     new NodeInfo(files[i][1]));

                     // Create a temporary child node so that this node will
                     //  have the "branch" icon displayed
                     node.add(new DefaultMutableTreeNode());
                  }
                  else
                     node = new DefaultMutableTreeNode(
                                                     new NodeInfo(files[i][1]));

                  // Add the node to the root
                  root.add(node);
               }
            }
            else {
               // Create a node that indicates that the directory is empty
               node = new DefaultMutableTreeNode(new NodeInfo("(Empty)"));
               root.add(node);
            }
         }
         else {
            // Create a node that indicates that we may have permission problems
            node = new DefaultMutableTreeNode(new NodeInfo("(Access Denied)"));
            root.add(node);
         }
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "createInitialBranches", e.toString());
      }

      return root;
   }

   /**
    * This is called when the user requests a branch expansion in the tree.
    *  The child files will be retrieve and displayed in the tree. The path
    *  to the branch to be expanded is passed in.
    */
   private void expandBranch(TreePath branchPath)
   {
      DefaultMutableTreeNode parentNode;
      NodeInfo nodeInfo;
      Object[] pathComponents;
      String pathString = null;
      String[][] files;

      try {
         setCursorType(Cursor.WAIT_CURSOR);

         ignoreExpansionRequests = true;

         // See if this branch has already been expanded
         parentNode = (DefaultMutableTreeNode)
                                            (branchPath.getLastPathComponent());
         nodeInfo = (NodeInfo)parentNode.getUserObject();

         // If this branch has not already been expanded, get its children and
         //  add them under this branch
         if (nodeInfo.beenExpanded() == false) {

            // Create the path string
            pathComponents = branchPath.getPath();
            for (int i = 0; i < pathComponents.length; i++) {
               if (pathString != null)
                  pathString += ((DefaultMutableTreeNode)pathComponents[i]).
                                                                     toString();
               else
                  pathString = ((DefaultMutableTreeNode)pathComponents[i]).
                                                                     toString();

               if ((i != 0) && (i != pathComponents.length - 1))
                  pathString += fileSeparator;
            }

            // Get the children for the branch
            files = ServerInterface.getFileList(host, pathString);

            // Remove the temporary child node that was added initialy
            parentNode.removeAllChildren();

            // Add the children to the display
            if (files != null) {

               if (files.length != 0) {
                  for (int i = 0; i < files.length; i++) {
                     if (files[i][0] == "D")
                        addChild(parentNode, files[i][1], true);
                     else
                        addChild(parentNode, files[i][1], false);
                  }
               }
               else
                  addChild(parentNode, "(Empty)", false);
            }
            else
               addChild(parentNode, "(Access Denied)", false);

            // Set the node to indicate that it has now been expanded
            nodeInfo.setBeenExpanded();
         }

         ignoreExpansionRequests = false;

         setCursorType(Cursor.DEFAULT_CURSOR);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "expandBranch", e.toString());
      }
   }

   /**
    * Adds the given child to the parent tree node
    */
   private void addChild(DefaultMutableTreeNode parent, String child,
                         boolean isDirectory)
   {
      DefaultMutableTreeNode childNode;

      try {
         // Create a new node for the tree
         childNode = new DefaultMutableTreeNode(new NodeInfo(child));

         // If the child is a directory entry, create a temporary child node
         //  for it so that this node will have the "branch" icon displayed
         if (isDirectory == true)
            childNode.add(new DefaultMutableTreeNode());

         // Add the child under its parent
         treeModel.insertNodeInto(childNode, parent, parent.getChildCount());

         // Make sure the user can see the new child node
         fileTree.scrollPathToVisible(new TreePath(childNode.getPath()));
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "addChild", e.toString());
      }
   }

   /**
    * Changes the cursor to the specified type
    */
   private void setCursorType(int type)
   {
      try {
         Cursor appCursor = this.getCursor();
         this.setCursor(appCursor.getPredefinedCursor(type));
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "setCursorType", e.toString());
      }
   }
}

/**
 * Keeps some information for a node in the file tree
 */
class NodeInfo
{
   private String text;
   private boolean beenExpanded;

   /**
    * Constructor
    */
   public NodeInfo(String txt)
   {
      text = txt;
      beenExpanded = false;
   }

   /**
    * Constructor
    */
   public NodeInfo(String txt, boolean expanded)
   {
      text = txt;
      beenExpanded = expanded;
   }

   /**
    * Returns a string representation of this node
    */
   public String toString()
   {
      return text;
   }

   /**
    * Returns whether the user has requested expansion of this node yet
    */
   public boolean beenExpanded()
   {
      return beenExpanded;
   }

   /**
    * Sets the "been expanded" member
    */
   public void setBeenExpanded()
   {
      beenExpanded = true;
   }
}
