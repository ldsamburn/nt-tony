/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/

package archive.client;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

import archive.common.*;

/**
 * This is the files tab in the tab pane
 */
public class FilesTab extends TabWithList
{
   private JButton addButton, deleteButton;
   private JList fileList;

   /**
    * Constructor
    */
   public FilesTab(int windowWidth, int windowHeight)
   {
      // Setup the basic look of the tab
      super(windowWidth, windowHeight);

      JPanel rightComponent;
      JPanel t1, t2;
      JScrollPane scrollPane;
      JLabel filesLabel;

      try {
         // Create the add button and its listener
         addButton = new JButton("Add");
         addButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         addButton.setPreferredSize(new Dimension(53, 25));
         addButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               addButtonSelected();
  	         }
	      });

         // Create the delete button
         deleteButton = new JButton("Delete");
         deleteButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         deleteButton.setPreferredSize(new Dimension(53, 25));
         deleteButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               Object[] candidates;

               candidates = fileList.getSelectedValues();
               for (int i = 0; i < candidates.length; i++)
                  // Remove the file to the current back up job
                  currentBkJbDesc.removeFile((String)candidates[i]);

               // Repaint the file list
               fileList.setListData(currentBkJbDesc.getFiles());
               fileList.repaint();
  	         }
	      });

         // Create the file list
         fileList = new JList();
         scrollPane = new JScrollPane(fileList,
                                    JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                    JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
         //scrollPane.setPreferredSize(new Dimension(windowWidth/2, windowHeight));
         scrollPane.setPreferredSize(new Dimension(250, 240));
         scrollPane.setBorder(new SoftBevelBorder(SoftBevelBorder.LOWERED));

         t1 = new JPanel();
         t1.add(addButton);
         t1.add(deleteButton);
         t2 = new JPanel();
         t2.add(scrollPane);

         // Create the component that is to populate the right side of the tab
         //  and add the panels to it
         rightComponent = new JPanel();
         rightComponent.setBorder(new EmptyBorder(8, 0, 0, 0));
         rightComponent.setLayout(new BorderLayout());
         filesLabel = new JLabel("Backup File List", JLabel.CENTER);
         filesLabel.setFont(new Font("Sanserif", Font.BOLD, 12));
         rightComponent.add(filesLabel, BorderLayout.NORTH);
         rightComponent.add(t2, BorderLayout.CENTER);
         rightComponent.add(t1, BorderLayout.SOUTH);

         // Add the component to the right side of the tab
         addRightComponent(rightComponent);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "FilesTab", e.toString());
      }
   }

   /**
    * This function is called when a host is selected in the host list. The
    *  host that was selected is passed in. It will set the displayed
    *  components to the selected host's data.
    */
   protected void hostSelected(String host)
   {
      Vector files;

      try {
         super.hostSelected(host);

         // Fill in the file list with the list of selected backup files for
         //  the given host
         files = currentBkJbDesc.getFiles();
         if (files == null)
            fileList.setListData(new Vector());
         else
            fileList.setListData(files);
         fileList.repaint();

         // Enable the add/delete buttons
         addButton.setEnabled(true);
         deleteButton.setEnabled(true);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "hostSelected", e.toString());
      }
   }

   /**
    * This refreshes the displayed list of selected hosts with the given
    *  host list
    */
   public void refresh(String[] selectedHosts, Hashtable table)
   {
      try {
         // This will cause the host list to be refreshed with any new hosts
         //  the user has selected
         super.refresh(selectedHosts, table);

         // Clear the file list
         fileList.setListData(new Vector());
         fileList.repaint();

         // Disable the add/delete buttons
         addButton.setEnabled(false);
         deleteButton.setEnabled(false);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "refresh", e.toString());
      }
   }

   /**
    * Called when the user clicks on the "add" button. This will display a
    *  file selection dialog
    */
   private void addButtonSelected()
   {
      try {
         FileSelectionDialog dialog =
                                   new FileSelectionDialog(this,
                                                         getHighlightedHost());
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "addButtonSelected", e.toString());
      }
   }

   /**
    * This is called from the file selection dialog when the user wants to
    *  add a file to the list
    */
   public void addFile(String selection)
   {
      try {
         // Check to see if the file is already in the list
         if (currentBkJbDesc.containsFile(selection) == true)
            JOptionPane.showMessageDialog((FSArchive.getHandle()),
                                          "Selection already in list", null,
                                          JOptionPane.ERROR_MESSAGE);
         else {
            // Add the file to the dissplayed list
            currentBkJbDesc.addFile(selection);
            fileList.setListData(currentBkJbDesc.getFiles());
            fileList.repaint();
         }
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "addFile", e.toString());
      }
   }
}
