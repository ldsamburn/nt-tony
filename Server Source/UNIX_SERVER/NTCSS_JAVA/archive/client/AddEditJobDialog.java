/****************************************************************************

               Copyright (c)2002 Northrop Grumman Corporation

                           All Rights Reserved


     This material may be reproduced by or for the U.S. Government pursuant
     to the copyright license under the clause at Defense Federal Acquisition
     Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
****************************************************************************/

package archive.client;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.util.*;
import java.io.*;

import archive.common.*;

/**
 * The dialog the user interacts with to add a new backup job
 */
public class AddEditJobDialog extends JDialog
{
   private Hashtable backupJob;
   private Hashtable originalBackupJob;
   private String[] selectedHosts;
   private JTabbedPane tabbedPane;
   private HostsTab hostsTab;
   private FilesTab  filesTab;
   private ScheduleTab scheduleTab;
   private MiscTab  miscTab;
   private final int HOST_TAB_INDEX = 0;
   private final int FILES_TAB_INDEX = 1;
   private final int SCHEDULE_TAB_INDEX = 2;
   private final int MISC_TAB_INDEX = 3;
   private Hashtable multicastResponseList;
   private String jobName;

   /**
    * Constructor - to be used to set up for backup job creation
    */
   public AddEditJobDialog(Hashtable mcastRspnsList)
   {
      super(FSArchive.getHandle());

      try {
         setTitle("Add Backup Job");

         multicastResponseList = mcastRspnsList;
         originalBackupJob = null;

         jobName = null;
         createDialog(null);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "AddEditJobDialog", e.toString());
      }
   }

   /**
    * Constructor - to be used to set up for backup job editing
    */
   public AddEditJobDialog(Hashtable mcastRspnsList, Hashtable bkupJob,
                           String jbName)
   {
      super(FSArchive.getHandle());

      Vector hostList;
      Enumeration hosts;
      String currentHost;
      int index;

      try {
         setTitle("Edit Backup Job");

         multicastResponseList = mcastRspnsList;
         originalBackupJob = bkupJob;

         hostList = new Vector();
         backupJob = new Hashtable();
         selectedHosts = new String[originalBackupJob.size()];
         index = 0;

         hosts = originalBackupJob.keys();
         while (hosts.hasMoreElements()) {
            currentHost = (String)hosts.nextElement();

            hostList.addElement(currentHost);

            backupJob.put(currentHost,
                    (BackupJobDescription)originalBackupJob.get(currentHost));

            selectedHosts[index++] = currentHost;
         }

         jobName = jbName;
         createDialog(hostList);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "AddEditJobDialog", e.toString());
      }
   }

   /**
    * Creates the dialog and its components
    */
   private void createDialog(Vector specialHosts)
   {
      JPanel buttonPanel;
      JButton applyButton, cancelButton;
      Dimension screenSize;
      int desiredX, desiredY;
      final int windowWidth = 650;
      final int windowHeight = 400;

      try {
         (FSArchive.getHandle()).setCursorType(Cursor.WAIT_CURSOR);

         // Set whether this dialog is modal or not
         setModal(true);
         setResizable(true);

         // Setup the menu bar for this dialog
         setJMenuBar(new JobDialogMenuBar(this));

         // Place the dialog in the middle of the screen
         setSize(new Dimension(windowWidth, windowHeight));
         screenSize = (Toolkit.getDefaultToolkit()).getScreenSize();
         desiredX = (screenSize.width/2) - (windowWidth/2);
         desiredY = (screenSize.height/2) - (windowHeight/2);
         setLocation(new Point(desiredX, desiredY));

         // Create the apply and cancel buttons
         applyButton = new JButton("OK");
         applyButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         applyButton.setPreferredSize(new Dimension(53, 25));
         applyButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               applyButtonSelected();
  	         }
	      });
         cancelButton = new JButton("Cancel");
         cancelButton.setBorder(new BevelBorder(BevelBorder.RAISED));
         cancelButton.setPreferredSize(new Dimension(53, 25));
         cancelButton.addActionListener(new ActionListener() {
	         public void actionPerformed(ActionEvent e) {
               cancelButtonSelected();
  	         }
	      });
         buttonPanel = new JPanel();
         buttonPanel.add(applyButton);
         buttonPanel.add(cancelButton);

         // Create the tabbed pane
         hostsTab = new HostsTab(this, specialHosts, jobName);
         filesTab = new FilesTab(windowWidth, windowHeight);
         scheduleTab = new ScheduleTab(windowWidth, windowHeight);
         miscTab = new MiscTab(windowWidth, windowHeight, multicastResponseList);

         tabbedPane = new JTabbedPane();
         tabbedPane.setBorder(new EmptyBorder(0, 5, 0, 5));
         tabbedPane.add(hostsTab, "Hosts", HOST_TAB_INDEX);
         tabbedPane.add(filesTab, "Files", FILES_TAB_INDEX);
         tabbedPane.add(scheduleTab, "Schedule", SCHEDULE_TAB_INDEX);
         tabbedPane.add(miscTab, "Misc", MISC_TAB_INDEX);
         tabbedPane.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
               tabSelected();
            }
         });

         // Add the components to the dialog
         getContentPane().add(buttonPanel, BorderLayout.SOUTH);
         getContentPane().add(tabbedPane, BorderLayout.CENTER);

         //pack();
         setVisible(true);

         (FSArchive.getHandle()).setCursorType(Cursor.DEFAULT_CURSOR);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "createDialog", e.toString());
      }
   }

   /**
    * Called when the user selects a tab.
    */
   private void tabSelected()
   {
      try {
         // If the selected tab is not the Hosts tab, refresh the selected
         //  tab's host list with the selected hosts and the backupJob
         if (tabbedPane.getSelectedIndex() != HOST_TAB_INDEX)
            ((TabWithList)tabbedPane.getSelectedComponent()).refresh(selectedHosts,
                                                               backupJob);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "tabSelected", e.toString());
      }
   }

   /**
    * Overide some of the window functions
    */
   protected void processWindowEvent(WindowEvent event)
   {
      try {
         if (event.getID() == WindowEvent.WINDOW_CLOSING)
            cancelButtonSelected();
         else
            super.processWindowEvent(event);
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "processWindowEvent", e.toString());
      }
   }

   /**
    * This is called when the user clicks the apply button. The data the user
    *  has enter in will be verified to make sure that it is completly filled
    *  out and it will then be applied. After this is done, the window will
    *  close.
    */
   private void applyButtonSelected()
   {
      try {
         if (JOptionPane.showConfirmDialog(FSArchive.getHandle(),
                                             "Are you sure you want to apply?",
                                             "Apply",
                                             JOptionPane.YES_NO_OPTION) ==
                                             JOptionPane.YES_OPTION) {

            // Make sure all the necessary fields in the backup job are
            //  filled out
            if (validateBackupJob() == true) {

               // Apply the backup job
               applyBackupJob();

               // Close the dialog box
               dispose();

               // Refresh the main screen
               (FSArchive.getHandle()).refresh();
            }
         }
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "applyButtonSelected", e.toString());
      }
   }

   /**
    * Called when the user hits the cancel button or tries to close the window.
    */
   private void cancelButtonSelected()
   {
      try {
         dispose();
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "cancelButtonSelected", e.toString());
      }
   }

   /**
    * Adds an entry for each host given to the backup job.
    */
   public void addHostsToBackupJob(Enumeration hosts)
   {
      try {
         // Create the backup job if it has not already been created
         if (backupJob == null)
            backupJob = new Hashtable();

         // Add an entry for each host in the backup job
         while (hosts.hasMoreElements())
            backupJob.put(hosts.nextElement(),
                                new BackupJobDescription());

         // Get all the hosts in the selected hosts table
         selectedHosts = hostsTab.getSelectedHosts();
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "addHostsToBackupJob", e.toString());
      }
   }

   /**
    * Removes the entry for each host given from the backup job.
    */
   public void removeHostsFromBackupJob(Enumeration hosts)
   {
      try {
         // Remove each host's entry in the backup job
         while (hosts.hasMoreElements())
            backupJob.remove(hosts.nextElement());

         // Get all the hosts in the selected hosts table
         selectedHosts = hostsTab.getSelectedHosts();
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "removeHostsFromBackupJob", e.toString());
      }
   }

   /**
    * This method will make sure all the necessary fields are filled in for the
    *  backup job. If all is filled in correctly, true is returned.
    */
   private boolean validateBackupJob()
   {
      String jobName;
      String message;
      String currentHost = null;
      MulticastResponse response;
      boolean alreadyExists;
      boolean isGroupJob;
      BackupJobDescription currentBkJbDesc;
      int jobCount;
      boolean isCurrentJobGroupType;

      try {
         // Make sure the user has selected atleast one host
         selectedHosts = hostsTab.getSelectedHosts();
         if ((selectedHosts == null) || (selectedHosts.length == 0)) {
            message = "No hosts have been selected";
            JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                          "Error", JOptionPane.ERROR_MESSAGE);
            return false;
         }

         // Make sure the user has filled in the backup job name field
         jobName = hostsTab.getBackupJobName();
         if ((jobName == null) || ((jobName.trim()).length() == 0)) {
            message = "Backup Job Name field left blank";
            JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                          "Error", JOptionPane.ERROR_MESSAGE);
            return false;
         }

         // Make sure that a backup job by the same name and type doesn't
         //  already exist on the selected host(s)
         alreadyExists = false;
         isGroupJob = (selectedHosts.length > 1);
         for (int i = 0; (alreadyExists == false) &&
                         (i < selectedHosts.length); i++) {
            currentHost = selectedHosts[i];
            response =
                     (MulticastResponse)multicastResponseList.get(currentHost);

            // If this is the add version of the dialog (i.e. originalBackupJob
            //  is null), or if this is the edit version of the dialog and the
            //  current host is not apart of the original backup job, run the
            //  check.
            if ((originalBackupJob == null) ||
                (originalBackupJob.containsKey(currentHost) == false)) {

               // Look at each backup job on the current host to see if any
               //  match the backup job name
               jobCount = response.getBackupJobCount();
               for (int j = 0; (alreadyExists == false)&&(j < jobCount); j++) {

                  // Get the type of the current job
                  isCurrentJobGroupType = response.isGroupBackupJob(j);
                  
                  if (((isGroupJob && isCurrentJobGroupType) ||
                       (!isGroupJob && !isCurrentJobGroupType)) &&
                      (jobName.equals(response.getBackupJobName(j)) == true))
                     alreadyExists = true;
               }
            }
         }
         if (alreadyExists == true) {
            message = "Job name already exists on " + currentHost;
            JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                          "Error", JOptionPane.ERROR_MESSAGE);
            return false;
         }

         // Check the files, schedule, and misc data for each host
         for (int i = 0; i < selectedHosts.length; i++) {
            currentBkJbDesc = (BackupJobDescription)backupJob.
                                                          get(selectedHosts[i]);

            // Make sure files have been selected on this host
            if (currentBkJbDesc.isFilesSet() == false) {
               message = "No files selected for " + selectedHosts[i];
               JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                            "Error", JOptionPane.ERROR_MESSAGE);
               return false;
            }

            // Make sure the schedule has been filled in for this host
            if (currentBkJbDesc.isDaysSet() == false) {
               message = "No days selected for " + selectedHosts[i];
               JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                            "Error", JOptionPane.ERROR_MESSAGE);
               return false;
            }

            // Make sure a device has been selected for this host
            if (currentBkJbDesc.isDeviceSet() == false) {
               message = "No device selected for " + selectedHosts[i];
               JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                            "Error", JOptionPane.ERROR_MESSAGE);
               return false;
            }

            // Make sure that if a message type is selected, that an email
            //  address is also specified
            if ((currentBkJbDesc.getMessageType() != Constants.NO_MESSAGE) &&
                (currentBkJbDesc.isEmailSet() == false)) {
               message = "No email address specified for " + selectedHosts[i];
               JOptionPane.showMessageDialog((FSArchive.getHandle()), message,
                                            "Error", JOptionPane.ERROR_MESSAGE);
               return false;
            }
         }


         // Set the job name and type fields for all the hosts
         for (int i = 0; i < selectedHosts.length; i++) {
            currentBkJbDesc = (BackupJobDescription)backupJob.
                                                          get(selectedHosts[i]);

            // Set the job name field
            currentBkJbDesc.setJobName(jobName);

            // Set the type field
            if (selectedHosts.length > 1)
               currentBkJbDesc.setJobType(Constants.GROUP_TYPE_VALUE);
            else
               currentBkJbDesc.setJobType(Constants.SINGLE_TYPE_VALUE);
         }
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "validateBackupJob", e.toString());
      }

      return true;
   }

   /**
    * Sends the backup job to each host that it applies to
    */
   private void applyBackupJob()
   {
      Enumeration hosts;
      String host;
      BackupJobDescription bkJbDesc;

      try {
         hosts = backupJob.keys();
         while(hosts.hasMoreElements()) {

            // Get the next host
            host = (String)hosts.nextElement();

            // Get that host's version of the backup job
            bkJbDesc = (BackupJobDescription)backupJob.get(host);

            // If this is the add dialog, then send an ADD message to the
            //  server. Else, this must be the edit dialog, so if the host
            //  is in the original backup, send an EDIT message; otherwise,
            //  send an ADD message.
            if (originalBackupJob == null)  // This is the add dialog
               ServerInterface.addBackupJob(host, bkJbDesc);
            else if (originalBackupJob.containsKey(host) == true)
               ServerInterface.editBackupJob(host, bkJbDesc);
            else
               ServerInterface.addBackupJob(host, bkJbDesc);
         }

         // If this is the edit dialog , check to see if any hosts in the
         //  original backup job don't occur in the final version of the backup
         //  job. If they don't occur in this final version, then the user must
         //  have removed them from the job. If this is the case, send a DELETE
         //  message to those host.
         if (originalBackupJob != null) {  // i.e. this is the edit dialog

            hosts = originalBackupJob.keys();
            while(hosts.hasMoreElements()) {

               // Get the next host
               host = (String)hosts.nextElement();

               // If the host is not in the final version of the backup job,
               //  then send DELETE message
               if (backupJob.containsKey(host) == false) {

                  // Get that host's version of the backup job
                  bkJbDesc = (BackupJobDescription)originalBackupJob.get(host);

                  // Inform the server to delete its copy of the backup job
                  ServerInterface.deleteBackupJob(host, bkJbDesc.getJobName());
               }
            }
         }
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "applyBackupJob", e.toString());
      }
   }

   /**
    * Gets from the tab that is displayed the currently selected host.
    */
   public String getCurrentHost()
   {
      String currentHost = null;

      try {
         currentHost = ((TabBase)tabbedPane.getSelectedComponent()).
                                                          getHighlightedHost();
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "getCurrentHost", e.toString());
      }

      return currentHost;
   }

   /**
    * Gets the hosts selected for this backup job
    */
   public String[] getSelectedHosts()
   {
      try {
         return selectedHosts;
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "getSelectedHosts", e.toString());
         return null;
      }
   }

   /**
    *  Copies "fromHost"'s version of the backupJob to the hosts specified.
    */
   public void copyJob(String fromHost, String[] toHosts)
   {
      BackupJobDescription desc;
      String rawJob;

      try {
         // Get the backup job description for the "from" host
         desc = (BackupJobDescription)backupJob.get(fromHost);
         rawJob = desc.getRawBackupJobDescription();

         // Copy the description the the hosts specified
         for (int i = 0; i < toHosts.length; i++)
            backupJob.put(toHosts[i], new BackupJobDescription(rawJob));
      }
      catch (Exception e) {
         OutputLog.log(OutputLog.EXCP, this.getClass().getName(),
                       "copyJob", e.toString());
      }
   }
}

