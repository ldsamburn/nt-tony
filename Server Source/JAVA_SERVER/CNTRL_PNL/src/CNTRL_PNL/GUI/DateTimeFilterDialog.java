/*
      Copyright(c)2002 Northrop Grumman Corporation

                All Rights Reserved

  This material may be reproduced by or for the U.S. Government pursuant to 
  the copyright license under the clause at Defense Federal Acquisition 
  Regulation Supplement (DFARS) 252.227-7013 (NOV 1995).
*/

package CNTRL_PNL.GUI;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.plaf.metal.*;


public class DateTimeFilterDialog extends JDialog
{
  private JRadioButton beforeRadioButton;
  private JRadioButton betweenRadioButton;
  private JRadioButton startRadioButton;
  private JRadioButton endRadioButton;
  private JRadioButton invisibleRadioButton;
  private JRadioButton afterRadioButton;
  private ButtonGroup group1, group2;
  private boolean okSelected = false;

  private DateField currentDateField;
  private DateField beforeDateField;
  private DateField startDateField;
  private DateField endDateField;
  private DateField afterDateField;

  private int dateFilterCode;

  public DateTimeFilterDialog()
  {
    super(NCPFrame.getHandle(), "Date Filter", true);

	  JPanel container = new JPanel();
	  JPanel filters = buildFilterPanel();
	  JPanel buttonPanel = buildButtonPanel();

	  container.setLayout( new BorderLayout() );
  	container.setBorder(BorderFactory.createTitledBorder(""));
    container.setPreferredSize(new Dimension(530, 300));
	  container.add(filters, BorderLayout.CENTER);
	  container.add(buttonPanel, BorderLayout.SOUTH);
	  getContentPane().add(container);
	  pack();
	  centerDialog();
    this.setVisible(true);
  }

  // Returns the value of the dateFilterCode member
  public int getDateFilterCode()
  {
     return dateFilterCode;
  }

  // Returns the date on which to start the filter
  public String getFilterStartDate()
  {
     String filterStartDate;

     switch (dateFilterCode) {
        case 1:
           filterStartDate = beforeDateField.toIntString();
           break;
        case 2:
           filterStartDate = startDateField.toIntString();
           break;
        default:
           filterStartDate = "not set";
           break;
     }

     return filterStartDate;
  }

  // Returns the date on which to end the filter
  public String getFilterEndDate()
  {
     String filterEndDate;

     switch (dateFilterCode) {
        case 2:
           filterEndDate = endDateField.toIntString();
           break;
        case 3:
           filterEndDate = afterDateField.toIntString();
           break;
        default:
           filterEndDate = "not set";
           break;
     }

     return filterEndDate;
  }

  // Builds the panel which contains all the filter components (date fields,
  //  before between and after buttons, and all the up/down buttons)
  private JPanel buildFilterPanel()
  {
	  JPanel filters = new JPanel();
    JPanel dateFieldsPanel = new JPanel();
    JPanel upDownButtonPanel = new JPanel();
    JPanel betweenFilter = new JPanel();

  	group1 = new ButtonGroup();
    group2 = new ButtonGroup();

    beforeRadioButton = new JRadioButton("Before", true);
    beforeRadioButton.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
         currentDateField = beforeDateField;
         dateFilterCode = 1;

         startRadioButton.setEnabled(false);
         endRadioButton.setEnabled(false);
         invisibleRadioButton.setSelected(true);
      }
	  });

    betweenRadioButton = new JRadioButton("Between");
    betweenRadioButton.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
         currentDateField = null;
         dateFilterCode = 2;

         startRadioButton.setEnabled(true);
         endRadioButton.setEnabled(true);
         invisibleRadioButton.setSelected(true);

      }
	  });

    startRadioButton = new JRadioButton();
    startRadioButton.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
         currentDateField = startDateField;
      }
	  });

    endRadioButton = new JRadioButton();
    endRadioButton.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
         currentDateField = endDateField;
      }
	  });

    invisibleRadioButton = new JRadioButton();
    invisibleRadioButton.setSelected(true);

    afterRadioButton = new JRadioButton("After");
    afterRadioButton.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
         currentDateField = afterDateField;
         dateFilterCode = 3;

         startRadioButton.setEnabled(false);
         endRadioButton.setEnabled(false);
         invisibleRadioButton.setSelected(true);
      }
	  });

	  JButton jButton1 = new JButton("UP");
	  jButton1.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				monthUpPressed();
			}
    });

	  JButton jButton2 = new JButton("DOWN");
	  jButton2.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				monthDownPressed();
			}
    });

	  JButton jButton3 = new JButton("UP");
	  jButton3.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				dayUpPressed();
			}
    });

	  JButton jButton4 = new JButton("DOWN");
	  jButton4.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				dayDownPressed();
			}
    });

	  JButton jButton5 = new JButton("UP");
	  jButton5.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				hourUpPressed();
			}
    });

	  JButton jButton6 = new JButton("DOWN");
	  jButton6.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				hourDownPressed();
			}
    });

	  JButton jButton7 = new JButton("UP");
	  jButton7.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				minuteUpPressed();
			}
    });

	  JButton jButton8 = new JButton("DOWN");
	  jButton8.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				minuteDownPressed();
			}
    });

	  JButton jButton9 = new JButton("UP");
	  jButton9.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				secondUpPressed();
			}
    });

	  JButton jButton10 = new JButton("DOWN");
	  jButton10.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				secondDownPressed();
			}
    });

    group1.add(beforeRadioButton);
    group1.add(betweenRadioButton);
    group1.add(afterRadioButton);

    group2.add(startRadioButton);
    group2.add(endRadioButton);
    group2.add(invisibleRadioButton);

    beforeDateField = new DateField(SwingConstants.RIGHT);
    startDateField = new DateField(SwingConstants.LEFT);
    endDateField = new DateField(SwingConstants.LEFT);
    afterDateField = new DateField(SwingConstants.RIGHT);

    currentDateField = beforeDateField;
    dateFilterCode = 1;

    // Add the components to the filter panel
    filters.setLayout(new BorderLayout());

    dateFieldsPanel.setLayout(new GridLayout(3, 4));

    dateFieldsPanel.add(new JLabel("  "));
    dateFieldsPanel.add(beforeRadioButton);
    dateFieldsPanel.add(beforeDateField);
    dateFieldsPanel.add(new JLabel("  "));

    betweenFilter.setLayout(new BorderLayout());
    JPanel radioButtonPanel = new JPanel();
    radioButtonPanel.setLayout(new GridLayout(2,0));
    radioButtonPanel.add(startRadioButton);
    radioButtonPanel.add(endRadioButton);
    JPanel dateFieldPanel = new JPanel();
    dateFieldPanel.setLayout(new GridLayout(2,0));
    dateFieldPanel.add(startDateField);
    dateFieldPanel.add(endDateField);
    betweenFilter.add(radioButtonPanel, BorderLayout.WEST);
    betweenFilter.add(dateFieldPanel, BorderLayout.CENTER);

    dateFieldsPanel.add(new JLabel("  "));
    dateFieldsPanel.add(betweenRadioButton);
    dateFieldsPanel.add(betweenFilter);
    dateFieldsPanel.add(new JLabel("  "));

    dateFieldsPanel.add(new JLabel("  "));
    dateFieldsPanel.add(afterRadioButton);
    dateFieldsPanel.add(afterDateField);
    dateFieldsPanel.add(new JLabel("  "));

    filters.add(dateFieldsPanel, BorderLayout.CENTER);

    upDownButtonPanel.setLayout(new GridLayout(3, 5));

    upDownButtonPanel.add(new JLabel("Month", SwingConstants.CENTER));
    upDownButtonPanel.add(new JLabel("Day", SwingConstants.CENTER));
    upDownButtonPanel.add(new JLabel("Hour", SwingConstants.CENTER));
    upDownButtonPanel.add(new JLabel("Minutes", SwingConstants.CENTER));
    upDownButtonPanel.add(new JLabel("Seconds", SwingConstants.CENTER));

    upDownButtonPanel.add(jButton1);
    upDownButtonPanel.add(jButton3);
    upDownButtonPanel.add(jButton5);
    upDownButtonPanel.add(jButton7);
    upDownButtonPanel.add(jButton9);

    upDownButtonPanel.add(jButton2);
    upDownButtonPanel.add(jButton4);
    upDownButtonPanel.add(jButton6);
    upDownButtonPanel.add(jButton8);
    upDownButtonPanel.add(jButton10);

    filters.add(upDownButtonPanel, BorderLayout.SOUTH);

	  return filters;
  }

  // Builds the panel which contains the OK and cancel buttons
  private JPanel buildButtonPanel()
  {
	  JPanel buttons = new JPanel();
	  buttons.setLayout ( new FlowLayout(FlowLayout.CENTER) );

	  JButton cancel = new JButton("Cancel");
	  cancel.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				cancelPressed();
			}
    });

	  JButton ok = new JButton("OK");
	  ok.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
				OKPressed();
			}
    });

	  buttons.add( ok );
    buttons.add( cancel );
	  getRootPane().setDefaultButton(ok);
    return buttons;
  }

  // Places the dialog in the center of the screen
  private void centerDialog()
  {
    Dimension screenSize = this.getToolkit().getScreenSize();
	  Dimension size = this.getSize();
  	screenSize.height = screenSize.height/2;
	  screenSize.width = screenSize.width/2;
  	size.height = size.height/2;
	  size.width = size.width/2;
  	int y = screenSize.height - size.height;
	  int x = screenSize.width - size.width;
  	this.setLocation(x,y);
  }

  // Called when the cancel button is pressed
  private void cancelPressed()
  {
    okSelected = false;
    this.setVisible(false);
  }

  // Called when the OK button is pressed
  private void OKPressed()
  {
    okSelected = true;
    this.setVisible(false);
  }

  // Increments the current date field's month by one
  private void monthUpPressed()
  {
     if (currentDateField != null) {
        currentDateField.incrementMonth();
        currentDateField.repaint();
     }
  }

  // Decrements the current date field's month by one
  private void monthDownPressed()
  {
     if (currentDateField != null) {
        currentDateField.decrementMonth();
        currentDateField.repaint();
     }
  }

  // Increments the current date field's day by one
  private void dayUpPressed()
  {
     if (currentDateField != null) {
        currentDateField.incrementDay();
        currentDateField.repaint();
     }
  }

  // Decrements the current date field's day by one
  private void dayDownPressed()
  {
     if (currentDateField != null) {
        currentDateField.decrementDay();
        currentDateField.repaint();
     }
  }

  // Increments the current date field's hours by one
  private void hourUpPressed()
  {
     if (currentDateField != null) {
        currentDateField.incrementHours();
        currentDateField.repaint();
     }
  }

  // Decrements the current date field's hours by one
  private void hourDownPressed()
  {
     if (currentDateField != null) {
        currentDateField.decrementHours();
        currentDateField.repaint();
     }
  }

  // Increments the current date field's minutes by one
  private void minuteUpPressed()
  {
     if (currentDateField != null) {
        currentDateField.incrementMinutes();
        currentDateField.repaint();
     }
  }

  // Decrements the current date field's minutes by one
  private void minuteDownPressed()
  {
     if (currentDateField != null) {
        currentDateField.decrementMinutes();
        currentDateField.repaint();
     }
  }

  // Increments the current date field's seconds by one
  private void secondUpPressed()
  {
     if (currentDateField != null) {
        currentDateField.incrementSeconds();
        currentDateField.repaint();
     }
  }

  // Decrements the current date field's seconds by one
  private void secondDownPressed()
  {
     if (currentDateField != null) {
        currentDateField.decrementSeconds();
        currentDateField.repaint();
     }
  }

  // Returns true if the OK button was pressed, or false if the cancel
  //  button was pressed
  public boolean wasOkSelected()
  {
    return okSelected;
  }

}

// This class is a label component which displays a date. The date value can
//  be modified using the increment/decrement field methods.
class DateField extends JLabel
{
   private int month, day, hours, minutes, seconds;

   public DateField()
   {
      // Default the date to Jan 1 12:00:00
      month   = 1;
      day     = 1;
      hours   = 12;
      minutes = 0;
      seconds = 0;

      setText(toString());
   }

   public DateField(int horizontalAlignment)
   {
      super("", horizontalAlignment);

      // Default the date to Jan 1 12:00:00
      month   = 1;
      day     = 1;
      hours   = 12;
      minutes = 0;
      seconds = 0;

      setText(toString());
   }

   // Increments the month field
   public void incrementMonth()
   {
      month = month == 12 ? 1 : month + 1;
      setText(toString());
   }

   // Decrements the month field
   public void decrementMonth()
   {
      month = month == 1 ? 12 : month - 1;
      setText(toString());
   }

   // Increments the day field
   public void incrementDay()
   {
      day = day == 31 ? 1 : day + 1;
      setText(toString());
   }

   // Decrements the day field
   public void decrementDay()
   {
      day = day == 1 ? 31 : day - 1;
      setText(toString());
   }

   // Increments the hours field
   public void incrementHours()
   {
      hours = hours == 23 ? 0 : hours + 1;
      setText(toString());
   }

   // Decrements the hours field
   public void decrementHours()
   {
      hours = hours == 0 ? 23 : hours - 1;
      setText(toString());
   }

   // Increments the minutes field
   public void incrementMinutes()
   {
      minutes = minutes == 59 ? 0 : minutes + 1;
      setText(toString());
   }

   // Decrements the minutes field
   public void decrementMinutes()
   {
      minutes = minutes == 0 ? 59 : minutes - 1;
      setText(toString());
   }

   // Increments the seconds field
   public void incrementSeconds()
   {
      seconds = seconds == 59 ? 0 : seconds + 1;
      setText(toString());
   }

   // Decrements the seconds field
   public void decrementSeconds()
   {
      seconds = seconds == 0 ? 59 : seconds - 1;
      setText(toString());
   }

   // Returns the formatted string representation of this date field
   public String toString()
   {
      String formattedDateField = "";

      // Add the month  field
      switch (month) {
         case 1:
            formattedDateField = "Jan  ";
            break;
         case 2:
            formattedDateField = "Feb  ";
            break;
         case 3:
            formattedDateField = "Mar  ";
            break;
         case 4:
            formattedDateField = "Apr  ";
            break;
         case 5:
            formattedDateField = "May  ";
            break;
         case 6:
            formattedDateField = "June ";
            break;
         case 7:
            formattedDateField = "July ";
            break;
         case 8:
            formattedDateField = "Aug  ";
            break;
         case 9:
            formattedDateField = "Sept ";
            break;
         case 10:
            formattedDateField = "Oct  ";
            break;
         case 11:
            formattedDateField = "Nov  ";
            break;
         case 12:
            formattedDateField = "Dec  ";
            break;
      }

      // Add the day field
      if (day < 10)
         formattedDateField += "0" + String.valueOf(day) + " ";
      else
         formattedDateField += String.valueOf(day) + " ";

      // Add the hours field
      if (hours < 10)
         formattedDateField += "0" + String.valueOf(hours) + ":";
      else
         formattedDateField += String.valueOf(hours) + ":";

      // Add the minutes field
      if (minutes < 10)
         formattedDateField += "0" + String.valueOf(minutes) + ":";
      else
         formattedDateField += String.valueOf(minutes) + ":";

      // Add the seconds field
      if (seconds < 10)
         formattedDateField += "0" + String.valueOf(seconds);
      else
         formattedDateField += String.valueOf(seconds);

      return formattedDateField;
   }

   // Returns the date field as a String represented integer
   public String toIntString()
   {
      String field;

      // Add the month field
      if (month < 10)
         field = "0" + String.valueOf(month);
      else
         field = String.valueOf(month);

      // Add the day field
      if (day < 10)
         field += "0" + String.valueOf(day);
      else
         field += String.valueOf(day);

      // Add the hours field
      if (hours < 10)
         field += "0" + String.valueOf(hours);
      else
         field += String.valueOf(hours);

      // Add the mintues field
      if (minutes < 10)
         field += "0" + String.valueOf(minutes);
      else
         field += String.valueOf(minutes);

      // Add the seconds field
      if (seconds < 10)
         field += "0" + String.valueOf(seconds);
      else
         field += String.valueOf(seconds);

      return field;
   }
}

